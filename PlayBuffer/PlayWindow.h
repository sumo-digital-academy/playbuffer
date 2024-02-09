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

namespace Play::Window
{
	// Create/Destroy functions
	//********************************************************************************************************************************

	// Initialises the Window Manager
	bool CreateManager( PixelData* pDisplayBuffer, int nScale );
	// Destroys the Window Manager
	bool DestroyManager();

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
	void RegisterMouse(Play::MouseData* pMouseData);

	// Getter functions
	//********************************************************************************************************************************

	// Gets the width of the display buffer in pixels
	int GetWidth();
	// Gets the height of the display buffer in pixels 
	int GetHeight();
	// Gets the scale of the display buffer in pixels
	int GetScale();
};

// Loading functions
//********************************************************************************************************************************

// Reads the width and height of a png image
int ReadPNGImage( std::string& fileAndPath, int& width, int& height );
// Loads a png image and puts the image data into the destination image provided
int LoadPNGImage( std::string& fileAndPath, Play::PixelData& destImage );
// Saves a png image using the image data into the source image provided
int SavePNGImage( std::string& fileAndPath, const Play::PixelData& sourceImage );

#endif // PLAY_PLAYWINDOW_H