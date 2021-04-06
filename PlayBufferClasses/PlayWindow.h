#ifndef PLAY_PLAYWINDOW_H
#define PLAY_PLAYWINDOW_H
//********************************************************************************************************************************
// File:		PlayWindow.h
// Description:	Platform specific code to provide a window to draw into
// Platform:	Windows
// Notes:		Uses a 32-bit ARGB display buffer
//********************************************************************************************************************************

// The target frame rate
constexpr int FRAMES_PER_SECOND = 60;

// Some defines to hide the complexity of arguments 
#define PLAY_IGNORE_COMMAND_LINE	int, char*[]

constexpr int PLAY_OK = 0;
constexpr int PLAY_ERROR = -1;

// Encapsulates the platform specific functionality of creating and managing a window 
// > Singleton class accessed using PlayWindow::Instance()
class PlayWindow
{
public:

	// Instance functions
	//********************************************************************************************************************************

	// Creates the PlayWindow instance and initialises the window
	static PlayWindow& Instance( PixelData* pDisplayBuffer, int nScale );
	// Returns the PlayWindow instance
	static PlayWindow& Instance();
	// Destroys the PlayWindow instance
	static void Destroy();

	// Windows functions
	//********************************************************************************************************************************

	// Call within WInMain to hand control of Windows functionality over to the PlayWindow class
	int HandleWindows( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow, LPCWSTR windowName );
	// Handles Windows messages for the PlayWindow  
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	// Copies the display buffer pixels to the window
	// > Returns the time taken for the present in seconds
	double Present();
	// Sets the pointer to write mouse input data to
	void RegisterMouse( MouseData* pMouseData ) { m_pMouseData = pMouseData; }

	// Getter functions
	//********************************************************************************************************************************

	// Gets the width of the display buffer in pixels
	int GetWidth() const { return m_pPlayBuffer->width; }
	// Gets the height of the display buffer in pixels 
	int GetHeight() const { return m_pPlayBuffer->height; }
	// Gets the scale of the display buffer in pixels
	int GetScale() const { return m_scale; }

	// Loading functions
	//********************************************************************************************************************************

	// Reads the width and height of a png image
	static int ReadPNGImage( std::string& fileAndPath, int& width, int& height );
	// Loads a png image and puts the image data into the destination image provided
	static int LoadPNGImage( std::string& fileAndPath, PixelData& destImage );

private:

	// Constructor / destructor
	//********************************************************************************************************************************

	// Creates a window of the required size and scale and stores a pointer to the display buffer source
	PlayWindow( PixelData* pDisplayBuffer, int nScale ); 
	// Frees up memory from all the different buffers
	~PlayWindow();
	// The assignment operator is removed to prevent copying of a singleton class
	PlayWindow& operator=( const PlayWindow& ) = delete;
	// The copy constructor is removed to prevent copying of a singleton class
	PlayWindow( const PlayWindow& ) = delete;

	// Miscellaneous internal functions
	//********************************************************************************************************************************

	// Display buffer dimensions
	int m_scale{ 0 };

	// Buffer pointers
	PixelData* m_pPlayBuffer{ nullptr };
	//Pointer to external mouse data
	MouseData* m_pMouseData{ nullptr };
	// Pointer to the instance.
	static PlayWindow* s_pInstance;
	// The handle to the Window 
	HWND m_hWindow{ nullptr };
	// A GDI+ token
	static unsigned long long s_pGDIToken;
};

#endif


