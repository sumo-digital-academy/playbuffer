//********************************************************************************************************************************
// File:		PlayInput.cpp
// Description:	Manages keyboard and mouse input 
// Platform:	Windows
// Notes:		Obtains mouse data from PlayWindow via MouseData structure
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

PlayInput* PlayInput::s_pInstance = nullptr;

//********************************************************************************************************************************
// Constructor and destructor (private)
//********************************************************************************************************************************
PlayInput::PlayInput( void )
{
	PLAY_ASSERT_MSG( !s_pInstance, "PlayInput is a singleton class: multiple instances not allowed!" );
	s_pInstance = this;
}

PlayInput::~PlayInput( void )
{
	s_pInstance = nullptr;
}

//********************************************************************************************************************************
// Instance access functions
//********************************************************************************************************************************

PlayInput& PlayInput::Instance()
{
	if( !s_pInstance )
		s_pInstance = new PlayInput();

	return *s_pInstance;
}

void PlayInput::Destroy()
{
	if( s_pInstance )
		delete s_pInstance;
}

//********************************************************************************************************************************
// Mouse functions
//********************************************************************************************************************************

bool PlayInput::GetMouseDown( MouseButton button ) const
{
	PLAY_ASSERT_MSG( button == BUTTON_LEFT || button == BUTTON_RIGHT, "Invalid mouse button selected." );

	if( button == BUTTON_LEFT )
		return m_mouseData.left;
	else
		return m_mouseData.right;
};

bool PlayInput::KeyPressed( int vKey, int frame )
{
	static std::map< int, int > keyMap;

	int& previous_frame = keyMap[vKey];

	// Returns true if key wasn't pressed the last time we checked or if this is the same frame as the last check
	if( KeyDown( vKey ) && ( previous_frame == 0 || ( previous_frame == frame && frame != -1 ) ) )
	{
		previous_frame = frame;
		return true;
	}

	if( !KeyDown( vKey ) )
		previous_frame = 0;

	return false;
}

bool PlayInput::KeyDown( int vKey )
{
	return GetAsyncKeyState( vKey ) & 0x8000; // Don't want multiple calls to KeyState
}