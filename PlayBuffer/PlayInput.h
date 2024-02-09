#ifndef PLAY_PLAYINPUT_H
#define PLAY_PLAYINPUT_H
//********************************************************************************************************************************
// File:		PlayInput.h
// Description:	Manages keyboard and mouse input 
// Platform:	Windows
// Notes:		Obtains mouse data from PlayWindow via MouseData structure
//********************************************************************************************************************************

namespace Play
{
	// values equivalent to Windows VK codes or ASCII alphanumeric
	enum KeyboardButton
	{
		KEY_BACKSPACE = 8, KEY_TAB,
		KEY_ENTER = 13,
		KEY_SHIFT = 16, KEY_CTRL, KEY_ALT,
		KEY_ESCAPE = 27,
		KEY_SPACE = 32, KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_END, KEY_HOME,
		KEY_LEFT = 37, KEY_UP, KEY_RIGHT, KEY_DOWN,
		KEY_INSERT = 45, KEY_DELETE,
		KEY_0 = 48, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
		KEY_A = 65, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
		KEY_NUMPAD_0 = 96, KEY_NUMPAD_1, KEY_NUMPAD_2, KEY_NUMPAD_3, KEY_NUMPAD_4, KEY_NUMPAD_5, KEY_NUMPAD_6, KEY_NUMPAD_7, KEY_NUMPAD_8, KEY_NUMPAD_9,
		KEY_PLUS = 187, KEY_NUMPAD_PLUS = 107,
		KEY_MINUS = 189, KEY_NUMPAD_MINUS = 109,
		KEY_GREATER_THAN = 190, KEY_LESS_THAN = 188,
		KEY_F1 = 112, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
		KEY_OPEN_SQB = 219,
		KEY_CLOSE_SQB = 221
	};

	namespace Input
	{

		enum class MouseButton
		{
			BUTTON_LEFT = 0,
			BUTTON_RIGHT
		};

		// Creates the Input Manager
		MouseData* CreateManager();
		// Destroys the Input Manager
		bool DestroyManager();
		// Returns the status of the supplied mouse button (0=left, 1=right)
		bool GetMouseDown( MouseButton button );
		// Get the screen position of the mouse cursor
		Point2f GetMousePos();
		// Returns true if the key has been pressed since it was last released
		// If you omit the frame number then only the first call in the same frame will ever return true
		bool KeyPress( Play::KeyboardButton key, int frame = -1 );
		// Returns true if the key is currently being held down
		bool KeyHeld( KeyboardButton key );
	};
}
#endif // PLAY_PLAYINPUT_H

