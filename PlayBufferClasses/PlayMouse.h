#ifndef PLAY_PLAYMOUSE_H
#define PLAY_PLAYMOUSE_H
//********************************************************************************************************************************
// File:		PlayMouse.h
// Platform:	Independent
// Description:	A mouse input data type 
//********************************************************************************************************************************

// A mouse structure to hold pointer co-ordinates and button states
struct MouseData
{
	Vector2f pos{ 0, 0 };
	bool left{ false };
	bool right{ false };
};

#endif
