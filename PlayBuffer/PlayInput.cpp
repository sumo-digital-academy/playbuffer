//********************************************************************************************************************************
// File:		PlayInput.cpp
// Description:	Manages keyboard and mouse input 
// Platform:	Windows
// Notes:		Obtains mouse data from PlayWindow via MouseData structure
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

#define ASSERT_INPUT PLAY_ASSERT_MSG( Play::Input::m_bCreated, "Input Manager not initialised. Call Input::CreateManager() before using the Play::Input library functions.")

namespace Play::Input
{
	// Flag to record whether the manager has been created
	bool m_bCreated = false;
	// The state of the mouse
	MouseData m_mouseData;

	//********************************************************************************************************************************
	// Create and Destroy functions
	//********************************************************************************************************************************
	MouseData* CreateManager(void)
	{
		PLAY_ASSERT_MSG(!m_bCreated, "Input manager has already been created!");
		m_bCreated = true;
		return &m_mouseData;
	}

	bool DestroyManager(void) 
	{
		ASSERT_INPUT;
		m_bCreated = false;
		return true;
	}

	//********************************************************************************************************************************
	// Mouse functions
	//********************************************************************************************************************************
	bool GetMouseDown(MouseButton button)
	{
		ASSERT_INPUT;
		PLAY_ASSERT_MSG(button == MouseButton::BUTTON_LEFT || button == MouseButton::BUTTON_RIGHT, "Invalid mouse button selected.");

		if (button == MouseButton::BUTTON_LEFT)
			return m_mouseData.left;
		else
			return m_mouseData.right;
	};

	bool KeyPress( KeyboardButton key, int frame)
	{
		ASSERT_INPUT;
		static std::map< int, int > keyMap;

		int& previous_frame = keyMap[key];

		// Returns true if key wasn't pressed the last time we checked or if this is the same frame as the last check
		if (KeyHeld(key) && (previous_frame == 0 || (previous_frame == frame && frame != -1)))
		{
			previous_frame = frame;
			return true;
		}

		if (!KeyHeld(key))
			previous_frame = 0;

		return false;
	}

	bool KeyHeld( KeyboardButton key)
	{
		ASSERT_INPUT;
		return GetAsyncKeyState(key) & 0x8000; // Don't want multiple calls to KeyState
	}

	Point2f GetMousePos() 
	{ 
		ASSERT_INPUT;
		return m_mouseData.pos; 
	}
}