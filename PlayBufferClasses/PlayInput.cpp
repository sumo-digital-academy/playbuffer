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

bool PlayInput::KeyPressed( int vKey )
{
	static std::map< int, bool > keyMap;

	bool& held = keyMap[vKey];

	if( KeyDown( vKey ) && !held )
	{
		held = true;
		return true;
	}

	if( !KeyDown( vKey ) )
		held = false;

	return false;
}

bool PlayInput::KeyDown( int vKey )
{
	return GetAsyncKeyState( vKey ) & 0x8000; // Don't want multiple calls to KeyState
}