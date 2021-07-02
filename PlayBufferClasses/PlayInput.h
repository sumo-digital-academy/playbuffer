#ifndef PLAY_PLAYINPUT_H
#define PLAY_PLAYINPUT_H
//********************************************************************************************************************************
// File:		PlayInput.h
// Description:	Manages keyboard and mouse input 
// Platform:	Windows
// Notes:		Obtains mouse data from PlayWindow via MouseData structure
//********************************************************************************************************************************

// Manages keyboard and mouse input 
// > Singleton class accessed using PlayInput::Instance()
#pragma once
class PlayInput
{
public:

	enum MouseButton
	{
		BUTTON_LEFT	= 0,
		BUTTON_RIGHT
	};

	// Instance functions
	//********************************************************************************************************************************

	// Creates / Returns the PlayInput instance
	static PlayInput& Instance();
	// Destroys the PlayInput instance
	static void Destroy();

	// Mouse and keyboard functions
	//********************************************************************************************************************************

	// Returns the status of the supplied mouse button (0=left, 1=right)
	bool GetMouseDown( MouseButton button ) const;
	// Get the screen position of the mouse cursor
	Point2f GetMousePos() const { return m_mouseData.pos; }
	// Returns true if the key has been pressed since it was last released
	// > https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	bool KeyPressed( int vKey );
	// Returns true if the key is currently being held down
	// > https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	bool KeyDown( int vKey );

	MouseData* GetMouseData( void ) { return &m_mouseData; }

private:

	// Constructor / destructor
	//********************************************************************************************************************************
	
	// Private constructor 
	PlayInput();
	// Private destructor
	~PlayInput();
	// The assignment operator is removed to prevent copying of a singleton class
	PlayInput& operator=( const PlayInput& ) = delete;
	// The copy constructor is removed to prevent copying of a singleton class
	PlayInput( const PlayInput& ) = delete;


	MouseData m_mouseData;
	// Pointer to the singleton
	static PlayInput* s_pInstance;

};


#endif

