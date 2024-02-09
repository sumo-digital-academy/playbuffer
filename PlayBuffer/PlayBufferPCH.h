//********************************************************************************************************************************
//				           ___                         ___                   .-.       .-.                          
//				          (   )                       (   )                 /    \    /    \                        
//				   .-..    | |    .---.   ___  ___     | |.-.    ___  ___   | .`. ;   | .`. ;    .--.    ___ .-.    
//				  /    \   | |   / .-, \ (   )(   )    | /   \  (   )(   )  | |(___)  | |(___)  /    \  (   )   \   
//				 ' .-,  ;  | |  (__) ; |  | |  | |     |  .-. |  | |  | |   | |_      | |_     |  .-. ;  | ' .-. ;  
//				 | |  . |  | |    .'`  |  | |  | |     | |  | |  | |  | |  (   __)   (   __)   |  | | |  |  / (___) 
//				 | |  | |  | |   / .'| |  | '  | |     | |  | |  | |  | |   | |       | |      |  |/  |  | |        
//				 | |  | |  | |  | /  | |  '  `-' |     | |  | |  | |  | |   | |       | |      |  ' _.'  | |        
//				 | |  ' |  | |  ; |  ; |   `.__. |     | '  | |  | |  ; '   | |       | |      |  .'.-.  | |        
//				 | `-'  '  | |  ' `-'  |   ___ | |     ' `-' ;   ' `-'  /   | |       | |      '  `-' /  | |        
//				 | \__.'  (___) `.__.'_.  (   )' |      `.__.     '.__.'   (___)     (___)      `.__.'  (___)       
//				 | |                       ; `-' '                                                                  
//				(___)                       .__.'                                                                  
//
//********************************************************************************************************************************
//                       PlayBuffer: An instructional scaffold for learning C++ through 2D game development
// 
//											Copyright 2020 Sumo Digital Limited
//	   C++ code is made available under the Creative Commons Attribution - No Derivatives 4.0 International Public License 
//									https://creativecommons.org/licenses/by-nd/4.0/legalcode
//********************************************************************************************************************************
#ifndef PLAYPCH_H
#define PLAYPCH_H

#define PLAY_VERSION	"Unknown"

#include <cstdint>
#include <cstdlib>
#include <cmath> 
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <future>
#include <mutex> 

// Exclude rarely-used content from the Windows headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

// Stop windows macros defining their own min and max macros
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Windows Header Files
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <xaudio2.h>


#ifdef _DEBUG
#include <DbgHelp.h>
#endif

// Includes the GDI plus headers.
// These are only needed by internal parts of the library.

#include "dwmapi.h"
#include <Shlobj.h>
#pragma warning(push)
#pragma warning(disable:4458) // Suppress warning for declaration of 'xyz' hides class member

 // Redirect the GDI to use the standard library for min and max
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <GdiPlus.h>
#pragma warning(pop)

// Macros for Assertion and Tracing
void TracePrintf(const char* file, int line, const char* fmt, ...);
void AssertFailMessage(const char* message, const char* file, long line );
void DebugOutput( const char* s );
void DebugOutput( std::string s );

#ifdef _DEBUG
#define PLAY_TRACE(fmt, ...) TracePrintf(__FILE__, __LINE__, fmt, __VA_ARGS__);
#define PLAY_ASSERT(x) if(!(x)){ PLAY_TRACE(" *** ASSERT FAIL *** !("#x")\n\n"); AssertFailMessage(#x, __FILE__, __LINE__), __debugbreak(); }
#define PLAY_ASSERT_MSG(x,y) if(!(x)){ PLAY_TRACE(" *** ASSERT FAIL *** !("#x")\n\n"); AssertFailMessage(y, __FILE__, __LINE__), __debugbreak(); }
#else
#define PLAY_TRACE(fmt, ...)
#define PLAY_ASSERT(x) if(!(x)){ AssertFailMessage(#x, __FILE__, __LINE__);  }
#define PLAY_ASSERT_MSG(x,y) if(!(x)){ AssertFailMessage(y, __FILE__, __LINE__); }
#endif // _DEBUG

// Suppress warning for an unused argument or variable
#define PLAY_UNUSED(x) ((void)x)

#include "PlayMemory.h"
#include "PlayMaths.h"
#include "PlayPixel.h"
#include "PlayMouse.h"
#include "PlayWindow.h"
#include "PlayBlends.h"
#include "PlayRender.h"
#include "PlayGraphics.h"
#include "PlayAudio.h"
#include "PlayInput.h"
#include "PlayManager.h"
#include "PlayObject.h"

#endif // PLAYPCH_H