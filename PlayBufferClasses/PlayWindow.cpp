//********************************************************************************************************************************
// File:		PlayWindow.cpp
// Description:	Platform specific code to provide a window to draw into
// Platform:	Windows
// Notes:		Uses a 32-bit ARGB display buffer
//********************************************************************************************************************************
#include "PlayBufferPCH.h"

// Instruct Visual Studio to add these to the list of libraries to link
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")

PlayWindow* PlayWindow::s_pInstance = nullptr;

// External functions which must be implemented by the user 
extern void MainGameEntry( int argc, char* argv[] ); 
extern bool MainGameUpdate( float ); // Called every frame
extern int MainGameExit( void ); // Called on quit

ULONG_PTR g_pGDIToken = 0;

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	// Initialize GDI+
	Gdiplus::GdiplusStartupInput startupInput;
	ULONG_PTR token;
	Gdiplus::Status gdiStatus = Gdiplus::GdiplusStartup( &token, &startupInput, NULL );
	PLAY_ASSERT( Gdiplus::Ok == gdiStatus );
	g_pGDIToken = token;

	MainGameEntry( __argc, __argv );

	return PlayWindow::Instance().HandleWindows( hInstance, hPrevInstance, lpCmdLine, nShowCmd, L"PlayBuffer" );
}

//********************************************************************************************************************************
// Constructor / Destructor (Private)
//********************************************************************************************************************************

PlayWindow::PlayWindow( PixelData* pDisplayBuffer, int nScale )
{
	PLAY_ASSERT( pDisplayBuffer );
	PLAY_ASSERT( nScale > 0 );
	m_pPlayBuffer = pDisplayBuffer;
	m_scale = nScale;
}

PlayWindow::~PlayWindow( void )
{
	s_pInstance = nullptr;
}

//********************************************************************************************************************************
// Instance functions
//********************************************************************************************************************************

PlayWindow& PlayWindow::Instance()
{
	if( !s_pInstance )
		PLAY_ASSERT_MSG( false, "Trying to use PlayBuffer without initialising it!" );

	return *s_pInstance;
}

PlayWindow& PlayWindow::Instance( PixelData* pDisplayBuffer, int nScale )
{
	PLAY_ASSERT_MSG( !s_pInstance, "Trying to create multiple instances of singleton class!" );
	s_pInstance = new PlayWindow( pDisplayBuffer, nScale );
	return *s_pInstance;
}

void PlayWindow::Destroy()
{
	PLAY_ASSERT_MSG( s_pInstance, "Trying to use destroy PlayBuffer which hasn't been instanced!" );
	delete s_pInstance;
	s_pInstance = nullptr;
}

//********************************************************************************************************************************
// Windows functions
//********************************************************************************************************************************

int PlayWindow::HandleWindows( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow, LPCWSTR windowName )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PlayWindow::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
	wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = windowName;
	wcex.hIconSm = LoadIcon( wcex.hInstance, IDI_APPLICATION );

	RegisterClassExW( &wcex );

	int	w = m_pPlayBuffer->width * m_scale;
	int h = m_pPlayBuffer->height * m_scale;

	UINT dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	RECT rect = { 0, 0, w, h }; 
	AdjustWindowRect( &rect, dwStyle, FALSE );
	HWND hWnd = CreateWindowW( windowName, windowName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr );

	if( !hWnd )
		return FALSE;

	m_hWindow = hWnd;

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	HACCEL hAccelTable = LoadAccelerators( hInstance, windowName );

	LARGE_INTEGER frequency;        
	double elapsedTime = 0.0;
	static LARGE_INTEGER lastDrawTime;
	LARGE_INTEGER now;

	MSG msg{};
	bool quit = false;

	// Set up counters for timing the frame
	QueryPerformanceCounter( &lastDrawTime );
	QueryPerformanceFrequency( &frequency );

	// Standard windows message loop
	while( !quit )
	{
		// Hangle windows messages
		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			if( msg.message == WM_QUIT )
				break;

			if( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}

		do
		{
			QueryPerformanceCounter( &now );
			elapsedTime = ( now.QuadPart - lastDrawTime.QuadPart ) * 1000.0 / frequency.QuadPart;

		} while( elapsedTime < 1000.0f / FRAMES_PER_SECOND );

		// Call the main game update function (only while we have the input focus in release mode)
#ifndef _DEBUG
		if( GetFocus() == m_hWindow )
#endif
			quit = MainGameUpdate( static_cast<float>( elapsedTime ) / 1000.0f );
		
		lastDrawTime = now;

		DwmFlush(); // Waits for DWM compositor to finish
	}

	// Call the main game cleanup function
	MainGameExit();

	PLAY_ASSERT( g_pGDIToken );
	Gdiplus::GdiplusShutdown( g_pGDIToken );

	return static_cast<int>( msg.wParam );
}

LRESULT CALLBACK PlayWindow::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_PAINT:
			PAINTSTRUCT ps;
			BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
		case WM_LBUTTONDOWN:
			if( s_pInstance->m_pMouseData )
				s_pInstance->m_pMouseData->left = true;
			break;
		case WM_LBUTTONUP:
			if( s_pInstance->m_pMouseData )
				s_pInstance->m_pMouseData->left = false;
			break;
		case WM_RBUTTONDOWN:
			if( s_pInstance->m_pMouseData )
				s_pInstance->m_pMouseData->right = true;
			break;
		case WM_RBUTTONUP:
			if( s_pInstance->m_pMouseData )
				s_pInstance->m_pMouseData->right = false;
			break;
		case WM_MOUSEMOVE:
			if( s_pInstance->m_pMouseData )
			{
				s_pInstance->m_pMouseData->pos.x = static_cast<float>( GET_X_LPARAM( lParam ) / s_pInstance->m_scale );
				s_pInstance->m_pMouseData->pos.y = static_cast<float>( GET_Y_LPARAM( lParam ) / s_pInstance->m_scale );
			}
			break;
		case WM_MOUSELEAVE:
			s_pInstance->m_pMouseData->pos.x = -1;
			s_pInstance->m_pMouseData->pos.y = -1;
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

double PlayWindow::Present( void )
{
	LARGE_INTEGER frequency;
	LARGE_INTEGER before;
	LARGE_INTEGER after;
	QueryPerformanceCounter( &before );
	QueryPerformanceFrequency( &frequency );

	// Set up a BitmapInfo structure to represent the pixel format of the display buffer
	BITMAPINFOHEADER bitmap_info_header
	{
			sizeof( BITMAPINFOHEADER ),								// size of its own data,
			m_pPlayBuffer->width, m_pPlayBuffer->height,		// width and height
			1, 32, BI_RGB,				// planes must always be set to 1 (docs), 32-bit pixel data, uncompressed 
			0, 0, 0, 0, 0				// rest can be set to 0 as this is uncompressed and has no palette
	};

	BITMAPINFO bitmap_info{ bitmap_info_header, { 0,0,0,0 } };	// No palette data required for this bitmap

	HDC hDC = GetDC( m_hWindow );

	// Copy the display buffer to the window: GDI only implements up scaling using simple pixel duplication, but that's what we want
	// Note that GDI+ DrawImage would do the same thing, but it's much slower! 
	StretchDIBits( hDC, 0, 0, m_pPlayBuffer->width * m_scale, m_pPlayBuffer->height * m_scale, 0, m_pPlayBuffer->height + 1, m_pPlayBuffer->width, -m_pPlayBuffer->height, m_pPlayBuffer->pPixels, &bitmap_info, DIB_RGB_COLORS, SRCCOPY ); // We flip h because Bitmaps store pixel data upside down.
	
	ReleaseDC( m_hWindow, hDC );

	QueryPerformanceCounter( &after );

	double elapsedTime = ( after.QuadPart - before.QuadPart ) * 1000.0 / frequency.QuadPart;

	return elapsedTime;
}

//********************************************************************************************************************************
// Loading functions
//********************************************************************************************************************************

int PlayWindow::ReadPNGImage( std::string& fileAndPath, int& width, int& height )
{
	// Convert filename from single to wide string for GDI+ compatibility
	size_t newsize = strlen( fileAndPath.c_str() ) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s( &convertedChars, wcstring, newsize, fileAndPath.c_str(), _TRUNCATE );

	HBITMAP hBitmap = NULL;

	// Use GDI+ to load file into Bitmap structure
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile( wcstring, false );
	int status = bitmap->GetLastStatus();

	if( status != Gdiplus::Ok )
		return -status;

	bitmap->GetHBITMAP( Gdiplus::Color( 255, 255, 255 ), &hBitmap );

	width = bitmap->GetWidth();
	height = bitmap->GetHeight();

	delete bitmap;
	delete[] wcstring;

	return 1;
}

int PlayWindow::LoadPNGImage( std::string& fileAndPath, PixelData& destImage )
{
	// Convert filename from single to wide string for GDI+ compatibility
	size_t newsize = strlen( fileAndPath.c_str() ) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s( &convertedChars, wcstring, newsize, fileAndPath.c_str(), _TRUNCATE );

	HBITMAP hBitmap = NULL;

	// Use GDI+ to load file into Bitmap structure
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile( wcstring, false );
	int status = bitmap->GetLastStatus();

	if( status != Gdiplus::Ok )
		return -status;

	bitmap->GetHBITMAP( Gdiplus::Color( 255, 255, 255 ), &hBitmap );

	// Create BitmapData structure to pull the data into
	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;

	destImage.width = bitmap->GetWidth();
	destImage.height = bitmap->GetHeight();

	// Lock the bitmap in advance of reading its data
	Gdiplus::Rect rect( 0, 0, destImage.width, destImage.height );
	bitmap->LockBits( &rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData );

	// Get a pointer to the actual bit data
	Pixel* sourcePixels = static_cast<Pixel*>( bitmapData->Scan0 );

	destImage.pPixels = new Pixel[destImage.width * destImage.height];
	memset( destImage.pPixels, 0, sizeof( Pixel ) * destImage.width * destImage.height );

	Pixel* destPixels = destImage.pPixels;

	// Copy the data across
	for( int b = 0; b < destImage.width * destImage.height; b++ )
		*destPixels++ = *sourcePixels++;

	//Unlock the bitmap
	bitmap->UnlockBits( bitmapData );

	delete bitmap;
	delete bitmapData;
	delete[] wcstring;

	return 1;
}

//********************************************************************************************************************************
// Miscellaneous functions
//********************************************************************************************************************************

void AssertFailMessage( const char* message, const char* file, long line )
{
	// file - the file in which the assertion failed ( __FILE__ )
	// line - the line of code where the assertion failed ( __LINE__ )
	std::filesystem::path p = file;
	std::string s = p.filename().string() + " : LINE " + std::to_string( line );
	s += "\n" + std::string( message );
	int wide_count = MultiByteToWideChar( CP_UTF8, 0, s.c_str(), -1, NULL, 0 );
	wchar_t* wide = new wchar_t[wide_count];
	MultiByteToWideChar( CP_UTF8, 0, s.c_str(), -1, wide, wide_count );
	MessageBox( NULL, wide, (LPCWSTR)L"Assertion Failure", MB_ICONWARNING );
	delete[] wide;
}

void DebugOutput( const char* s )
{
	OutputDebugStringA( s );
}

void DebugOutput( std::string s )
{
	OutputDebugStringA( s.c_str() );
}

void TracePrintf( const char* file, int line, const char* fmt, ... )
{
	constexpr size_t kMaxBufferSize = 512u;
	char buffer[kMaxBufferSize];

	va_list args;
	va_start( args, fmt );
	// format should be double click-able in VS 
	int len = sprintf_s( buffer, kMaxBufferSize, "%s(%d): ", file, line );
	vsprintf_s( buffer + len, kMaxBufferSize - len, fmt, args );
	DebugOutput( buffer );
	va_end( args );
}
