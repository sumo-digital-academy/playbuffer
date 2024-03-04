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

#define PLAY_VERSION	"2.0.24.03.04"

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

#ifndef PLAY_PLAYMEMORY_H
#define PLAY_PLAYMEMORY_H
//********************************************************************************************************************************
// File:		PlayMemory.h
// Platform:	Independent
// Description:	Declaration for a simple memory tracker to prevent leaks
//********************************************************************************************************************************
#ifdef _DEBUG
	// Prints out all the currently allocated memory to the debug output
namespace Play
{
	void PrintAllocations(const char* tagText);
}; // namespace Play
	void* operator new  (std::size_t sizeBytes);
	void* operator new[](std::size_t sizeBytes);

	void operator delete  (void* ptr) noexcept;
	void operator delete[](void* ptr) noexcept;
#else
namespace Play
{ 
	inline void PrintAllocations(const char*) {};
} // namespace Play
#endif

#endif // PLAY_PLAYMEMORY_H
#ifndef PLAY_PLAYMATHS_H
#define PLAY_PLAYMATHS_H
//********************************************************************************************************************************
// File:		PlayMaths.h
// Description:	A very simple set of 2D maths structures and operations
// Platform:	Independent
//********************************************************************************************************************************
namespace Play
{
	// Global constants such as PI
	constexpr float PLAY_PI = 3.14159265358979323846f;   // pi

	// Converts radians to degrees
	constexpr float RadToDeg( float radians ) { return (radians / PLAY_PI) * 180.0f; }

	// Converts radians to degrees
	constexpr float DegToRad( float degrees ) { return (degrees / 180.0f) * PLAY_PI; }

	struct Vector3f;

	// The main 2D structure used in the library
	#pragma warning (push)
	#pragma warning (disable : 4201) // nonstandard extension used: nameless struct/union
	struct Vector2f
	{
		Vector2f() {}
		// We're encouraging implicit type conversions between float and int with the same number of parameters
		// This is really just to help new coders who already suffering from cognitive overload! :-)
		Vector2f(float x, float y) : x(x), y(y) {}
		Vector2f(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
		Vector2f(float x, int y) : x(x), y(static_cast<float>(y)) {}
		Vector2f(int x, float y) : x(static_cast<float>(x)), y(y) {}

		// Different ways of accessing member data
		union
		{
			float v[2];
			struct { float x; float y; };
			struct { float width; float height; };
		};

		// Calculates and returns the length of the vector
		float Length() const { return sqrt( Dot( *this ) ); }
		// Calculates and returns the length of the vector squared (faster)
		float LengthSqr() const { return Dot( *this ); }
		// Scales this vector to a unit length (with the same direction)
		void Normalize();
		// Returns a vector at right angles to this one
		Vector2f Perpendicular() const;
		// Returns true if the vector is euivalent to this one within tolerances (read about floating point accuracy!)
		bool AboutEqualTo(const Vector2f& rhs, const float tolerance) const;
		// Returns the dot product between the  vector provided and this one
		float Dot(const Vector2f& rhs) const;
		// Assignment operator with Vector3f
		Vector2f& operator = (const Vector3f& rhs);
		// Copy constructor with Vector3f
		Vector2f(const Vector3f& rhs);
	};

	// A point is conceptually different to a vector, but maps to Vector2f/Vector3f for ease of use
	//! @brief A Point on screen, with an x/y coordinate.
	using Point2f = Vector2f;
	//! @brief A Point on screen, with an x/y coordinate.
	using Point2D = Vector2f;
	using Vector2D = Vector2f;

	// A 2D vector with a w component for use with matrices
	struct Vector3f
	{
		Vector3f() {}
		Vector3f(float x, float y, float w) : x(x), y(y), w(w) {}

		// Different ways of accessing member data
		union
		{
			float v[3];
			struct { float x; float y; float w; };
			struct { float width; float height; float w; };
		};

		// Returns the 2D part of the 3D vector
		Vector2f As2D() const { return Vector2f(x, y); }
		// Calculates and returns the length of the vector
		float Length() const { return sqrt( Dot( *this ) ); }
		// Calculates and returns the length of the vector squared (faster)
		float LengthSqr() const { return Dot( *this ); }
		// Scales this vector to a unit length (with the same direction)
		void Normalize();
		// Returns a vector at right angles to this one
		Vector3f Perpendicular() const;
		// Returns true if the vector is euivalent to this one within tolerances (read about floating point accuracy!)
		bool AboutEqualTo(const Vector3f& rhs, const float tolerance) const;
		// Returns the dot product between the  vector provided and this one
		float Dot(const Vector3f& rhs) const;
		// Assignment operator with Vector2f
		Vector3f& operator = (const Vector2f& rhs);
		// Copy constructor with Vector3f
		Vector3f(const Vector2f& rhs);
	};

#pragma warning(pop)

	using Point3f = Vector3f;
	using Point3D = Vector3f;
	using Vector3D = Vector3f;

	// Vector assignment and copy operations
	//**************************************************************************************************

	inline Vector2f& Vector2f::operator = (const Vector3f& rhs)
	{
		v[0] = rhs.v[0];
		v[1] = rhs.v[1];
		return *this;
	}

	inline Vector2f::Vector2f(const Vector3f& rhs)
	{
		v[0] = rhs.v[0];
		v[1] = rhs.v[1];
	}

	inline Vector3f& Vector3f::operator = (const Vector2f& rhs)
	{
		v[0] = rhs.v[0];
		v[1] = rhs.v[1];
		v[2] = 0.0f;
		return *this;
	}

	inline Vector3f::Vector3f(const Vector2f& rhs)
	{
		v[0] = rhs.v[0];
		v[1] = rhs.v[1];
		v[2] = 0.0f;
	}

	// Vector component operations
	//**************************************************************************************************

	// Vector component addition
	inline Vector2f operator + (const Vector2f& lhs, const Vector2f& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			ret.v[i] = lhs.v[i] + rhs.v[i];

		return ret;
	}

	inline Vector3f operator + (const Vector3f& lhs, const Vector3f& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			ret.v[i] = lhs.v[i] + rhs.v[i];

		return ret;
	}

	// Vector component assignment addition
	inline Vector2f& operator += (Vector2f& lhs, const Vector2f& rhs)
	{
		for (int i = 0; i < 2; ++i)
			lhs.v[i] += rhs.v[i];

		return lhs;
	}

	inline Vector3f& operator += (Vector3f& lhs, const Vector3f& rhs)
	{
		for (int i = 0; i < 3; ++i)
			lhs.v[i] += rhs.v[i];

		return lhs;
	}

	// Vector component subtraction
	inline Vector2f operator - (const Vector2f& lhs, const Vector2f& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			ret.v[i] = lhs.v[i] - rhs.v[i];

		return ret;
	}

	inline Vector3f operator - (const Vector3f& lhs, const Vector3f& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			ret.v[i] = lhs.v[i] - rhs.v[i];

		return ret;
	}

	// Vector component assignment subtraction
	inline Vector2f& operator -= (Vector2f& lhs, const Vector2f& rhs)
	{
		for (int i = 0; i < 2; ++i)
			lhs.v[i] -= rhs.v[i];

		return lhs;
	}

	inline Vector3f& operator -= (Vector3f& lhs, const Vector3f& rhs)
	{
		for (int i = 0; i < 3; ++i)
			lhs.v[i] -= rhs.v[i];

		return lhs;
	}

	// Vector component unary negation
	inline Vector2f operator - (const Vector2f& op)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			ret.v[i] = -op.v[i];

		return ret;
	}

	inline Vector3f operator - (const Vector3f& op)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			ret.v[i] = -op.v[i];

		return ret;
	}

	// Vector component multiplication
	inline Vector2f operator * (const Vector2f& lhs, const Vector2f& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			ret.v[i] = lhs.v[i] * rhs.v[i];

		return ret;
	}

	inline Vector3f operator * (const Vector3f& lhs, const Vector3f& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			ret.v[i] = lhs.v[i] * rhs.v[i];

		return ret;
	}

	// Vector component assignment multiplication
	inline Vector2f operator *= (Vector2f& lhs, const Vector2f& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			lhs.v[i] *= rhs.v[i];

		return lhs;
	}

	inline Vector3f operator *= (Vector3f& lhs, const Vector3f& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			lhs.v[i] *= rhs.v[i];

		return lhs;
	}

	// Vector component division
	inline Vector2f operator / (const Vector2f& lhs, const Vector2f& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			ret.v[i] = lhs.v[i] / rhs.v[i];

		return ret;
	}

	inline Vector3f operator / (const Vector3f& lhs, const Vector3f& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			ret.v[i] = lhs.v[i] / rhs.v[i];

		return ret;
	}

	// Vector component assignment division
	inline Vector2f operator /= (Vector2f& lhs, const Vector2f& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			lhs.v[i] /= rhs.v[i];

		return lhs;
	}

	inline Vector3f operator /= (Vector3f& lhs, const Vector3f& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			lhs.v[i] /= rhs.v[i];

		return lhs;
	}

	// Vector scalar operations
	//**************************************************************************************************

	// Vector scalar multiplication
	inline Vector2f operator * (const Vector2f& lhs, const float rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			ret.v[i] = lhs.v[i] * rhs;

		return ret;
	}

	inline Vector3f operator * (const Vector3f& lhs, const float rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			ret.v[i] = lhs.v[i] * rhs;

		return ret;
	}

	// Vector scalar multiplication (reverse operands)
	inline Vector2f operator * (const float lhs, const Vector2f& rhs) { return rhs * lhs; }
	inline Vector3f operator * (const float lhs, const Vector3f& rhs) { return rhs * lhs; }

	// Vector scalar assignment multiplication
	inline Vector2f operator *= (Vector2f& lhs, const float& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			lhs.v[i] *= rhs;

		return lhs;
	}

	inline Vector3f operator *= (Vector3f& lhs, const float& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			lhs.v[i] *= rhs;

		return lhs;
	}

	// Vector scalar division
	inline Vector2f operator / (const Vector2f& lhs, const float rhs) { return lhs * (float(1) / rhs); }
	inline Vector3f operator / (const Vector3f& lhs, const float rhs) { return lhs * (float(1) / rhs); }

	// Vector scalar division (reverse operands)
	inline Vector2f operator / (const float lhs, const Vector2f& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			ret.v[i] = lhs / rhs.v[i];

		return ret;
	}

	inline Vector3f operator / (const float lhs, const Vector3f& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			ret.v[i] = lhs / rhs.v[i];

		return ret;
	}

	// Vector scalar assignment multiplication
	inline Vector2f operator /= (Vector2f& lhs, const float& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
			lhs.v[i] /= rhs;

		return lhs;
	}

	inline Vector3f operator /= (Vector3f& lhs, const float& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
			lhs.v[i] /= rhs;

		return lhs;
	}

	// Vector component comparisons
	//**************************************************************************************************

	// Vector components exactly equal
	inline bool operator == (const Vector2f& lhs, const Vector2f& rhs)
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
		{
			if (lhs.v[i] != rhs.v[i])
				return false;
		}
		return true;
	}

	inline bool operator == (const Vector3f& lhs, const Vector3f& rhs)
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
		{
			if (lhs.v[i] != rhs.v[i])
				return false;
		}
		return true;
	}

	// Vector components not equal
	inline bool operator != (const Vector2f& lhs, const Vector2f& rhs) { return !(lhs == rhs); }
	inline bool operator != (const Vector3f& lhs, const Vector3f& rhs) { return !(lhs == rhs); }

	// Vector components equal to within specified tolerance.
	inline bool Vector2f::AboutEqualTo(const Vector2f& rhs, const float tolerance) const
	{
		Vector2f ret;
		for (int i = 0; i < 2; ++i)
		{
			if (std::abs(v[i] - rhs.v[i]) > tolerance)
				return false;
		}
		return true;
	}

	inline bool Vector3f::AboutEqualTo(const Vector3f& rhs, const float tolerance) const
	{
		Vector3f ret;
		for (int i = 0; i < 3; ++i)
		{
			if (std::abs(v[i] - rhs.v[i]) > tolerance)
				return false;
		}
		return true;
	}

	// Common maths functions
	//**************************************************************************************************

	// Dot product
	inline float  Vector2f::Dot(const Vector2f& rhs) const
	{
		float ret = 0.f;
		for (int i = 0; i < 2; ++i)
			ret += v[i] * rhs.v[i];

		return ret;
	}


	inline float Vector3f::Dot(const Vector3f& rhs) const
	{
		float ret = 0.f;
		for (int i = 0; i < 3; ++i)
			ret += v[i] * rhs.v[i];

		return ret;
	}

	inline float dot(const Vector2f& lhs, const Vector2f& rhs) { return lhs.Dot(rhs); }
	inline float dot(const Vector3f& lhs, const Vector3f& rhs) { return lhs.Dot(rhs); }

	// Orthogonal vector
	inline Vector2f Vector2f::Perpendicular() const
	{
		Vector2f ret;
		ret.v[0] = -v[1];
		ret.v[1] = v[0];
		return ret;
	}

	// Orthogonal vector
	inline Vector2f perpendicular(const Vector3f& rhs)
	{
		Vector2f ret;
		ret.v[0] = -rhs.v[1];
		ret.v[1] = rhs.v[0];
		return ret;
	}

	inline float lengthSqr(const Vector2f& v) { return dot(v, v); }
	inline float lengthSqr(const Vector3f& v) {	return dot(v, v); }
	inline float length(const Vector2f& v) { return sqrt(dot(v, v)); }
	inline float length( const Vector3f& v ) { return sqrt( dot( v, v ) ); }
	inline Vector2f normalize(const Vector2f& v) { return v / v.Length(); }
	inline Vector3f normalize(const Vector3f& v) { return v / v.Length(); }
	inline void Vector2f::Normalize() { *this /= Length(); }
	inline void Vector3f::Normalize() {	*this /= Length(); }

	// A 3x3 structure for representing 2D matrices
	struct Matrix2D
	{
		Matrix2D() {}
		explicit Matrix2D(const Vector3f& row0, const Vector3f& row1, const Vector3f& row2)
			: row{ row0, row1, row2 }
		{
		}
		union
		{
			float m[3][3];
			Vector3f row[3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };
			struct { float _00, _01, _02, _10, _11, _12, _20, _21, _22; };
		};

		// Transforms a vector by this matrix
		Vector3f Transform(const Vector3f& v) const;
		// Transforms a vector by this matrix
		Vector2f Transform(const Vector2f& v) const;
		// Transposes the matrix (swaps the columns and rows)
		void Transpose();
		// Compares this matrix to another within a tolerance (read about floating point inaccuracies!)
		bool AboutEqualTo(const Matrix2D& rhs, const float tolerance) const;
		// Inverts this matrix (makes it perform the opposite operation)
		void Inverse();
		// Reverses the direction of rotation
		Matrix2D MatrixReverse(const Matrix2D& m);
	};

	// Matrix addition
	inline Matrix2D operator + (const Matrix2D& A, const Matrix2D& B)
	{
		Matrix2D result;
		result.row[ 0 ] = A.row[ 0 ] + B.row[ 0 ];
		result.row[ 1 ] = A.row[ 1 ] + B.row[ 1 ];
		result.row[ 2 ] = A.row[ 2 ] + B.row[ 2 ];
		return result;
	}

	// Matrix subtraction
	inline Matrix2D operator - (const Matrix2D& A, const Matrix2D& B)
	{
		Matrix2D result;
		result.row[ 0 ] = A.row[ 0 ] - B.row[ 0 ];
		result.row[ 1 ] = A.row[ 1 ] - B.row[ 1 ];
		result.row[ 2 ] = A.row[ 2 ] - B.row[ 2 ];
		return result;
	}

	// Matrix multiplication
	inline Matrix2D operator*( const Matrix2D& A, const Matrix2D& B ) 
	{
		Matrix2D result;

		// Written out in long form for ease of understanding
		result._00 = (A._00 * B._00) + (A._01 * B._10) + (A._02 * B._20);
		result._01 = (A._00 * B._01) + (A._01 * B._11) + (A._02 * B._21);
		result._02 = (A._00 * B._02) + (A._01 * B._12) + (A._02 * B._22);

		result._10 = (A._10 * B._00) + (A._11 * B._10) + (A._12 * B._20);
		result._11 = (A._10 * B._01) + (A._11 * B._11) + (A._12 * B._21);
		result._12 = (A._10 * B._02) + (A._11 * B._12) + (A._12 * B._22);

		result._20 = (A._20 * B._00) + (A._21 * B._10) + (A._22 * B._20);
		result._21 = (A._20 * B._01) + (A._21 * B._11) + (A._22 * B._21);
		result._22 = (A._20 * B._02) + (A._21 * B._12) + (A._22 * B._22);

		return result;
	}

	// Multiply a vector by a matrix
	inline Vector2f Matrix2D::Transform(const Vector2f& v) const
	{
		Vector2f result;
		result.x = (v.x * _00) + (v.y * _10) + _20; 
		result.y = (v.x * _01) + (v.y * _11) + _21; 
		return result;
	}

	// Multiply a vector by a matrix
	inline Vector3f Matrix2D::Transform( const Vector3f & v ) const
	{
		Vector3f result;
		result.x = (v.x * _00) + (v.y * _10) + (v.w * _20);
		result.y = (v.x * _01) + (v.y * _11) + (v.w * _21);
		result.w = (v.x * _02) + (v.y * _12) + (v.w * _22);
		return result;
	}

	// Transpose the contents of a matrix
	inline void Matrix2D::Transpose()
	{
		*this = Matrix2D(
			Vector3f( _00, _10, _20 ),
			Vector3f( _01, _11, _21 ),
			Vector3f( _02, _12, _22 )
		);
	}

	// Create an identity matrix
	inline Matrix2D MatrixIdentity()
	{
		return Matrix2D(
			Vector3f( 1, 0, 0 ),
			Vector3f( 0, 1, 0 ),
			Vector3f( 0, 0, 1 )
		);
	}

	// Create a rotation matrix
	inline Matrix2D MatrixRotation(const float theta)
	{
		float c = cos(theta);
		float s = sin(theta);

		return Matrix2D(
			Vector3f(c, s, 0),
			Vector3f(-s, c, 0),
			Vector3f(0, 0, 1)
		);
	}

	// Create a scaling matrix
	inline Matrix2D MatrixScale(const float x, const float y)
	{
		return Matrix2D(
			Vector3f(x, 0, 0),
			Vector3f(0, y, 0),
			Vector3f(0, 0, 1)
		);
	}

	// Create a translation matrix
	inline Matrix2D MatrixTranslation(const float x, const float y)
	{
		return Matrix2D(
			Vector3f(1, 0, 0),
			Vector3f(0, 1, 0),
			Vector3f(x, y, 1)
		);
	}

	// Reverse the rotation direction of the matrix
	inline Matrix2D MatrixReverse(const Matrix2D& m)
	{
		return Matrix2D(
			Vector3f( m._00, m._10, m._02 ),
			Vector3f( m._01, m._11, m._12 ),
			Vector3f( m._20, m._21, m._22 )
		);
	}

	// Compare two matrices within a tolerance value
	inline bool Matrix2D::AboutEqualTo(const Matrix2D& rhs, const float tolerance) const
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (std::abs(m[i][j] - rhs.m[i][j]) > tolerance)
					return false;
			}
		}
		return true;
	}

	// Calculate the determinant of a 2x2 matrix
	inline float Determinant(const Matrix2D& m)
	{
		return m.m[0][0] * m.m[1][1] * m.m[2][2]
			+ m.m[1][0] * m.m[2][1] * m.m[0][2]
		 	+ m.m[2][0] * m.m[0][1] * m.m[1][2]
		 	- m.m[0][0] * m.m[2][1] * m.m[1][2]
			- m.m[1][0] * m.m[0][1] * m.m[2][2]
			- m.m[2][0] * m.m[1][1] * m.m[0][2];
	}

	inline float det2x2( float a, float b, float c, float d ) { return a * d - b * c; }

	// Calculate the inverse of a 2D matrix
	inline void Matrix2D::Inverse()
	{
		float d = Determinant(*this);
		PLAY_ASSERT_MSG(d != 0.f, "Zero determinant");

		float f = float(1) / d;

		float c00 = det2x2(m[1][1], m[2][1], m[1][2], m[2][2]) * f;
		float c10 = det2x2(m[0][1], m[2][1], m[0][2], m[2][2]) * -f;
		float c20 = det2x2(m[0][1], m[1][1], m[0][2], m[1][2]) * f;

		float c01 = det2x2(m[1][0], m[2][0], m[1][2], m[2][2]) * -f;
		float c11 = det2x2(m[0][0], m[2][0], m[0][2], m[2][2]) * f;
		float c21 = det2x2(m[0][0], m[1][0], m[0][2], m[1][2]) * -f;

		float c02 = det2x2(m[1][0], m[2][0], m[1][1], m[2][1]) * f;
		float c12 = det2x2(m[0][0], m[2][0], m[0][1], m[2][1]) * -f;
		float c22 = det2x2(m[0][0], m[1][0], m[0][1], m[1][1]) * f;

		row[0] = Vector3f(c00, c10, c20);
		row[1] = Vector3f(c01, c11, c21);
		row[2] = Vector3f(c02, c12, c22);
	}
}
#endif // PLAY_PLAYMATHS_H
#ifndef PLAY_PLAYPIXEL_H
#define PLAY_PLAYPIXEL_H
//********************************************************************************************************************************
// File:		PlayPixel.h
// Platform:	Independent
// Description:	Pixel types for holding image data
//********************************************************************************************************************************
namespace Play
{
	// A pixel structure to represent an ARBG format pixel
	struct Pixel
	{
		Pixel() {}
		Pixel(uint32_t bits) : bits(bits) {}
		Pixel(float r, float g, float b) :
			a(0xFF), r(static_cast<uint8_t>(r)), g(static_cast<uint8_t>(g)), b(static_cast<uint8_t>(b))
		{
		}
		Pixel(int r, int g, int b) :
			a(0xFF), r(static_cast<uint8_t>(r)), g(static_cast<uint8_t>(g)), b(static_cast<uint8_t>(b))
		{
		}
		Pixel(int a, int r, int g, int b) :
			a(static_cast<uint8_t>(a)), r(static_cast<uint8_t>(r)), g(static_cast<uint8_t>(g)), b(static_cast<uint8_t>(b))
		{
		}

		union
		{
			uint32_t bits{ 0xFF000000 }; // Alpha set to opaque by default
			struct { uint8_t b, g, r, a; }; // This order corresponds to ( a<<24 | r<<16 | g<<8 | b )
		};
	};

	const Pixel PIX_BLACK{ 0x00, 0x00, 0x00 };
	const Pixel PIX_WHITE{ 0xFF, 0xFF, 0xFF };
	const Pixel PIX_RED{ 0xFF, 0x00, 0x00 };
	const Pixel PIX_GREEN{ 0x00, 0x8F, 0x00 };
	const Pixel PIX_BLUE{ 0x00, 0x00, 0xFF };
	const Pixel PIX_MAGENTA{ 0xFF, 0x00, 0xFF };
	const Pixel PIX_CYAN{ 0x00, 0xFF, 0xFF };
	const Pixel PIX_YELLOW{ 0xFF, 0xFF, 0x00 };
	const Pixel PIX_ORANGE{ 0xFF, 0x8F, 0x00 };
	const Pixel PIX_GREY{ 0x80, 0x80, 0x80 };
	const Pixel PIX_TRANS{ 0x00, 0x00, 0x00, 0x00 };

	struct PixelData
	{
		int width{ 0 };
		int height{ 0 };
		Pixel* pPixels{ nullptr };
		bool preMultiplied = false;
	};

	struct BlendColour
	{
		float alpha{ 1.0f };
		float red{ 1.0f };
		float green{ 1.0f };
		float blue{ 1.0f };
	};
}
#endif // PLAY_PLAYPIXEL_H
#ifndef PLAY_PLAYMOUSE_H
#define PLAY_PLAYMOUSE_H
//********************************************************************************************************************************
// File:		PlayMouse.h
// Platform:	Independent
// Description:	A mouse input data type 
// Notes:		Shared between Play::Input and Play::Window
//********************************************************************************************************************************
namespace Play
{
	// A mouse structure to hold pointer co-ordinates and button states
	struct MouseData
	{
		Vector2f pos{ 0, 0 };
		bool left{ false };
		bool right{ false };
	};
}
#endif // PLAY_PLAYMOUSE_H
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
#ifndef PLAY_PLAYBLENDS_H
#define PLAY_PLAYBLENDS_H
//********************************************************************************************************************************
// File:		PlayBlends.h
// Description:	A set of software pixel blending operations used by the templates in PlayRender.h
// Platform:	Independent
//********************************************************************************************************************************
namespace Play::Render
{
	// The pre-multiplied alpha buffers store the number of subsequent pixels which are also transparent 
	// so they can be skipped. This works for any blend mode which uses the pre-multiplied alpha buffer. 
	inline void Skip(uint32_t*& srcPixels, uint32_t*& destPixels, const uint32_t* destRowEnd)
	{
		// If this is a fully transparent pixel then the low bits store how many there are in a row
		// This means we can skip straight to the next pixel which isn't fully transparent
		uint32_t skip = static_cast<uint32_t>(destRowEnd - destPixels) - 1;
		uint32_t src = *srcPixels & 0x00FFFFFF;
		if (skip > src) skip = src;
		srcPixels += skip + 1;
		++destPixels += skip;
	}

	class AlphaBlendPolicy
	{
	public:
		// Standard alpha blending using a pre-multiplied srcAlpha buffer: (src * srcAlpha)+(dest * (1-srcAlpha)
		static inline void BlendFastSkip(uint32_t*& srcPixels, uint32_t*& destPixels, const uint32_t* destRowEnd)
		{
			// Optimal alpha blend approach performed on all channels simultaneously! 
			// Fully transparent pixels can be skipped in the optimal way using the Skip function above  
			if (BlendFast(srcPixels, destPixels))
				srcPixels++, destPixels++;
			else
				Skip(srcPixels, destPixels, destRowEnd);
		}

		// Standard alpha blending, but with an additional global alpha multiply
		static inline void BlendSkip( uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply, const uint32_t* destRowEnd )
		{
			// A slower blend calculation is required for semi-transparent pixels with a global multiply
			// Fully transparent pixels can be skipped in the optimal way using the Skip function above   
			if( Blend( srcPixels, destPixels, globalMultiply ) )
				srcPixels++, destPixels++;
			else
				Skip( srcPixels, destPixels, destRowEnd );
		}

		// *******************************************************************************************************************************************************
		// A basic approach which separates the channels and performs a 'typical' alpha blending operation: (src * srcAlpha)+(dest * (1-srcAlpha))
		// Has the advantage that a global alpha multiplication can be easily added over the top, so we use this method when a global multiply is required
		// Notes: Requires a source buffer which has the source alpha pre-multiplied
		// *******************************************************************************************************************************************************
		static inline bool Blend(uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply)
		{
			if (*srcPixels > 0xFF000000) return false; // No pixels to draw( fully transparent )

			uint32_t src = *srcPixels;
			uint32_t dest = *destPixels;

			// Note: our source alpha values are already 1-srcAlpha (to make the optimal BlendFast approach faster) so we need to flip them back again
			uint32_t srcAlpha = static_cast<int>((0xFF - (src >> 24)) * globalMultiply.alpha);
			uint32_t constAlpha = static_cast<int>(0xFF * globalMultiply.alpha);

			// Source pixels are already multiplied by srcAlpha so we just apply the constant alpha multiplier
			uint32_t destRed = (uint32_t)(constAlpha * globalMultiply.red * ((src >> 16) & 0xFF));
			uint32_t destGreen = (uint32_t)(constAlpha * globalMultiply.green * ((src >> 8) & 0xFF));
			uint32_t destBlue = (uint32_t)(constAlpha * globalMultiply.blue * (src & 0xFF));

			uint32_t invSrcAlpha = 0xFF - srcAlpha;

			// Apply a standard Alpha blend [ src*srcAlpha + dest*(1-SrcAlpha) ]
			destRed += invSrcAlpha * ((dest >> 16) & 0xFF);
			destGreen += invSrcAlpha * ((dest >> 8) & 0xFF);
			destBlue += invSrcAlpha * (dest & 0xFF);

			// Bring back to the range 0-255
			destRed >>= 8;
			destGreen >>= 8;
			destBlue >>= 8;

			// Put ARGB components back together again
			*destPixels = 0xFF000000 | (destRed << 16) | (destGreen << 8) | destBlue;

			return true;
		}

		// *******************************************************************************************************************************************************
		// An optimized approach which uses pre-multiplied alpha, parallel channel multiplication and pixel skipping to achieve the same 'typical' alpha 
		// blending operation (src * srcAlpha)+(dest * (1-srcAlpha)). Not easy to apply a global alpha multiplication over the top, but used everywhere else.
		// Notes: Requires a source buffer which has the source alpha pre-multiplied
		// *******************************************************************************************************************************************************
		static inline bool BlendFast(uint32_t*& srcPixels, uint32_t*& destPixels)
		{
			if (*srcPixels > 0xFF000000) return false; // No pixels to draw( fully transparent )

			// This performs the dest*(1-srcAlpha) calculation for all channels in parallel with minor accuracy loss in dest colour.
			// It does this by shifting all the destination channels down by 4 bits in order to "make room" for the later multiplication.
			// After shifting down, it masks out the bits which have shifted into the adjacent channel data.
			// This causes the RGB data to be rounded down to their nearest 16 producing a reduction in colour accuracy for pixels with alpha
			// This is then multiplied by the inverse alpha (inversed in PreMultiplyAlpha), also divided by 16 (hence >> 8+8+8+4).
			// The multiplication brings our RGB values back up to their original bit ranges (albeit rounded to the nearest 16).
			// As the colour accuracy only affects the destination pixels behind semi-transparent source pixels thia isn't very obvious.
			uint32_t dest = (((*destPixels >> 4) & 0x000F0F0F) * (*srcPixels >> 28));
			// Add the (pre-multiplied Alpha) source to the destination and force alpha to opaque
			*destPixels = (*srcPixels + dest) | 0xFF000000;

			return true;
		}
	};

	class AdditiveBlendPolicy
	{
	public:
		// Standard additive blending using pre-multiplied srcAlpha buffer: src*srcAlpha + dest*destAlpha
		// This isn't actually a very common requirement, so we default to the same global multiply approach below 
		static inline void BlendFastSkip(uint32_t*& srcPixels, uint32_t*& destPixels, const uint32_t* destRowEnd)
		{
			BlendSkip( srcPixels, destPixels, { 1.0f, 1.0f, 1.0f, 1.0f }, destRowEnd );
		}

		// Standard additive blending, with a global alpha multiply. This is the most common requirement for particle effects.
		static inline void BlendSkip( uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply, const uint32_t* destRowEnd )
		{
			// A full blend calculation is required for semi-transparent pixels with a global multiply
			// Fully transparent pixels can be skipped in the optimal way using the Skip function above   
			if( Blend( srcPixels, destPixels, globalMultiply ) )
				srcPixels++, destPixels++;
			else
				Skip( srcPixels, destPixels, destRowEnd );
		}

		// *******************************************************************************************************************************************************
		// A basic approach which separates the channels and performs an additive blending operation: (src * srcAlpha)+(dest * destAlpha)
		// Has the advantage that a global alpha multiplication can be easily added over the top, so we use this method when a global multiply is required
		// Notes: Requires a source buffer which has the source alpha pre-multiplied
		// *******************************************************************************************************************************************************
		static inline bool Blend(uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply)
		{
			if (*srcPixels > 0xFF000000) return false; // No pixels to draw( fully transparent )

			uint32_t src = *srcPixels;
			uint32_t dest = *destPixels;

			// Keep the blended alpha calculation in the high bits
			uint32_t destAlpha = dest >> 24;
			uint32_t srcAlpha = 0xFF - (src >> 24);
			uint32_t blendedAlpha = srcAlpha + destAlpha;

			// Source pixels are already multiplied by srcAlpha so we just apply the constant alpha multiplier
			uint32_t blendedRed = (uint32_t)( globalMultiply.alpha * globalMultiply.red * ((src >> 8) & 0xFF00));
			uint32_t blendedGreen = (uint32_t)( globalMultiply.alpha * globalMultiply.green * (src & 0xFF00));
			uint32_t blendedBlue = (uint32_t)( globalMultiply.alpha * globalMultiply.blue * ((src << 8) & 0xFF00));

			// Apply an additive blend [ src*srcAlpha + dest*destAlpha ]
			blendedRed += 0xFF * ((dest >> 16) & 0xFF);
			blendedGreen += 0xFF * ((dest >> 8) & 0xFF);
			blendedBlue += 0xFF * (dest & 0xFF);

			// Bring back to the range 0-255
			blendedRed >>= 8;
			blendedGreen >>= 8;
			blendedBlue >>= 8;

			if (blendedAlpha > 0xFF) blendedAlpha = 0xFF;
			if (blendedRed > 0xFF) blendedRed = 0xFF;
			if (blendedGreen > 0xFF) blendedGreen = 0xFF;
			if (blendedBlue > 0xFF) blendedBlue = 0xFF;

			// Put ARGB components back together again
			*destPixels = (blendedAlpha << 24) | (blendedRed << 16) | (blendedGreen << 8) | blendedBlue;

			return true;
		}
	};

	class MultiplyBlendPolicy
	{
	public:

		// Standard multipy blend using an unmodified srcAlpha buffer (the original canvas buffer): dest* invSrcAlpha + (src * dest) * srcAlpha
		static inline void BlendSkip(uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply, const uint32_t*)
		{
			// Transparent pixels still need to be multiplied, so we have only one route
			Blend(srcPixels, destPixels, globalMultiply);
			srcPixels++, destPixels++;
		}

		// Standard additive blending, with a global alpha multiply. This is the most common requirement for particle effects.
		static inline void BlendFastSkip(uint32_t*& srcPixels, uint32_t*& destPixels, const uint32_t*)
		{
			// Transparent pixels still need to be multiplied, so we have only one route
			Blend(srcPixels, destPixels, { 1.0f, 1.0f, 1.0f, 1.0f });
			srcPixels++, destPixels++;
		}

		// *******************************************************************************************************************************************************
		// A basic approach which separates the channels and performs an additive blending operation: (src * srcAlpha)+(dest * destAlpha)
		// Has the advantage that a global alpha multiplication can be easily added over the top, so we use this method when a global multiply is required
		// Notes: Requires a source buffer which has the source alpha unmodified
		// *******************************************************************************************************************************************************
		static inline void Blend(uint32_t*& srcPixels, uint32_t*& destPixels, BlendColour globalMultiply)
		{
			if (*srcPixels < 0x00FFFFFF) return; // No pixels to draw( fully transparent )

			uint32_t src = *srcPixels;
			uint32_t dest = *destPixels;

			// Source pixels
			uint32_t srcAlpha = src >> 24;
			uint32_t srcRed = (src >> 16) & 0xFF;
			uint32_t srcGreen = (src >> 8) & 0xFF;
			uint32_t srcBlue = src & 0xFF;

			// Destination pixels
			uint32_t destAlpha = dest >> 24;
			uint32_t destRed = (dest >> 16) & 0xFF;
			uint32_t destGreen = (dest >> 8) & 0xFF;
			uint32_t destBlue = dest & 0xFF;

			// Apply a multiplicative blend [ dest*invSrcAlpha + (src*dest)*srcAlpha ]
			uint32_t blendAlpha = static_cast<int>(srcAlpha * globalMultiply.alpha);
			uint32_t invBlendAlpha = (0xFF - blendAlpha) * 0xFF;
			uint32_t blendRed = (uint32_t)(globalMultiply.red * (destRed * invBlendAlpha) + ((srcRed * destRed) * blendAlpha));
			uint32_t blendGreen = (uint32_t)(globalMultiply.green * (destGreen * invBlendAlpha) + ((srcGreen * destGreen) * blendAlpha));
			uint32_t blendBlue = (uint32_t)(globalMultiply.blue * (destBlue * invBlendAlpha) + ((srcBlue * destBlue) * blendAlpha));

			// Bring back to the range 0-255
			blendRed >>= 16;
			blendGreen >>= 16;
			blendBlue >>= 16;

			// Shouldn't be necessary
			if (blendRed > 0xFF) blendRed = 0xFF;
			if (blendGreen > 0xFF) blendGreen = 0xFF;
			if (blendBlue > 0xFF) blendBlue = 0xFF;

			// Put ARGB components back together again
			*destPixels = (destAlpha << 24) | (blendRed << 16) | (blendGreen << 8) | blendBlue;
		}
	};
}
#endif
#ifndef PLAY_PLAYRENDER_H
#define PLAY_PLAYRENDER_H
//********************************************************************************************************************************
// File:		PlayRender.h
// Description:	A software pixel renderer for drawing 2D primitives into a PixelData buffer
// Platform:	Independent
// Notes:		The only internal state/data stored by the renderer is a pointer to the render target
//********************************************************************************************************************************
namespace Play::Render
{
	// Set the render target for all subsequent drawing operations
	// Returns a pointer to any previous render target
	PixelData* SetRenderTarget( PixelData* pRenderTarget );

	extern PixelData* m_pRenderTarget;

	// Primitive drawing functions
	//********************************************************************************************************************************

	// Sets the colour of an individual pixel on the render target
	template< typename TBlend > void DrawPixel(int posX, int posY, Pixel pix);
	// Sets the colour of an individual pixel on the render target
	template< typename TBlend > void DrawPixelPreMult(int posX, int posY, Pixel pix);

	// Draws a line of pixels into the render target
	void DrawLine( int startX, int startY, int endX, int endY, Pixel pix );
	// Draws pixel data to the render target using a direct copy
	// > Setting alphaMultiply < 1 forces a less optimal rendering approach (~50% slower) 
	template< typename TBlend > void BlitPixels(const PixelData& srcImage, int srcOffset, int blitX, int blitY, int blitWidth, int blitHeight, BlendColour globalMultiply );
	// Draws rotated and scaled pixel data to the render target (much slower than BlitPixels)
	// > Setting alphaMultiply < 1 is not much slower overall (~10% slower) 
	template< typename TBlend > void RotateScalePixels(const PixelData& srcPixelData, int srcFrameOffset, int srcWidth, int srcHeight, const Point2f& origin, const Matrix2D& m, BlendColour globalMultiply);
	// Clears the render target using the given pixel colour
	void ClearRenderTarget( Pixel colour );
	// Copies a background image of the correct size to the render target
	void BlitBackground( PixelData& backgroundImage );

	//********************************************************************************************************************************
	// Function:	BlitPixels - draws image data with and without a global alpha multiply
	// Parameters:	spriteId = the id of the sprite to draw
	//				xpos, ypos = the position you want to draw the sprite
	//				frameIndex = which frame of the animation to draw (wrapped)
	// Notes:		Blend implmentation depends on TBlend class (see PlayBlends.h) - should all end up inlined!
	//********************************************************************************************************************************
	template< typename TBlend > void BlitPixels(const PixelData& srcPixelData, int srcOffset, int blitX, int blitY, int blitWidth, int blitHeight, BlendColour globalMultiply)
	{
		blitY = m_pRenderTarget->height - blitY; // Flip the y-coordinate to be consistant with a Cartesian co-ordinate system

		// Nothing within the display buffer to draw
		if (blitX > m_pRenderTarget->width || blitX + blitWidth < 0 || blitY > m_pRenderTarget->height || blitY + blitHeight < 0)
			return;

		// Work out if we need to clip to the display buffer (and by how much)
		int xClipStart = -blitX;
		if (xClipStart < 0) { xClipStart = 0; }

		int xClipEnd = (blitX + blitWidth) - m_pRenderTarget->width;
		if (xClipEnd < 0) { xClipEnd = 0; }

		int yClipStart = -blitY;
		if (yClipStart < 0) { yClipStart = 0; }

		int yClipEnd = (blitY + blitHeight) - m_pRenderTarget->height;
		if (yClipEnd < 0) { yClipEnd = 0; }

		// Set up the source and destination pointers based on clipping
		int destOffset = (m_pRenderTarget->width * (blitY + yClipStart)) + (blitX + xClipStart);
		uint32_t* destPixels = &m_pRenderTarget->pPixels->bits + destOffset;

		int srcClipOffset = (srcPixelData.width * yClipStart) + xClipStart;
		uint32_t* srcPixels = &srcPixelData.pPixels->bits + srcOffset + srcClipOffset;

		// Work out in advance how much we need to add to src and dest to reach the next row 
		int destInc = m_pRenderTarget->width - blitWidth + xClipEnd + xClipStart;
		int srcInc = srcPixelData.width - blitWidth + xClipEnd + xClipStart;

		//Work out final pixel in destination.
		int destColOffset = (m_pRenderTarget->width * (blitHeight - yClipEnd - yClipStart - 1)) + (blitWidth - xClipEnd - xClipStart);
		uint32_t* destColEnd = destPixels + destColOffset;

		//How many pixels per row in sprite.
		int endRow = blitWidth - xClipEnd - xClipStart;

		if (globalMultiply.alpha < 1.0f || globalMultiply.red < 1.0f || globalMultiply.green < 1.0f || globalMultiply.blue < 1.0f )
		{
			// It is slightly faster to loop through without the additions 
			while (destPixels < destColEnd)
			{
				uint32_t* destRowEnd = destPixels + endRow;

				// Call the more versatile global multiply blend function
				while (destPixels < destRowEnd)
					TBlend::BlendSkip(srcPixels, destPixels, globalMultiply, destRowEnd);

				// Increase buffers by pre-calculated amounts
				destPixels += destInc;
				srcPixels += srcInc;
			}
		}
		else
		{
			// It is slightly faster to loop through without the additions 
			while (destPixels < destColEnd)
			{
				uint32_t* destRowEnd = destPixels + endRow;

				// Call the fastest available blend function
				while (destPixels < destRowEnd)
					TBlend::BlendFastSkip(srcPixels, destPixels, destRowEnd);

				// Increase buffers by pre-calculated amounts
				destPixels += destInc;
				srcPixels += srcInc;
			}
		}

		return;
	}

	//********************************************************************************************************************************
	// Function:	TransformPixels - draws the image data transforming each screen pixel into image space
	// Parameters:	srcPixelData = the pixel data you want to draw
	//				srcFrameOffset = the horizontal pixel offset for the required animation frame within the PixelData
	//				srcDrawWidth, srcDrawHeight = the width and height of the source image frame
	//				srcOrigin = the centre of rotation for the source image
	//				alphaMultiply = additional transparancy applied to the whole sprite
	// Notes:		Much slower than BlitPixels, alphaMultiply is a negligable overhead compared to the rotation
	//********************************************************************************************************************************
	template< typename TBlend > void TransformPixels(const PixelData& srcPixelData, int srcFrameOffset, int srcDrawWidth, int srcDrawHeight, const Point2f& srcOrigin, const Matrix2D& transform, BlendColour globalMultiply)
	{
		// We flip the y screen coordinate and reverse the rotatation to be consistant with a right-handed Cartesian co-ordinate system 
		Matrix2D right;
		right.row[0] = { transform.row[0].x, transform.row[1].x, 0.0f };
		right.row[1] = { transform.row[0].y, transform.row[1].y, 0.0f };
		right.row[2] = { transform.row[2].x, m_pRenderTarget->height - transform.row[2].y, 1.0f };

		static float inf = std::numeric_limits<float>::infinity();
		float dst_minx{ inf }, dst_miny{ inf }, dst_maxx{ -inf }, dst_maxy{ -inf };

		float x[2] = { -srcOrigin.x, srcDrawWidth - srcOrigin.x };
		float y[2] = { -srcOrigin.y, srcDrawHeight - srcOrigin.y };
		Point2f vertices[4] = { { x[0], y[0] }, { x[1], y[0] }, { x[1], y[1] }, { x[0], y[1] } };

		// Calculate the extremes of the rotated corners.
		for (int i = 0; i < 4; i++)
		{
			vertices[i] = right.Transform(vertices[i]);
			dst_minx = floor(dst_minx < vertices[i].x ? dst_minx : vertices[i].x);
			dst_maxx = ceil(dst_maxx > vertices[i].x ? dst_maxx : vertices[i].x);
			dst_miny = floor(dst_miny < vertices[i].y ? dst_miny : vertices[i].y);
			dst_maxy = ceil(dst_maxy > vertices[i].y ? dst_maxy : vertices[i].y);
		}

		// Calculate the inverse transform so that we can iterate through the render target's pixels within the sprite's space
		if (Determinant(right) == 0.0f) return;
		Matrix2D invTransform = right;
		invTransform.Inverse();

		// Calculate the minimum drawing area which would contain the rotated corners
		int dst_draw_width = static_cast<int>(dst_maxx - dst_minx);
		int dst_draw_height = static_cast<int>(dst_maxy - dst_miny);
		int dst_buffer_width = m_pRenderTarget->width;
		int dst_buffer_height = m_pRenderTarget->height;

		// Clip the drawing area if any of the rotated corners are outside of the render target buffer
		if (dst_miny < 0) { dst_draw_height += (int)dst_miny; dst_miny = 0; }
		if (dst_maxy > (float)dst_buffer_height) { dst_draw_height -= (int)dst_maxy - dst_buffer_height; dst_maxy = (float)dst_buffer_height; }
		if (dst_minx < 0) { dst_draw_width += (int)dst_minx; dst_minx = 0; }
		if (dst_maxx > (float)dst_buffer_width) { dst_draw_width -= (int)dst_maxx - dst_buffer_width;  dst_maxx = (float)dst_buffer_width; }

		// Transform the starting position within the render target into the sprite's space 
		Point2f dst_pixel_start{ dst_minx, dst_miny };
		Point2f src_pixel_start = invTransform.Transform(dst_pixel_start) + srcOrigin;

		// We need floating point for the sprite space as one pixel on the render target is not a whole pixel in the sprite
		float src_posx = src_pixel_start.x;
		float src_posy = src_pixel_start.y;

		// Integer arithmetic is best for the render target as we're working in whole pixels
		int dst_posx = static_cast<int>(dst_pixel_start.x);
		int dst_posy = static_cast<int>(dst_pixel_start.y);

		// The inverse transform matrix contains axis unit vectors for navigating render target space within sprite space
		float src_xincx = invTransform.row[0].x;
		float src_xincy = invTransform.row[0].y;
		float src_yincx = invTransform.row[1].x;
		float src_yincy = invTransform.row[1].y;
		float src_xresetx = src_xincx * dst_draw_width;
		float src_xresety = src_xincy * dst_draw_width;

		// Calculate the pixel start and end positions within the render target buffer
		int dst_start_pixel_index = dst_posx + (dst_posy * dst_buffer_width);
		uint32_t* dst_pixel = (uint32_t*)m_pRenderTarget->pPixels + dst_start_pixel_index;
		uint32_t* dst_pixel_end = dst_pixel + (dst_draw_height * dst_buffer_width);

		// Iterate sequentially through pixels within the render target buffer
		while (dst_pixel < dst_pixel_end)
		{
			// For each row of pixels in turn
			uint32_t* dst_row_end = dst_pixel + dst_draw_width;
			while (dst_pixel < dst_row_end)
			{
				// The origin of a pixel is in its centre
				int roundX = static_cast<int>(src_posx + 0.5f);
				int roundY = static_cast<int>(src_posy + 0.5f);

				// Clip within the sprite boundaries
				if (roundX >= 0 && roundY >= 0 && roundX < srcDrawWidth && roundY < srcDrawHeight)
				{
					int src_pixel_index = roundX + (roundY * srcPixelData.width);
					uint32_t* src = ((uint32_t*)srcPixelData.pPixels + src_pixel_index + srcFrameOffset);
					TBlend::Blend(src, dst_pixel, globalMultiply); // Perform the appropriate blend using a template
				}

				// Move one horizontal pixel in render target, which corresponds to the x axis of the inverse matrix in sprite space
				dst_pixel++;
				src_posx += src_xincx;
				src_posy += src_xincy;
			}

			// Move render target pointer to the start of the next row
			dst_pixel += dst_buffer_width - dst_draw_width;

			// Move sprite buffer pointer back to the start of the current row
			src_posx -= src_xresetx;
			src_posy -= src_xresety;

			// One vertical pixel in the render target corresponds to the y axis of the inverse matrix in sprite space
			src_posx += src_yincx;
			src_posy += src_yincy;
		}
	}

	template< typename TBlend > void DrawPixelPreMult(int posX, int posY, Pixel srcPixel)
	{
		if (srcPixel.a == 0x00 || posX < 0 || posX >= m_pRenderTarget->width || posY < 0 || posY >= m_pRenderTarget->height)
			return;

		// Pre-multiply alpha and invert
		srcPixel.r = (srcPixel.r * srcPixel.a) >> 8;
		srcPixel.g = (srcPixel.g * srcPixel.a) >> 8;
		srcPixel.b = (srcPixel.b * srcPixel.a) >> 8;
		srcPixel.a = 0xFF - srcPixel.a;

		uint32_t* pDest = &m_pRenderTarget->pPixels[(posY * m_pRenderTarget->width) + posX].bits;
		uint32_t* pSrc = &srcPixel.bits;

		TBlend::Blend(pSrc, pDest, { 1.0f, 1.0f, 1.0f, 1.0f });

		return;
	}

	template< typename TBlend > void DrawPixel(int posX, int posY, Pixel srcPixel)
	{
		if (srcPixel.a == 0x00 || posX < 0 || posX >= m_pRenderTarget->width || posY < 0 || posY >= m_pRenderTarget->height)
			return;

		uint32_t* pDest = &m_pRenderTarget->pPixels[(posY * m_pRenderTarget->width) + posX].bits;
		uint32_t* pSrc = &srcPixel.bits;

		TBlend::Blend(pSrc, pDest, { 1.0f, 1.0f, 1.0f, 1.0f });

		return;
	}
};
#endif // PLAY_PLAYRENDER_H
#ifndef PLAY_PLAYGRAPHICS_H
#define PLAY_PLAYGRAPHICS_H
//********************************************************************************************************************************
// File:		PlayGraphics.h
// Description:	Manages 2D graphics operations on a PixelData buffer 
// Platform:	Independent
// Notes:		Uses PNG format. The end of the filename indicates the number of frames e.g. "bat_4.png" or "tiles_10x10.png"
//********************************************************************************************************************************
namespace Play::Graphics
{
	enum BlendMode
	{
		BLEND_NORMAL = 0,
		BLEND_ADD,
		BLEND_MULTIPLY,
		BLEND_SUBTRACT
	};

	extern BlendMode blendMode;

	// Create/Destroy manager functions
	//********************************************************************************************************************************

	// Creates the PlayGraphics manager and generates sprites from all the PNGs in the directory indicated
	bool CreateManager( int bufferWidth, int bufferHeight, const char* path );
	// Destroys the PlayGraphics manager
	bool DestroyManager();

	// Basic drawing functions
	//********************************************************************************************************************************

	// Sets the colour of an individual pixel in the display buffer
	void DrawPixel( Point2f pos, Pixel pix );
	// Draws a line of pixels into the display buffer
	void DrawLine( Point2f startPos, Point2f endPos, Pixel pix );
	// Draws a rectangle into the display buffer
	void DrawRect( Point2f bottomLeft, Point2f topRight, Pixel pix, bool fill = false );
	// Draws a circle into the display buffer
	void DrawCircle( Point2f centrePos, int radius, Pixel pix );
	// Draws raw pixel data to the display buffer
	// > Pre-multiplies the alpha on the image data if this hasn't been done before
	void DrawPixelData( PixelData* pixelData, Point2f pos, float alpha = 1.0f );

	// Debug font functions
	//********************************************************************************************************************************

	// Draws a single character using the in-built debug font
	// > Returns the character width in pixels
	int DrawDebugCharacter( Point2f pos, char c, Pixel pix );
	// Draws text using the in-built debug font
	// > Returns the x position at the end of the text
	int DrawDebugString( Point2f pos, const std::string& s, Pixel pix, bool centred = true );

	// Sprite Loading functions
	//********************************************************************************************************************************

	// Loads a sprite sheet and creates a sprite from it (custom asset pipelines)
	// > All sprites are normally automatically created by the PlayGraphics constructor
	int LoadSpriteSheet( const std::string& path, const std::string& filename );
	// Adds a sprite sheet dynamically from memory (custom asset pipelines)
	// > All sprites are normally created by the PlayGraphics constructor
	int AddSprite( const std::string& name, PixelData& pixelData, int hCount = 1, int vCount = 1 );
	// Updates a sprite sheet dynamically from memory (custom asset pipelines)
	// > Left to caller to release old PixelData
	int UpdateSprite( const std::string& name, PixelData& pixelData, int hCount = 1, int vCount = 1 );
	// Regenerates the premultiplied alpha data.
	int UpdateSprite( const std::string& name );
	
	// Loads a background image which is assumed to be the same size as the display buffer
	// > Returns the index of the loaded background
	int LoadBackground( const char* fileAndPath );

	// Sprite Getters and Setters
	//********************************************************************************************************************************

	// Gets the sprite id of the first matching sprite whose filename contains the given text
	// > Returns -1 if not found
	int GetSpriteId( const char* spriteName );
	// Gets the root filename of a specific sprite
	const std::string& GetSpriteName( int spriteId );
	// Gets the size of the sprite with the given id
	Vector2f GetSpriteSize(int spriteId);
	// Gets the number of frames in the sprite with the given id
	int GetSpriteFrames( int spriteId );
	// Gets the origin of the sprite with the given id (offset from bottom left)
	Vector2f GetSpriteOrigin( int spriteId );
	// Sets the origin of the sprite with the given id (offset from bottom left)
	void SetSpriteOrigin( int spriteId, Vector2f newOrigin, bool relative = false );
	// Vertically flips the origin of the sprite with the given id (for legacy sprite origins)
	void FlipSpriteOriginVertically( int spriteId );
	// Vertically flips the origin of all sprites (for legacy sprite origins)
	void FlipAllSpriteOriginsVertically( void );
	// Centres the origin of the sprite with the given id
	void CentreSpriteOrigin( int spriteId );
	// Centres the origins of all the sprites
	void CentreAllSpriteOrigins();
	// Sets the origin of all sprites found matching the given name (offset from bottom left)
	void SetSpriteOrigins( const char* rootName, Vector2f newOrigin, bool relative = false );
	// Gets the number of sprites which have been loaded and created by PlayGraphics
	int GetTotalLoadedSprites();
	// Gets a (read only) pointer to a sprite's canvas buffer data
	const PixelData* GetSpritePixelData( int spriteId ); 

	// Sprite Drawing functions
	//********************************************************************************************************************************

	// Draw the sprite with transparency (slower than without transparency)
	void DrawTransparent(int spriteId, Point2f pos, int frameIndex, BlendColour globalMultiply = { 1.0f, 1.0f, 1.0f, 1.0f } ); 
	// Draw the sprite without rotation or transparency (fastest draw)
	inline void Draw( int spriteId, Point2f pos, int frameIndex ) { DrawTransparent( spriteId, pos, frameIndex ); } // DrawTransparent only ends up performing a global multiply if any of its values are < 1.0f
	// Draw the sprite rotated with transparency (slowest draw)
	void DrawRotated( int spriteId, Point2f pos, int frameIndex, float angle, float scale = 1.0f, BlendColour globalMultiply = { 1.0f, 1.0f, 1.0f, 1.0f } );
	// Draw the sprite using a matrix transformation and transparency (slowest draw)
	void DrawTransformed( int spriteId, const Matrix2D& transform, int frameIndex, BlendColour globalMultiply = { 1.0f, 1.0f, 1.0f, 1.0f } );
	// Draws a previously loaded background image
	void DrawBackground( int backgroundIndex = 0 );
	// Multiplies the sprite image buffer by the colour values
	// > Applies to all subseqent drawing calls for this sprite, but can be reset by calling agin with rgb set to white
	void ColourSprite( int spriteId, int r, int g, int b );

	// Draws a string using a sprite-based font exported from PlayFontTool
	int DrawString( int fontId, Point2f pos, std::string text );
	// Draws a centred string using a sprite-based font exported from PlayFontTool
	int DrawStringCentred( int fontId, Point2f pos, std::string text );
	// Draws an individual text character using a sprite-based font 
	int DrawChar( int fontId, Point2f pos, char c );
	// Draws a rotated text character using a sprite-based font 
	int DrawCharRotated( int fontId, Point2f pos, float angle, float scale, char c );
	// Gets the width of an individual text character from a sprite-based font
	int GetFontCharWidth( int fontId, char c );

	// A pixel-based sprite collision test (slooow!)
	int SpriteCollide( int spriteIdA, int frameIndexA, Matrix2D& transA, int spriteIdB, int frameIndexB, Matrix2D& transB );

	// Internal sprite structure for storing individual sprite data
	struct Sprite
	{
		int id{ -1 }; // Fast way of finding the right sprite
		std::string name; // Slow way of finding the right sprite
		int width{ -1 }, height{ -1 }; // The width and height of a single image in the sprite
		//int canvasWidth{ -1 }, canvasHeight{ -1 }; // The width and height of the entire sprite canvas
		int hCount{ -1 }, vCount{ -1 }, totalCount{ -1 };  // The number of sprite images in the canvas horizontally and vertically
		int originX{ 0 }, originY{ 0 }; // The origin and centre of rotation for the sprite (whole pixels only)
		PixelData canvasBuffer; // The sprite image data
		PixelData preMultAlpha; // The sprite data pre-multiplied with its own alpha
		Sprite() = default;
	};

	// Miscellaneous functions
	//********************************************************************************************************************************

	// Gets a pointer to the drawing buffer's pixel data
	PixelData* GetDrawingBuffer(void);
	// Resets the timing bar data and sets the current timing bar segment to a specific colour
	void TimingBarBegin( Pixel pix );
	// Sets the current timing bar segment to a specific colour
	// > Returns the number of timing segments
	int SetTimingBarColour( Pixel pix );
	// Draws the timing bar for the previous frame at the given position and size
	void DrawTimingBar( Point2f pos, Point2f size );
	// Gets the duration (in milliseconds) of a specific timing segment
	float GetTimingSegmentDuration( int id );
	// Clears the display buffer using the given pixel colour
	inline void ClearBuffer( Pixel colour ) { Render::ClearRenderTarget( colour ); }
	// Sets the render target for drawing operations
	inline PixelData* SetRenderTarget(PixelData* renderTarget) { return Render::SetRenderTarget(renderTarget); }
	// Set the blend mode for all subsequent drawing operations that support different blend modes
	inline void SetBlendMode(BlendMode bMode) { blendMode = bMode; }
};
#endif // PLAY_PLAYGRAPHICS_H
#ifndef PLAY_PLAYAUDIO_H
#define PLAY_PLAYAUDIO_H
//********************************************************************************************************************************
// File:		PlayAudio.h
// Description:	Declarations for a simple audio manager 
// Platform:	Independant
//********************************************************************************************************************************
namespace Play::Audio
{
	// Initialises the audio manager, using the directory provided as its root for finding .WAV files
	bool CreateManager( const char* path );
	// Destroys any memory associated with the audio manager
	bool DestroyManager();
	// Play a sound using part of all of its filename, returns the voice id
	int StartSound( const char* name, bool bLoop = false, float volume = 1.0f, float freqMod = 1.0f);
	//  Stop a currently playing sound using its voice id
	bool StopSound( int voiceId ); 
	//  Stop all currently playing sounds with the same filename 
	bool StopSound( const char* name );
	// Set the volume of a looping sound that's playing using name
	void SetLoopingSoundVolume( const char* name, float volume = 1.0f);
	// Set the volume of a looping sound that's playing using id
	void SetLoopingSoundVolume( int voiceId, float volume = 1.0f);
	// Set the pitch of a looping sound that's playing using name
	void SetLoopingSoundPitch(const char* name, float freqMod = 1.0f);
	// Set the pitch of a looping sound that's playing using id
	void SetLoopingSoundPitch(int voiceId, float freqMod = 1.0f);
};
#endif // PLAY_PLAYAUDIO_H

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


#ifndef PLAY_PLAYMANAGER_H
#define PLAY_PLAYMANAGER_H
//********************************************************************************************************************************
// File:		PlayManager.h
// Description:	A manager for providing simplified access to the PlayBuffer framework with managed GameObjects
// Platform:	Independent
// Notes:		The GameObject management is "opt in" as many will want to create their own GameObject class
//********************************************************************************************************************************

#define TRANSFORM_SPACE( p )  (Play::drawSpace == Play::DrawingSpace::WORLD ? p - Play::cameraPos : p)
#define TRANSFORM_MATRIX_SPACE( t ) (Play::drawSpace == Play::DrawingSpace::WORLD ? t * (MatrixTranslation( -Play::cameraPos.x, -Play::cameraPos.y )) : t)

//! @brief Main Namespace for PlayBuffer
namespace Play
{
	//! @brief Alignment for font drawing operations
	enum Align
	{
		//! @brief Text is aligned to the left of the origin.
		LEFT = 0, 
		//! @brief Text is aligned to the right of the origin.
		RIGHT, 
		//! @brief Text is aligned to the middle of the origin.
		CENTRE,
	};

	//! @brief Used to indicate which edge of the screen you want to check if a GameObject is moving off.
	enum Direction
	{
		//! @brief Check if it's moving off the left or right sides of the screen.
		HORIZONTAL = 0, 
		//! @brief Check if it's moving off the top or bottom sides of the screen.
		VERTICAL, 
		//! @brief Check if it's moving off any side of the screen.
		ALL,
	};


	//! @brief The drawing space values.
	//! The drawing space determines where things are drawn on the screen. There are two values for this, WORLD and SCREEN.
		enum DrawingSpace
	{
		//! @brief This takes the camera position into account when things are drawn. Essentially, the position that is used is DrawingPosition - CameraPosition.
		WORLD = 0, 
		//! @brief This ignores the camera position, and purely uses the position that gets passed through. This is most useful for user interface elements that should always be drawn in the same position on the screen.
		SCREEN,
	};

	//! @brief The Blend modes for sprite drawing.
	enum BlendMode
	{
		//! @brief This blends the sprite normally. If using a transparent sprite draw, this also uses the transparency of the sprite.
		BLEND_NORMAL = 0, 
		//! @brief This uses an additive blend, where the colour values of the sprite being drawn are added to the buffer underneath. This has the effect of brightening what is underneath the sprite.
		BLEND_ADD, 
		//! @brief This uses an multiplicative blend, where the colour values of the sprite being drawn are multiplied with what is in the buffer underneath. This will darken what is underneath the sprite.
		BLEND_MULTIPLY
	};

	//! @brief A PlayBuffer colour value. Colours are defined in percentages of red, green and blue. All zero is black, All 100 is white.
	struct Colour
	{
		//! Create a colour using float values.
		//! @param r Amount of red.
		//! @param g Amount of green.
		//! @param b Amount of blue.
		Colour( float r, float g, float b ) : red( r ), green( g ), blue( b ) {}
		//! Create a colour using int values.
		//! @param r Amount of red.
		//! @param g Amount of green.
		//! @param b Amount of blue.
		Colour( int r, int g, int b ) : red( static_cast<float>( r ) ), green( static_cast<float>( g ) ), blue( static_cast<float>( b ) ) {}
		//! @var red 
		//! Red value, from 0 to 100.
		//! @var green 
		//! Green value, from 0 to 100.
		//! @var blue
		//! Blue value, from 0 to 100.
		float red, green, blue;
	};

	extern Colour cBlack, cRed, cGreen, cBlue, cMagenta, cCyan, cYellow, cOrange, cWhite, cGrey;
	extern int frameCount;
	extern Point2f cameraPos;
	extern DrawingSpace drawSpace;



	// Manager creation and deletion
	//**************************************************************************************************

	//! @brief Initialises the manager and creates a window of the required dimensions.
	//! @param width The width of the window in pixels.
	//! @param height The height of the window in pixels.
	//! @param scale Pixel scale. One-pixel equals (scale x scale) pixels in final window.
	void CreateManager( int width, int height, int scale );
	//! @brief Shuts down the manager and closes the window.
	void DestroyManager();
	// Get the width of the play buffer
	inline int GetBufferWidth() { return Play::Window::GetWidth(); }
	// Get the height of the play buffer
	inline int GetBufferHeight() { return Play::Window::GetHeight(); }

	// PlayWindow functions
	//**************************************************************************************************

	//! @brief Copies the contents of the drawing buffer to the window
	void PresentDrawingBuffer();
	//! @brief Gets the co-ordinates of the mouse cursor within the display buffer
	//! @return The x/y coordinates of the mouse cursor in pixels.
	inline Point2D GetMousePos() { return Play::Input::GetMousePos(); }
	//! @brief Gets the status of the mouse buttons
	//! @param button Mouse button to check
	//! @return The x/y coordinates of the mouse cursor in pixels.
	inline bool GetMouseButton( Align button ) { return Play::Input::GetMouseDown( static_cast<Play::Input::MouseButton>(button) ); }
	//! @brief Gets the width of the display buffer
	//! @return Width of the display buffer in pixels.
	int GetBufferWidth();
	//! @brief Gets the height of the display buffer
	//! @return Height of the display buffer in pixels.
	int GetBufferHeight();

	// PlayAudio functions
	//**************************************************************************************************

	//! @brief Plays an audio file from the "Data/Audio" directory, in WAV format.
	//! @param wavFilename The filename of the audio file you want to play.
	//! @return The id of the sound that is playing.
	inline int PlayAudio( const char* wavFilename ) { return Play::Audio::StartSound( wavFilename, false, 1.0f, 1.0f ); }
	//! @brief Plays an audio file from the "Data/Audio" directory, in WAV format, at a random pitch from min to max percent.
	//! @param wavFilename The filename of the audio file you want to play.
	//! @param minPercent Minimum random pitch, as a percentage.
	//! @param maxPercent Maximum random pitch, as a percentage.
	//! @return The id of the sound that is playing.
	int PlayAudioPitch( const char* wavFilename, int minPercent, int maxPercent );
	//! @brief Plays an audio file from the "Data/Audio" directory, in WAV format, and loops it repeatedly until you stop it
	//! @param wavFilename The filename of the audio file you want to play.
	//! @return The id of the sound that is playing.
	inline int StartAudioLoop( const char* wavFilename ) { return Play::Audio::StartSound( wavFilename, true, 1.0f, 1.0f ); }
	//! @brief Stops a looping audio file, using the sound ID that the StartAudioLoop function returned.
	//! @param soundId The ID of the sound you want to stop.
	//! @return Whether the sound stopped playing successfully.
	inline bool StopAudioLoop( int soundId ) { return Play::Audio::StopSound( soundId ); }
	//! @brief Stops a sound, based upon the filename
	//! @param fileName The filename of the audio file you want to stop. This can be either a normal sound or a looping sound.
	//! @return Whether the sound stopped playing successfully.
	inline bool StopAudio( const char* fileName ) { return Play::Audio::StopSound( fileName ); }

	// Camera functions
	//**************************************************************************************************

	//! @brief Move the camera to the position specified
	//! @param pos The desired position of the camera
	inline void SetCameraPosition( Point2f pos ) { cameraPos = pos; }
	//! @brief Changes the drawing space for all drawing functions.
	//! @param space What drawing space should the item be drawn in.
	inline void SetDrawingSpace( DrawingSpace space ) { drawSpace = space; }
	//! @brief Get the camera's current position
	//! @return The position of the camera
	inline Point2f GetCameraPosition( void ) { return cameraPos; }
	//! @brief Gets the current drawing space.
	//! @return What drawing space PlayBuffer is currently using
	inline DrawingSpace GetDrawingSpace( void ) { return drawSpace; }

	// PlayGraphics functions
	//**************************************************************************************************

	//! @brief Clears the display buffer using the colour provided
	//! @param col The colour to clear the drawing buffer with.
	void ClearDrawingBuffer( Colour col );
	//! @brief Loads a PNG file as a background image for the window.
	//! The image must be the same size as the drawing buffer. It cannot be smaller or larger. Multiple backgrounds can be loaded, and will have it's own index (from zero).
	//! @param pngFilename The full path and filename of the background image in PNG format.
	//! @return The index of the newly loaded background
	inline int LoadBackground( const char* pngFilename ) { return Play::Graphics::LoadBackground( pngFilename ); }
	//! @brief Draws the background image previously loaded with Play::LoadBackground() into the drawing buffer.
	//! @param background Optional argument to specify the background index. Defaults to zero (the first loaded background).
	inline void DrawBackground( int background = 0 ) { Play::Graphics::DrawBackground( background ); }
	//! @brief Draws text to the screen, using the built in debug font.
	//! @param pos Position of the text on screen. This position is affected by the Drawing Space.
	//! @param text The text you want to draw, as a string literal.
	//! @param col Optional argument to set the colour of the text (defaults to white).
	//! @param centred Optional argument to centre the text (defaults to true).
	void DrawDebugText( Point2D pos, const char* text, Colour col = cWhite, bool centred = true );

	//! @brief Gets the sprite ID of the first matching sprite whose filename contains the given text.
	//! @param spriteName The name of the sprite you want to find the ID for. 
	//! @return The ID of the sprite, if PlayBuffer could find it. If PlayBuffer couldn't find the sprite, then it returns -1
	inline int GetSpriteId( const char* spriteName ) { return Play::Graphics::GetSpriteId( spriteName ); }
	//! @brief Gets the pixel height of a sprite
	//! @param spriteName The name of the sprite you want to find the height of. 
	//! @return The height of the sprite in pixels
	inline int GetSpriteHeight( const char* spriteName ) { return static_cast<int>(Play::Graphics::GetSpriteSize( GetSpriteId( spriteName ) ).height); }
	//! @brief Gets the pixel width of a sprite
	//! @param spriteName The name of the sprite you want to find the width of. 
	//! @return The width of the sprite in pixels
	inline int GetSpriteWidth( const char* spriteName ) { return static_cast<int>(Play::Graphics::GetSpriteSize( GetSpriteId( spriteName ) ).width); }
	//! @brief Gets the pixel height of a sprite
	//! @param spriteId The Id number of the sprite you want to find the height of. 
	//! @return The height of the sprite in pixels.
	inline int GetSpriteHeight( int spriteId ) { return static_cast<int>(Play::Graphics::GetSpriteSize( spriteId ).height); }
	//! @brief Gets the pixel width of a sprite
	//! @param spriteId The Id number of the sprite you want to find the width of. 
	//! @return The width of the sprite in pixels
	inline int GetSpriteWidth( int spriteId ) { return static_cast<int>(Play::Graphics::GetSpriteSize( spriteId ).width); }
	//! @brief Gets the stem filename for the sprite this does not include the path or the extension.
	//! @param spriteId The Id number of the sprite you want to get the filename of.
	//! @return The stem filename for the sprite
	inline const char* GetSpriteName( int spriteId ) { return Play::Graphics::GetSpriteName( spriteId ).c_str(); }
	//! @brief Gets the total number of frames in the sprite.
	//! @param spriteId The Id number of the sprite you want to find the number of frames of. 
	//! @return The number of frames that the sprite has
	inline int GetSpriteFrames( int spriteId ) { return static_cast<int>(Play::Graphics::GetSpriteFrames( spriteId )); }
	//! @brief Blends the sprite with the given colour.
	//! This function works best with sprites that are coloured white, as it multiplies the colour of the sprite with the colour that you provide.
	//! @note Sprite colouring changes the loaded in sprite. This will result in the sprite being permanently coloured for the remainder of the game. (This doesn't affect the sprite on disk!)<br>You can reset the colouring by calling this again, using a white colour.
	//! @param spriteName The name of the sprite you want to colour.
	//! @param col The colour you want to blend with the sprite.
	void ColourSprite( const char* spriteName, Colour col );

	//! @brief Centres the origin of the first sprite found matching the given name
	//! @param spriteName The name of the sprite you want to centre the origin of.
	void CentreSpriteOrigin( const char* spriteName );
	//! @brief Centres the origins of all sprites found with matching parts of their name.
	//! @param partName A common part of a sprite name to centre all the origins of.
	void CentreMatchingSpriteOrigins( const char* partName );
	//! @brief Centres the origins of all loaded sprites.
	void CentreAllSpriteOrigins();
	//! @brief Moves the origins of a sprite by a given number of pixels in x and y. A sprite's origin does not have to be inside the sprite itself and can be moved any distance.
	//! @param spriteName The name of the sprite you want to centre the origin of.
	//! @param xOffset The number of pixels you want to move the origin to the right (negative value moves left).
	//! @param yOffset The number of pixels you want to move the origin up (negative value moves down).
	void MoveSpriteOrigin( const char* spriteName, int xOffset, int yOffset );
	//! @brief Moves the origins of all sprites found matching the given name by a given number of pixels. A sprite's origin does not have to be inside the sprite itself and can be moved any distance.
	//! @param partName A common part of a sprite name to centre all the origins of.
	//! @param xoffset The number of pixels you want to move the origin to the right (negative value moves left).
	//! @param yoffset The number of pixels you want to move the origin up (negative value moves down).
	inline void MoveMatchingSpriteOrigins( const char* partName, int xoffset, int yoffset ) { Play::Graphics::SetSpriteOrigins( partName, { xoffset, yoffset }, true ); }
	//! @brief Moves the origins of all sprites by a given number of pixels in x and y. A sprite's origin does not have to be inside the sprite itself and can be moved any distance.
	//! @param xoffset The number of pixels you want to move the origin to the right (negative value moves left).
	//! @param yoffset The number of pixels you want to move the origin up (negative value moves down).
	void MoveAllSpriteOrigins( int xoffset, int yoffset );
	//! @brief Sets the origins of a sprite, to the x and y coordinates, relative to the bottom left corner of the sprite. A sprite's origin does not have to be inside the sprite itself and can be positioned anywhere.
	//! @param spriteName The name of the sprite you want to set the origin of.
	//! @param xOrigin Where you want to move the origin on the x axis, relative to the left of the sprite.
	//! @param yOrigin Where you want to move the origin on the y axis, relative to the bottom of the sprite.
	void SetSpriteOrigin( const char* spriteName, int xOrigin, int yOrigin );
	//! @brief Sets the origins of a sprite, to the x and y coordinates, relative to the bottom left corner of the sprite. A sprite's origin does not have to be inside the sprite itself and can be positioned anywhere.
	//! @param spriteId The id of the sprite you want to set the origin of. 
	//! @param xOrigin Where you want to move the origin on the x axis, relative to the left of the sprite.
	//! @param yOrigin Where you want to move the origin on the y axis, relative to the bottom of the sprite.
	inline void SetSpriteOrigin( int spriteId, int xOrigin, int yOrigin ) { Play::Graphics::SetSpriteOrigin( spriteId, { xOrigin, yOrigin } ); }
	//! @brief Gets the origin of the first sprite found matching the given name. This value is relative to the bottom left corner of the sprite.
	//! @param spriteName The name of the sprite you want to get the origin of.
	inline Point2D GetSpriteOrigin( const char* spriteName ) { return Play::Graphics::GetSpriteOrigin( GetSpriteId( spriteName ) ); }
	//! @brief Gets the origin of the sprite with the given ID. This value is relative to the bottom left corner of the sprite.
	//! @param spriteId The ID of the sprite you want to get the origin of.
	//! @return The origin of the sprite, relative to it's bottom left corner.
	inline Point2D GetSpriteOrigin( int spriteId ) { return Play::Graphics::GetSpriteOrigin( spriteId ); }
	//! @brief Gets a pointer to the raw pixel data of the sprite. The pixel data is read-only, and cannot be edited.
	//! @param spriteId The ID of the sprite you want to get the pixel data of.
	//! @return A pointer to the raw pixel data. Read-only.
	inline const PixelData* GetSpritePixelData( int spriteId ) { return Play::Graphics::GetSpritePixelData( spriteId ); }

	//! @brief Set the blend mode for all subsequent drawing operations that support different blend modes.
	//! @param blendMode The blend mode that you want to draw things with.
	inline void SetDrawingBlendMode( BlendMode blendMode ) { Graphics::SetBlendMode( static_cast<Graphics::BlendMode>(blendMode) ); }
	//! @brief Draws the first matching sprite whose filename contains the given text.
	//! @param spriteName The name of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frameIndex When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	inline void DrawSprite( const char* spriteName, Point2D pos, int frameIndex ) { Play::Graphics::Draw( Play::Graphics::GetSpriteId( spriteName ), TRANSFORM_SPACE( pos ), frameIndex ); }
	//! @brief Draws the sprite with the matching sprite ID. Using this is more efficient than drawing it using the sprite name.
	//! @param spriteID The ID of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frameIndex When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	inline void DrawSprite( int spriteID, Point2D pos, int frameIndex ) { Play::Graphics::Draw( spriteID, TRANSFORM_SPACE( pos ), frameIndex ); }
	//! @brief Draws the first matching sprite whose filename contains the given text, using transparency. This is slower than DrawSprite and should only be used if you need transparency.
	//! @param spriteName The name of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all).
	//! @param colour The colour tint of the sprite.
	void DrawSpriteTransparent( const char* spriteName, Point2D pos, int frame, float opacity, Colour colour = cWhite );
	//! @brief Draws the sprite with the matching sprite ID, using transparency. This is slower than DrawSprite and should only be used if you need transparency.
	//! @param spriteID The ID of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all).
	//! @param colour The colour tint of the sprite. Defaults to white.
	void DrawSpriteTransparent( int spriteID, Point2D pos, int frame, float opacity, Colour colour = cWhite );
	//! @brief Draws the first matching sprite whose filename contains the given text, using the specified angle, scale, and opacity. Note that this is the slowest sprite draw function and so should only be used when you need rotation or scale.
	//! @param spriteName The name of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param angle Angle in radians to rotate the sprite clockwise.
	//! @param scale Amount to scale the sprite, with 1.0f being full size, 0.5f half size, 2.0f double sized and so on. Defaults to 1.0f (normal scale).
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all). Defaults to 1.0f (completely opaque).
	//! @param colour The colour tint of the sprite. Defaults to white.
	void DrawSpriteRotated( const char* spriteName, Point2D pos, int frame, float angle, float scale = 1.0f, float opacity = 1.0f, Colour colour = cWhite );
	//! @brief Draws the sprite with the matching sprite ID, using the specified angle, scale, and opacity. Note that this is the slowest sprite draw function and so should only be used when you need rotation or scale.
	//! @param spriteID The ID of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param angle Angle in radians to rotate the sprite clockwise.
	//! @param scale Amount to scale the sprite, with 1.0f being full size, 0.5f half size, 2.0f double sized and so on. Defaults to 1.0f (normal scale).
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all). Defaults to 1.0f (completely opaque).
	//! @param colour The colour tint of the sprite. Defaults to white.
	void DrawSpriteRotated( int spriteID, Point2D pos, int frame, float angle, float scale = 1.0f, float opacity = 1.0f, Colour colour = cWhite );
	//! @brief Draws the sprite with the matching sprite ID, using a transformation matrix. This can be slower or faster depending on the contents of the matrix.
	//! @param spriteID The ID of the sprite you want to draw.
	//! @param transform The transformation matrix that you want to use to draw the sprite with.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all). Defaults to 1.0f (completely opaque).
	//! @param colour The colour tint of the sprite. Defaults to white.
	void DrawSpriteTransformed( int spriteID, const Matrix2D& transform, int frame, float opacity = 1.0f, Colour colour = cWhite );
	//! @brief Draws a single-pixel wide line between two points in the given colour.
	//! @param start The x/y coordinate for the start point of the line.
	//! @param end The x/y coordinate for the end point of the line.
	//! @param col The colour of the line.
	void DrawLine( Point2D start, Point2D end, Colour col );
	//! @brief Draws a single-pixel wide circle at a given origin.
	//! @param pos The x/y coordinate for the origin of the circle.
	//! @param radius The length of the circle's radius in pixels.
	//! @param col The colour of the circle.
	void DrawCircle( Point2D pos, int radius, Colour col );
	//! @brief Draws a rectangle, defined by the bottom left and top right corners, in the given colour.
	//! @param bottomLeft The x/y coordinate for the bottom left corner.
	//! @param topRight The x/y coordinate for the top right corner.
	//! @param col The colour of the rectangle.
	//! @param fill Is the rectangle filled in? Defaults to not filled in.
	void DrawRect( Point2D bottomLeft, Point2D topRight, Colour col, bool fill = false );
	//! @brief Draws a line between two points using a sprite as a 'pen', blended with the given colour. Note that colouring affects subsequent DrawSprite calls using the same sprite!
	//! @param startPos The x/y coordinate for the start point of the line.
	//! @param endPos The x/y coordinate for the end point of the line.
 	//! @param penSprite The sprite to use as the 'pen'.
	//! @param col The colour of the sprites drawing the line.
	void DrawSpriteLine( Point2D startPos, Point2D endPos, const char* penSprite, Colour col = cWhite );
	//! @brief Draws a circle using a sprite as a 'pen', blended with the given colour. Note that colouring affects subsequent DrawSprite calls using the same sprite!
	//! @param pos The x/y coordinate for the origin of the circle.
	//! @param radius The length of the circle's radius in pixels.
	//! @param penSprite The sprite to use as the 'pen'.
	//! @param col The colour of the sprites drawing the circle.
	void DrawSpriteCircle( Point2D pos, int radius, const char* penSprite, Colour col = cWhite );
	//! @brief Draws text using a sprite-based font exported from PlayFontTool.
	//! @param fontId The unique sprite id of the font you want to use.
	//! @param text The string containing the text you want to draw.
	//! @param pos The x/y coordinate for the location for text to be drawn at.
	//! @param justify Optional argument determining whether the text is left, right, or centre justified (defaults to left justified).
	void DrawFontText( const char* fontId, std::string text, Point2D pos, Align justify = Align::LEFT );
	//! @brief Draws a single pixel on screen.
	//! @param pos The x/y coordinate of the pixel you wish to draw.
	//! @param col The colour of the pixel.
	void DrawPixel( Point2D pos, Colour col );

	//! @brief Resets the timing bar data and sets the current timing bar segment to a specific colour
	//! @param colour Colour of the timing bar segment.
	void BeginTimingBar( Colour colour );
	// Sets the current timing bar segment to a specific colour
	//! @param colour Colour of the timing bar segment.
	//! @return The number of timing segments
	int ColourTimingBar( Colour colour );
	//! @brief Draws the timing bar for the previous frame at the given position and size
	//! @param pos The x/y coordinate you want to draw the timing bar at.
	//! @param size The size of the timing bar in pixels.
	inline void DrawTimingBar( Point2f pos, Point2f size ) { Play::Graphics::DrawTimingBar( pos, size ); }

	// Miscellaneous functions
	//**************************************************************************************************

	//! @brief Returns true if the key has been pressed since it was last released.
	//! @param key The key that you want to check for.
	//! @return If the key has been pressed. This will return true on the frame the key is pushed down, but after that point will return false, even if held.
	inline bool KeyPressed( KeyboardButton key ) { return Play::Input::KeyPress( key, frameCount ); }
	//! @brief Returns true if the key is currently pressed down.
	//! @param key The key that you want to check for.
	//! @return If the key has been pressed. This will return true while it is down, and false if not pressed
	inline bool KeyDown( KeyboardButton key ) { return Play::Input::KeyHeld( key ); }
	//! @brief Returns a random number as if you rolled a die with this many sides.
	//! @param sides How many sides the 'die' has.
	//! @return The number that was rolled.
	inline int RandomRoll( int sides ) { return (rand() % sides) + 1; }
	//! @brief Returns a random number, between the min and max parameters.
	//! @param min The smallest number that the roll can return.
	//! @param max The largest number that the roll can return.
	//! @return The number that was rolled.
	int RandomRollRange( int min, int max );
}
#endif // PLAY_PLAYMANAGER_H
#ifndef PLAY_PLAYOBJECT_H
#define PLAY_PLAYOBJECT_H
//********************************************************************************************************************************
// PlayBuffer provides a managed Play::GameObject structure which can be extended below. Adding additional member variables to
// the struct will cause them to be added to ALL GameObjects regardless of their type, so you may want to consider creating your 
// own GameObject class heirarchy on the longer term.
// Note: Play::GameObject and its associated functionality is only included if you #define PLAY_USING_GAMEOBJECT_MANAGER
//********************************************************************************************************************************
#ifdef PLAY_USING_GAMEOBJECT_MANAGER

	
//! \brief Main Namespace for PlayBuffer
namespace Play
{
	//! @brief The Gameobject struct. Holds all the data that a GameObject requires, and can be extended by the user.
	struct GameObject
	{
		//! @brief GameObject constructor.
		//! @param type The type of the GameObject.
		//! @param pos The initial x/y coordinates of the GameObject.
		//! @param collisionRadius The radius of the collision circle of this GameObject.
		//! @param spriteId The sprite ID of the GameObject.
		GameObject(int type, Point2D pos, int collisionRadius, int spriteId); 

		// Default member variables: don't change these!
		//! A number representing the type of the GameObject as an int or enum. So a type of 1 might correspond to a health pickup and a type of 2 might correspond to a missile, for example. The only type value defined by PlayManager is -1, which corresponds to "no type". It is up to the user to decide how to assign other GameObject types. PlayManager will simply treat each unique value as a distinct type.
		int type{ -1 };
		//! If the object has changed type since it was created, then this holds the previous type of the object.
		int oldType{ -1 };
		//! The unique id of the sprite to be associated with the GameObject as used in certain DrawSprite commands.
		int spriteId{ -1 }; 
		//! The x/y position where the origin of the GameObject is placed. Used for calculating collisions as well as drawing the sprite.
		Point2D pos{ 0.0f, 0.0f }; 
		//! The x/y position of the GameObject from the previous frame.
		Point2D oldPos{ 0.0f, 0.0f };
		//! An x/y vector that determines how the position of the GameObject changes each time it is updated. The x-velocity will be added to the x position, and y-velocity to the y position.
		Vector2D velocity{ 0.0f, 0.0f };
		//! An x/y vector that determines how the velocity of the GameObject changes each time it is updated. The x-acceleration will be added to the x velocity, and y-acceleration to the y velocity.
		Vector2D acceleration{ 0.0f, 0.0f };
		//! The angle by which GameObject should be rotated when it is drawn. Measured in radians, clockwise from 12-o'clock.
		float rotation{ 0.0f };
		 //! A constant value which is added to the rotation of the GameObject each time it is updated.
		float rotSpeed{ 0.0f };
		//! The angle of rotation of the GameObject from the previous frame.
		float oldRot{ 0.0f };
		 //! The sprite frame to use when the GameObject is drawn.
		int frame{ 0 };
		//! The fraction of the sprite frame that the animation is currently on. animSpeed adds on to this value every frame, and when this rolls over 1, then it resets back down and increments frame.
		float framePos{ 0.0f };
		//! The amount the sprite frame is increased by each time the GameObject is updated. Numbers greater than 1 will result in frame-skipping. Numbers less than 1 will slow the animation down, but are best kept to fractions that divide equally into 1 (0.2, 0.5. 0.333, etc).
		float animSpeed{ 0.0f };
		//! The distance away from GameObject's origin to detect collisions. Measured in pixels.
		int radius{ 0 };
		//! The size to draw the sprite associated with the GameObject. 1.0f is full size, 0.5f half size, 2.0f double size, and so on.
		float scale{ 1 };
		int order{ 0 };
		//! What frame did this GameObject last get updated on? This stops GameObjects being updated multiple times per frame.
		int lastFrameUpdated{ -1 };

		// Add your own member variables here and every GameObject will have them
		// int something{ 0 };

		//! @brief Allows you to get the unique ID of a GameObject if you only have a reference or a copy of it.
		//! @return This GameObject's unique ID.
		int GetId() { return m_id; }

	private:
		// The GameObject's id should never be changed manually so we make it private!
		int m_id{ -1 };

		// Preventing assignment and copying reduces the potential for bugs
		GameObject& operator=(const GameObject&) = delete;
		GameObject(const GameObject&) = delete;
	};

	// GameObject functions
	//**************************************************************************************************

	//! @brief Creates a new GameObject and adds it to the managed list.
	//! @param type The type of the GameObject.
	//! @param pos The initial x/y coordinates of the GameObject.
	//! @param collisionRadius The radius of the collision circle of this GameObject.
	//! @param spriteName The name of the sprite to use for the GameObject.
	//! @return Returns the new object's unique id.
	int CreateGameObject(int type, Point2D pos, int collisionRadius, const char* spriteName);
	//! @brief Retrieves a GameObject from the ID passed to this function.
	//! @param id The ID of the GameObject you wish to retrieve.
	//! @return The game object associated with that ID. An object with a type of -1 is returned if no object can be found.
	GameObject& GetGameObject(int id);
	//! @brief Retrieves the first GameObject matching the type that you pass through as a parameter.
	//! @param type The type of the GameObject you wish to retrieve.
	//! @return The first GameObject that matches the given type. An object with a type of -1 is returned if no object can be found.
	GameObject& GetGameObjectByType(int type);
	//! @brief Collects the IDs of all of the GameObjects with the matching type
	//! @param type The type of the GameObjects you wish to retrieve.
	//! @return A vector containing IDs of GameObjects of that type. The vector will be empty if no GameObjects match the type.
	std::vector<int> CollectGameObjectIDsByType(int type);
	//! @brief Collects the IDs of all of the GameObjects
	//! @return A vector containing the IDs of all of the GameObjects that the manager contains. The vector will be empty if there are no GameObjects.
	std::vector<int> CollectAllGameObjectIDs();
	//! @brief Performs a typical update of the object's position and animation. Changes its velocity by its acceleration, its position by its velocity, its rotation by its rotation speed, and its animation frame by its animation speed.
	//! @note Can only be called once per object per frame unless allowMultipleUpdatesPerFrame is set to true.
	//! @param object The GameObject you wish to update.
	//! @param bWrap Should the object wrap around the edge of the screen to the other side? Defaults to no.
	//! @param wrapBorderSize If the object is wrapping, then how far off the edge of the screen should the object get before it wraps? Defaults to 0 pixels.
	//! @param allowMultipleUpdatesPerFrame If set to true, then this allows for the object to be updated again if it already has this frame.
	void UpdateGameObject(GameObject& object, bool bWrap = false, int wrapBorderSize = 0, bool allowMultipleUpdatesPerFrame = false);
	//! @brief Deletes the GameObject with the corresponding Id.
	//! @param id The unique id of the GameObject you wish to delete.
	void DestroyGameObject(int id);
	//! @brief Deletes all GameObjects with the corresponding type.
	//! @param type The type of the GameObjects you wish to delete.
	void DestroyGameObjectsByType(int type);
	//! @brief Deletes all GameObjects.
	void DestroyAllGameObjects();

	//! @brief Checks whether the two GameObjects are within each other's collision radii.
	//! @param obj1 The first GameObject we want to check has collided.
	//! @param obj2 The second GameObject we want to check has collided.
	//! @return Returns true if the GameObjects are overlapping, false otherwise.
	bool IsColliding(GameObject& obj1, GameObject& obj2);
	//! @brief Checks whether any part of the GameObject is visible within the DisplayBuffer
	//! @param obj The GameObject that we want to check for visibility.
	bool IsVisible(GameObject& obj);
	//! @brief Checks whether the GameObject is overlapping the edge of the screen and moving outwards.
	//! @param obj The GameObject that we want to check for overlapping.
	//! @param dirn Which side of the screen are we checking? Defaults to all sides.
	//! @return Returns true if the object is overlapping, false otherwise.
	bool IsLeavingDisplayArea(GameObject& obj, Direction dirn = Direction::ALL);
	//! @brief Checks whether the animation has completed playing.
	//! @param obj The GameObject that we want to check.
	//! @return Returns true if the animation has finished, false if it hasn't.
	bool IsAnimationComplete(GameObject& obj);
	//! @brief Sets the velocity and angle of the GameObject.
	//! @param obj The GameObject you wish to change the velocity and direction of.
	//! @param speed The number of pixels you want the GameObject to move in a straight line each time it is updated.
	//! @param rotation The angle you want the GameObject to move in.
	void SetGameObjectDirection(GameObject& obj, int speed, float rotation);
	//! @brief Sets the rotation of the object to point towards the target coordinate.
	//! @param obj The GameObject you wish to change the direction of.
	//! @param targetX The x coordinate of the point you wish the object to point at.
	//! @param targetY The y coordinate of the point you wish the object to point at.
	void PointGameObject(GameObject& obj,int targetX, int targetY);
	//! @brief Changes the GameObject's current spite and resets its animation frame to the start.
	//! @param obj The GameObject you wish to set the sprite for.
	//! @param spriteName The name of the sprite you wish to set for the GameObject.
	//! @param animSpeed The number of frames to increase the animation by each time the GameObject is updated.
	void SetSprite(GameObject& obj, const char* spriteName, float animSpeed );
	//! @brief Changes the GameObject's current spite and resets its animation frame to the start.
	//! @param obj The GameObject you wish to set the sprite for.
	//! @param spriteIndex The index of the sprite you wish to set for the GameObject.
	//! @param animSpeed The number of frames to increase the animation by each time the GameObject is updated.
	void SetSprite( GameObject& obj, int spriteIndex, float animSpeed );
	//! @brief Draws the GameObject's sprite without rotation or transparency. This is the fastest way to draw a GameObject, and so should be the preferred method when rotation and alpha are not required.
	//! @param obj The GameObject you wish to draw.
	void DrawObject(GameObject& obj);
	//! @brief Draws the GameObject's sprite with transparency. This is slower than DrawObject and so should only be used if you require transparency.
	//! @param obj The GameObject you wish to draw.
	//! @param opacity How transparent the object should be. 0.0f is fully transparent and 1.0f is fully opaque.
	void DrawObjectTransparent(GameObject& obj, float opacity);
	//! @brief Draws the object's sprite with rotation, scale, and transparency. This is slower than DrawObject or DrawObjectTransparent and so should only be used if you require rotation or scale. Rotation and scale are taken from the properties of the GameObject.
	//! @param obj The GameObject you wish to draw.
	//! @param opacity How transparent the object should be. 0.0f is fully transparent and 1.0f is fully opaque.
	void DrawObjectRotated(GameObject& obj, float opacity = 1.0f);
	//! @brief Draws debug info for all of the GameObjects that exist.
	//! @details This includes the object's ID, sprite name and current animation frame. It will also draw the sprite's boundaries and the collision radius of the object.
	void DrawGameObjectsDebug();
}
#endif
#endif // PLAY_PLAYOBJECT_H

#endif // PLAYPCH_H
//*******************************************************************
//*******************************************************************
#ifdef PLAY_IMPLEMENTATION
//*******************************************************************
//*******************************************************************
//********************************************************************************************************************************
//* File:			PlayMemory.cpp
//* Platform:		Independent
//* Description:	Implementation of a simple memory tracker to prevent leaks. 
//*                 Avoids use of STL or anything else which allocates memory as this could create infinite loops!
//* Notes:          This is a simple approach to support your understanding of overriding the new operator.
//*                 See below for alternative approaches:
//*                 1) The CRT Debug Heap Library 
//*                 https://docs.microsoft.com/en-us/visualstudio/debugger/crt-debug-heap-details?view=vs-2019
//*                 2) Heap Profiling in the debugger 
//*                 https://docs.microsoft.com/en-us/visualstudio/profiling/memory-usage?view=vs-2019
//********************************************************************************************************************************


#pragma comment(lib, "DbgHelp.lib")

#ifdef _DEBUG

namespace Play
{
	constexpr int MAX_ALLOCATIONS = 8192 * 4;
	constexpr int MAX_FILENAME = 1024;
	constexpr int STACKTRACE_OFFSET = 2;
	constexpr int STACKTRACE_DEPTH = 1;

	static unsigned int g_allocId = 0;

	// A structure to store data on each memory allocation
	struct ALLOC
	{
		void* address = 0;
		size_t sizeBytes = 0;
		int id = 0;
		void* stack[STACKTRACE_DEPTH];
		int frames;
	};

	ALLOC g_allocations[MAX_ALLOCATIONS];
	unsigned int g_allocCount = 0;
	int g_id = -1;
	void* g_allocDeleted = nullptr;

	CRITICAL_SECTION g_allocCritSec;

	// A method for printing out all the memory allocation immediately before program exit (or as close as you can get)
	// This is achieved by creating a class as a static object before the first memory allocation, which stays in scope until
	// after everything else is destroyed. When it is cleaned up its destructor prints the allocations to show us any leaks!
	class DestroyedLast
	{
	public:
		DestroyedLast()
		{
			InitializeCriticalSectionAndSpinCount(&g_allocCritSec, 64);

#ifdef _DEBUG
			HANDLE process = GetCurrentProcess();
			SymInitialize(process, NULL, TRUE);
			SymSetOptions(SYMOPT_LOAD_LINES);
#endif

			PrintAllocations("<STARTUP>");
		}
		~DestroyedLast()
		{
			if (g_allocCount > 0)
			{
				PrintAllocations("<MEMORY LEAK>");
			}
			else
			{
				DebugOutput("**************************************************\n");
				DebugOutput("NO MEMORY LEAKS!\n");
				DebugOutput("**************************************************\n");
			}

			DeleteCriticalSection(&g_allocCritSec);
		}
	};

	void CreateStaticObject(void)
	{
		static DestroyedLast last;
	}

	void TrackAllocation(void* ptr, std::size_t sizeBytes)
	{
		CreateStaticObject();

		EnterCriticalSection(&g_allocCritSec);

		PLAY_ASSERT(g_allocCount < MAX_ALLOCATIONS);
		ALLOC& rAlloc(g_allocations[g_allocCount++]);
		rAlloc.address = ptr;
		rAlloc.sizeBytes = sizeBytes;
		rAlloc.frames = RtlCaptureStackBackTrace(STACKTRACE_OFFSET, STACKTRACE_DEPTH, rAlloc.stack, NULL);

		LeaveCriticalSection(&g_allocCritSec);
	}

	void UntrackAllocation(void* ptr)
	{
		EnterCriticalSection(&g_allocCritSec);
		g_allocDeleted = ptr;

		for (unsigned int i = 0; i < g_allocCount; i++)
		{
			if (g_allocations[i].address == ptr)
			{
				if (g_allocations[i].id == g_id)
					g_allocations[i].id = g_id;

				g_allocations[i] = g_allocations[g_allocCount - 1];
				g_allocations[g_allocCount - 1].address = nullptr;
				g_allocCount--;

				// Early out
				LeaveCriticalSection(&g_allocCritSec);
				return;
			}
		}
		LeaveCriticalSection(&g_allocCritSec);
	}

	//********************************************************************************************************************************
	// Printing allocations
	//********************************************************************************************************************************

	void PrintAllocation(const char* tagText, const ALLOC& rAlloc)
	{
		char buffer[MAX_FILENAME * 2] = { 0 };

		if (rAlloc.address != nullptr)
		{
			HANDLE process = GetCurrentProcess();

			DWORD  dwDisplacement;

			IMAGEHLP_LINE64 line;
			constexpr size_t MAX_SYM_NAME_LENGTH = 255;
			SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME_LENGTH + 1);
			symbol->MaxNameLen = MAX_SYM_NAME_LENGTH;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

			sprintf_s(buffer, "%s 0x%02p %d bytes [%d]: \n", tagText, rAlloc.address, static_cast<int>(rAlloc.sizeBytes), rAlloc.id);
			DebugOutput(buffer);
			for (int i = 0; i < rAlloc.frames; ++i)
			{
				SymFromAddr(process, (DWORD64)(rAlloc.stack[i]), 0, symbol);
				SymGetLineFromAddr64(process, (DWORD64)rAlloc.stack[i], &dwDisplacement, &line);

				// Format in such a way that VS can double click to jump to the allocation.
				sprintf_s(buffer, "%s(%d): %s\n", line.FileName, line.LineNumber, symbol->Name);

				DebugOutput(buffer);
			}

			free(symbol);
		}
	}

	void PrintAllocations(const char* tagText)
	{
		EnterCriticalSection(&g_allocCritSec);

		int bytes = 0;
		char buffer[MAX_FILENAME * 2] = { 0 };
		DebugOutput("****************************************************\n");
		DebugOutput("MEMORY ALLOCATED\n");
		DebugOutput("****************************************************\n");
		for (unsigned int n = 0; n < g_allocCount; n++)
		{
			ALLOC& a = g_allocations[n];
			PrintAllocation(tagText, a);
			bytes += static_cast<int>(a.sizeBytes);
		}
		sprintf_s(buffer, "%s Total = %d bytes\n", tagText, bytes);
		DebugOutput(buffer);
		DebugOutput("**************************************************\n");

		LeaveCriticalSection(&g_allocCritSec);
	}

} // namespace Play

//********************************************************************************************************************************
// Overrides for new operator (x2)
//********************************************************************************************************************************
void* operator new  (std::size_t sizeBytes)
{
	void* ptr = malloc(sizeBytes);
	Play::TrackAllocation(ptr, sizeBytes);
	return ptr;
}

void* operator new[](std::size_t sizeBytes)
{
	void* ptr = malloc(sizeBytes);
	Play::TrackAllocation(ptr, sizeBytes);
	return ptr;
}

//********************************************************************************************************************************
// Overrides for delete operator (x2)
//********************************************************************************************************************************
void operator delete  (void* ptr) noexcept
{
	Play::UntrackAllocation(ptr);
	free(ptr);
}

void operator delete[](void* ptr) noexcept
{
	Play::UntrackAllocation(ptr);
	free(ptr);
}
#endif

//********************************************************************************************************************************
// File:		PlayWindow.cpp
// Description:	Platform specific code to provide a window to draw into
// Platform:	Windows
// Notes:		Uses a 32-bit ARGB display buffer
//********************************************************************************************************************************

using namespace Play; 

// Instruct Visual Studio to add these to the list of libraries to link
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")

// External functions which must be implemented by the user 
extern void MainGameEntry( int argc, char* argv[] ); 
extern bool MainGameUpdate( float ); // Called every frame
extern int MainGameExit( void ); // Called on quit
	
ULONG_PTR g_pGDIToken = 0;

#define ASSERT_WINDOW PLAY_ASSERT_MSG( Play::Window::m_bCreated, "Window Manager not initialised. Call Window::CreateManager() before using the Play::Window library functions.")

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	// Initialize GDI+
	Gdiplus::GdiplusStartupInput startupInput;
	ULONG_PTR token;
	Gdiplus::Status gdiStatus = Gdiplus::GdiplusStartup(&token, &startupInput, NULL);
	PLAY_ASSERT(Gdiplus::Ok == gdiStatus);
	g_pGDIToken = token;

	MainGameEntry(__argc, __argv);

	return Play::Window::HandleWindows( hInstance, hPrevInstance, lpCmdLine, nShowCmd, L"PlayBuffer" );
}

namespace Play::Window
{
	// Private data
	int m_scale{ 0 };
	PixelData* m_pPlayBuffer{ nullptr };
	MouseData* m_pMouseData{ nullptr };
	HWND m_hWindow{ nullptr };
	bool m_bCreated = false;

	//********************************************************************************************************************************
	// Create / Destroy functions for the Window Manager
	//********************************************************************************************************************************

	bool CreateManager( PixelData* pDisplayBuffer, int nScale )
	{
		PLAY_ASSERT(pDisplayBuffer);
		PLAY_ASSERT(nScale > 0);
		m_pPlayBuffer = pDisplayBuffer;
		m_scale = nScale;
		m_bCreated = true;
		return true;
	}

	bool DestroyManager( void )
	{
		ASSERT_WINDOW;
		m_bCreated = false;
		return true;
	}

	//********************************************************************************************************************************
	// Windows functions
	//********************************************************************************************************************************

	int HandleWindows( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow, LPCWSTR windowName )
	{
		ASSERT_WINDOW;

		UNREFERENCED_PARAMETER(hPrevInstance);
		UNREFERENCED_PARAMETER(lpCmdLine);

		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Play::Window::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = windowName;
		wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

		RegisterClassExW(&wcex);

		int	w = m_pPlayBuffer->width * m_scale;
		int h = m_pPlayBuffer->height * m_scale;

		UINT dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
		RECT rect = { 0, 0, w, h };
		AdjustWindowRect(&rect, dwStyle, FALSE);
		HWND hWnd = CreateWindowW(windowName, windowName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

		if (!hWnd)
			return FALSE;

		m_hWindow = hWnd;

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);

		HACCEL hAccelTable = LoadAccelerators(hInstance, windowName);

		LARGE_INTEGER frequency;
		double elapsedTime = 0.0;
		static LARGE_INTEGER lastDrawTime;
		LARGE_INTEGER now;

		MSG msg{};
		bool quit = false;

		// Set up counters for timing the frame
		QueryPerformanceCounter(&lastDrawTime);
		QueryPerformanceFrequency(&frequency);

		// Standard windows message loop
		while (!quit)
		{
			// Hangle windows messages
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;

				if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			do
			{
				QueryPerformanceCounter(&now);
				elapsedTime = (now.QuadPart - lastDrawTime.QuadPart) * 1000.0 / frequency.QuadPart;

			} while (elapsedTime < 1000.0f / FRAMES_PER_SECOND);

			// Call the main game update function (only while we have the input focus in release mode)
#ifndef _DEBUG
			if (GetFocus() == m_hWindow)
#endif
				quit = MainGameUpdate(static_cast<float>(elapsedTime) / 1000.0f);

			lastDrawTime = now;

			DwmFlush(); // Waits for DWM compositor to finish
		}

		// Call the main game cleanup function
		MainGameExit();

		PLAY_ASSERT(g_pGDIToken);
		Gdiplus::GdiplusShutdown(g_pGDIToken);

		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		ASSERT_WINDOW;

		switch (message)
		{
		case WM_PAINT:
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_LBUTTONDOWN:
			if (m_pMouseData)
				m_pMouseData->left = true;
			break;
		case WM_LBUTTONUP:
			if (m_pMouseData)
				m_pMouseData->left = false;
			break;
		case WM_RBUTTONDOWN:
			if (m_pMouseData)
				m_pMouseData->right = true;
			break;
		case WM_RBUTTONUP:
			if (m_pMouseData)
				m_pMouseData->right = false;
			break;
		case WM_MOUSEMOVE:
			if (m_pMouseData)
			{
				m_pMouseData->pos.x = static_cast<float>(GET_X_LPARAM(lParam) / m_scale);
				m_pMouseData->pos.y = m_pPlayBuffer->height - static_cast<float>(GET_Y_LPARAM(lParam) / m_scale);
			}
			break;
		case WM_MOUSELEAVE:
			m_pMouseData->pos.x = -1;
			m_pMouseData->pos.y = -1;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	double Present( void )
	{
		ASSERT_WINDOW;

		LARGE_INTEGER frequency;
		LARGE_INTEGER before;
		LARGE_INTEGER after;
		QueryPerformanceCounter(&before);
		QueryPerformanceFrequency(&frequency);

		// Set up a BitmapInfo structure to represent the pixel format of the display buffer
		BITMAPINFOHEADER bitmap_info_header
		{
				sizeof(BITMAPINFOHEADER),								// size of its own data,
				m_pPlayBuffer->width, m_pPlayBuffer->height,		// width and height
				1, 32, BI_RGB,				// planes must always be set to 1 (docs), 32-bit pixel data, uncompressed 
				0, 0, 0, 0, 0				// rest can be set to 0 as this is uncompressed and has no palette
		};

		BITMAPINFO bitmap_info{ bitmap_info_header, { 0,0,0,0 } };	// No palette data required for this bitmap

		HDC hDC = GetDC(m_hWindow);

		// Copy the display buffer to the window: GDI only implements up scaling using simple pixel duplication, but that's what we want
		// Note that GDI+ DrawImage would do the same thing, but it's much slower! 
		StretchDIBits(hDC, 0, 0, m_pPlayBuffer->width * m_scale, m_pPlayBuffer->height * m_scale, 0, m_pPlayBuffer->height + 1, m_pPlayBuffer->width, -m_pPlayBuffer->height, m_pPlayBuffer->pPixels, &bitmap_info, DIB_RGB_COLORS, SRCCOPY); // We flip h because Bitmaps store pixel data upside down.

		ReleaseDC(m_hWindow, hDC);

		QueryPerformanceCounter(&after);

		double elapsedTime = (after.QuadPart - before.QuadPart) * 1000.0 / frequency.QuadPart;

		return elapsedTime;
	}

	void RegisterMouse( MouseData* pMouseData ) 
	{ 
		ASSERT_WINDOW;
		m_pMouseData = pMouseData; 
	}

	int GetWidth() 
	{ 
		ASSERT_WINDOW;
		return m_pPlayBuffer->width; 
	}

	int GetHeight() 
	{ 
		ASSERT_WINDOW;
		return m_pPlayBuffer->height;
	}

	int GetScale() 
	{ 
		ASSERT_WINDOW;
		return m_scale; 
	}

}

//********************************************************************************************************************************
// Loading functions
//********************************************************************************************************************************

int ReadPNGImage(std::string& fileAndPath, int& width, int& height)
{
	// Convert filename from single to wide string for GDI+ compatibility
	size_t newsize = strlen(fileAndPath.c_str()) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, fileAndPath.c_str(), _TRUNCATE);

	HBITMAP hBitmap = NULL;

	// Use GDI+ to load file into Bitmap structure
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(wcstring, false);
	int status = bitmap->GetLastStatus();

	if (status != Gdiplus::Ok)
		return -status;

	bitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);

	width = bitmap->GetWidth();
	height = bitmap->GetHeight();

	delete bitmap;
	delete[] wcstring;

	return 1;
}

int LoadPNGImage(std::string& fileAndPath, PixelData& destImage)
{
	// Convert filename from single to wide string for GDI+ compatibility
	size_t newsize = strlen(fileAndPath.c_str()) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, fileAndPath.c_str(), _TRUNCATE);

	HBITMAP hBitmap = NULL;

	// Use GDI+ to load file into Bitmap structure
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(wcstring, false);
	int status = bitmap->GetLastStatus();

	if (status != Gdiplus::Ok)
		return -status;

	bitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);

	// Create BitmapData structure to pull the data into
	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;

	destImage.width = bitmap->GetWidth();
	destImage.height = bitmap->GetHeight();

	// Lock the bitmap in advance of reading its data
	Gdiplus::Rect rect(0, 0, destImage.width, destImage.height);
	bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData);

	// Get a pointer to the actual bit data
	Pixel* sourcePixels = static_cast<Pixel*>(bitmapData->Scan0);

	destImage.pPixels = new Pixel[destImage.width * destImage.height];
	memset(destImage.pPixels, 0, sizeof(Pixel) * destImage.width * destImage.height);

	Pixel* destPixels = destImage.pPixels;

	// Copy the data across
	for (int b = 0; b < destImage.width * destImage.height; b++)
		*destPixels++ = *sourcePixels++;

	//Unlock the bitmap
	bitmap->UnlockBits(bitmapData);

	delete bitmap;
	delete bitmapData;
	delete[] wcstring;

	return 1;
}

int GetEncoderClsid( const WCHAR* format, CLSID* pClsid )
{
	UINT num = 0;
	UINT size = 0;

	Gdiplus::GetImageEncodersSize( &num, &size );
	if( size == 0 )
		return -1;

	Gdiplus::ImageCodecInfo* imageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc( size ));
	if( imageCodecInfo == NULL )
		return -1;

	Gdiplus::GetImageEncoders( num, size, imageCodecInfo );

	for( UINT j = 0; j < num; ++j )
	{
		if( wcscmp( imageCodecInfo[ j ].MimeType, format ) == 0 )
		{
			*pClsid = imageCodecInfo[ j ].Clsid;
			free( imageCodecInfo );
			return j;
		}
	}

	free( imageCodecInfo );
	return -1;
}

int SavePNGImage( std::string& fileAndPath, const PixelData& sourceImage )
{
	// Convert filename from single to wide string for GDI+ compatibility
	size_t newsize = strlen( fileAndPath.c_str() ) + 1;
	wchar_t* wcstring = new wchar_t[ newsize ];
	size_t convertedChars = 0;
	mbstowcs_s( &convertedChars, wcstring, newsize, fileAndPath.c_str(), _TRUNCATE );

	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap( sourceImage.width, sourceImage.height, PixelFormat32bppARGB );
	Gdiplus::Rect rect( 0, 0, sourceImage.width, sourceImage.height );
	Gdiplus::BitmapData bitmapData;

	// Lock the bitmap in advance of writing its data
	bitmap->LockBits( &rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData );

	Pixel* sourcePixels = sourceImage.pPixels;
	Pixel* destPixels = static_cast<Pixel*>(bitmapData.Scan0);

	// Copy the data from source image to destination bitmap
	for( int b = 0; b < sourceImage.width * sourceImage.height; b++ )
		*destPixels++ = *sourcePixels++;

	// Unlock the bitmap
	bitmap->UnlockBits( &bitmapData );

	// Save the bitmap as a PNG file
	CLSID pngClsid;
	GetEncoderClsid( L"image/png", &pngClsid );
	int status = bitmap->Save( wcstring, &pngClsid, NULL );

	delete bitmap;
	delete[] wcstring;

	if( status != Gdiplus::Ok )
		return -status;

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
	std::string s = p.filename().string() + " : LINE " + std::to_string(line);
	s += "\n" + std::string(message);
	int wide_count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
	wchar_t* wide = new wchar_t[wide_count];
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, wide, wide_count);
	MessageBox(NULL, wide, (LPCWSTR)L"Assertion Failure", MB_ICONWARNING);
	delete[] wide;
}

void DebugOutput( const char* s )
{
	OutputDebugStringA(s);
}

void DebugOutput( std::string s )
{
	OutputDebugStringA(s.c_str());
}

void TracePrintf( const char* file, int line, const char* fmt, ... )
{
	constexpr size_t kMaxBufferSize = 512u;
	char buffer[kMaxBufferSize];

	va_list args;
	va_start(args, fmt);
	// format should be double click-able in VS 
	int len = sprintf_s(buffer, kMaxBufferSize, "%s(%d): ", file, line);
	vsprintf_s(buffer + len, kMaxBufferSize - len, fmt, args);
	DebugOutput(buffer);
	va_end(args);
}

//********************************************************************************************************************************
// File:		PlayRender.cpp
// Description:	A software pixel renderer for drawing 2D primitives into a PixelData buffer
// Platform:	Independent
//********************************************************************************************************************************


#define ASSERT_RENDERTARGET	PLAY_ASSERT_MSG( Play::Render::m_pRenderTarget, "Render Target not initialised. Call Render::SetRenderTarget() before using the Play::Render library functions.")

namespace Play::Render
{
	// Internal (private) namespace variables
	PixelData* m_pRenderTarget{ nullptr };

	PixelData* SetRenderTarget( PixelData* pRenderTarget ) 
	{ 
		PixelData* old = m_pRenderTarget; 
		m_pRenderTarget = pRenderTarget; 
		return old; 
	}

	void DrawLine( int startX, int startY, int endX, int endY, Pixel pix ) 
	{
		ASSERT_RENDERTARGET;

		//Implementation of Bresenham's Line Drawing Algorithm
		int dx = abs(endX - startX);
		int sx = 1;
		if (endX < startX) { sx = -1; }

		int dy = -abs(endY - startY);
		int sy = 1;
		if (endY < startY) { sy = -1; }

		int err = dx + dy;

		if (dx == 0 && dy == 0) return;

		int x = startX;
		int y = startY;

		while (true)
		{
			Graphics::DrawPixel({ x, y }, pix); // Axis flipping is performed within

			if (x == endX && y == endY)
				break;

			int e2 = 2 * err;
			if (e2 >= dy)
			{
				err += dy;
				x += sx;
			}
			if (e2 <= dx)
			{
				err += dx;
				y += sy;
			}
		}
	}

	void ClearRenderTarget( Pixel colour ) 
	{
		ASSERT_RENDERTARGET;
		Pixel* pBuffEnd = m_pRenderTarget->pPixels + (m_pRenderTarget->width * m_pRenderTarget->height);
		for (Pixel* pBuff = m_pRenderTarget->pPixels; pBuff < pBuffEnd; *pBuff++ = colour.bits);
		m_pRenderTarget->preMultiplied = false;
	}

	void BlitBackground( PixelData& backgroundImage ) 
	{
		ASSERT_RENDERTARGET;
		PLAY_ASSERT_MSG(backgroundImage.height == m_pRenderTarget->height && backgroundImage.width == m_pRenderTarget->width, "Background size doesn't match render target!");
		// Takes about 1ms for 720p screen on i7-8550U
		memcpy(m_pRenderTarget->pPixels, backgroundImage.pPixels, sizeof(Pixel) * m_pRenderTarget->width * m_pRenderTarget->height);
	}
}
//********************************************************************************************************************************
// File:		PlayGraphics.cpp
// Description:	Manages 2D graphics operations on a PixelData buffer 
// Platform:	Independent
// Notes:		Uses PNG format. The end of the filename indicates the number of frames e.g. "bat_4.png" or "tiles_10x10.png"
//********************************************************************************************************************************


#define ASSERT_GRAPHICS PLAY_ASSERT_MSG( Play::Graphics::m_bCreated, "Graphics Manager not initialised. Call Graphics::CreateManager() before using the Play::Graphics library functions.")

namespace Play::Graphics
{
	// Internal (private) declarations
	// 
	// Flag to record whether the manager has been created
	bool m_bCreated = false;
	// Count of the total number of sprites loaded
	int m_nTotalSprites{ 0 };
	// Buffer pointers
	PixelData m_playBuffer;
	// A buffer for unpacking the debug font into 
	uint8_t* m_pDebugFontBuffer{ nullptr };

	// A vector of all the loaded sprites
	std::vector< Sprite > m_vSpriteData;
	// A vector of all the loaded backgrounds
	std::vector< PixelData > m_vBackgroundData;

	// Multiplies the sprite image by its own alpha transparency values to save repeating this calculation on every draw
	// > A colour multiplication can also be applied at this stage, which affects all subseqent drawing operations on the sprite
	void PreMultiplyAlpha( Pixel* source, Pixel* dest, int width, int height, int maxSkipWidth, float alphaMultiply, Pixel colourMultiply );
	// Allocates a buffer for the debug font and copies the font pixel data to it
	void DecompressDubugFont( void );
	// Returns the pixel width of a string using the debug font
	int GetDebugStringWidth( const std::string& s );
	// Draws the offset points from the origin in all octants
	void DrawCircleOctants( int posX, int posY, int offX, int offY, Pixel pix );
	// Ends the current timing segment and calculates the duration
	LARGE_INTEGER EndTimingSegment();

	struct TimingSegment
	{
		Pixel pix;
		long long begin{ 0 };
		long long end{ 0 };
		float millisecs{ 0 };
	};

	// Vectors of timing data segments
	std::vector<TimingSegment> m_vTimings;
	std::vector<TimingSegment> m_vPrevTimings;

	// The blend mode state
	BlendMode blendMode{ BLEND_NORMAL };

	bool CreateManager( int bufferWidth, int bufferHeight, const char* path )
	{
		PLAY_ASSERT_MSG( !m_bCreated, "Graphics Manager already initialised! Cannot call Graphics::CreateManager() more than once.");

		m_bCreated = true;

		// A working buffer for our display. Each pixel is stored as an unsigned 32-bit integer: alpha<<24 | red<<16 | green<<8 | blue
		m_playBuffer.width = bufferWidth;
		m_playBuffer.height = bufferHeight;
		m_playBuffer.pPixels = new Pixel[static_cast<size_t>( bufferWidth ) * bufferHeight];
		m_playBuffer.preMultiplied = false;
		PLAY_ASSERT( m_playBuffer.pPixels );

		memset( m_playBuffer.pPixels, 0, sizeof( uint32_t ) * bufferWidth * bufferHeight );

		// Make the display buffer the render target for the blitter
		Render::SetRenderTarget( &m_playBuffer );

		// Iterate through the directory
		PLAY_ASSERT_MSG( std::filesystem::exists( path ), "PlayBuffer: Drectory provided does not exist." );

		for( const auto& p : std::filesystem::directory_iterator( path ) )
		{
			// Switch everything to uppercase to avoid need to check case each time
			std::string filename = p.path().string();
			for( char& c : filename ) c = static_cast<char>( toupper( c ) );

			// Only attempt to load PNG files
			if( filename.find( ".PNG" ) != std::string::npos )
			{
				std::ifstream png_infile;
				png_infile.open( filename, std::ios::binary ); // Don't do this as part of the constructor or we lose 16 bytes!

				// If the PNG was opened okay
				if( png_infile )
				{
					int spriteId = LoadSpriteSheet( p.path().parent_path().string() + "\\", p.path().stem().string() );

					// Now we check for .inf file for each sprite and load origins
					int originX = 0, originY = 0;

					std::string info_filename = filename.replace( filename.find( ".PNG" ), 4, ".INF" );

					if( std::filesystem::exists( info_filename ) )
					{
						std::ifstream info_infile;
						info_infile.open( info_filename, std::ios::in );

						PLAY_ASSERT_MSG( info_infile.is_open(), std::string( "Unable to load existing .inf file: " + info_filename ).c_str() );
						if( info_infile.is_open() )
						{
							std::string type;
							info_infile >> type;
							info_infile >> originX;
							info_infile >> originY;
						}
						info_infile.close();
					}
					SetSpriteOrigin( spriteId, { originX, originY }, false );
				}
				png_infile.close();
			}
		}
		return true;
	}

	bool DestroyManager()
	{
		ASSERT_GRAPHICS;

		for( Sprite& s : m_vSpriteData )
		{
			if( s.canvasBuffer.pPixels )
				delete[] s.canvasBuffer.pPixels;

			if( s.preMultAlpha.pPixels )
				delete[] s.preMultAlpha.pPixels;
		}

		for( PixelData& pBgBuffer : m_vBackgroundData )
			delete[] pBgBuffer.pPixels;

		if( m_pDebugFontBuffer )
			delete[] m_pDebugFontBuffer;

		delete[] m_playBuffer.pPixels;

		m_bCreated = false;
		return true;
	}

	//********************************************************************************************************************************
	// Loading functions
	//********************************************************************************************************************************

	int LoadSpriteSheet( const std::string& path, const std::string& filename )
	{
		ASSERT_GRAPHICS;

		PixelData canvasBuffer;
		std::string spriteName = filename;
		bool isSpriteSheet = false;
		int hCount = 1;
		int vCount = 1;

		// Switch everything to uppercase to avoid need to check case each time
		for( char& c : spriteName ) c = static_cast<char>( toupper( c ) );

		// Look for the final number in the filename to pull out the number of frames across the width
		size_t frameWidthEnd = spriteName.find_last_of( "0123456789" );
		size_t frameWidthStart = spriteName.find_last_not_of( "0123456789" );

		// Make sure we are a sprite sheet (valid forms are sprite_w and sprite_wXh)
		if (spriteName[frameWidthStart] == 'X' && frameWidthStart != 0)
		{
			size_t frameWidthDelimPos = spriteName.find_last_not_of("0123456789", frameWidthStart - 1);
			if (spriteName[frameWidthDelimPos] == '_')
				isSpriteSheet = true;
		}
		else if (spriteName[frameWidthStart] == '_')
		{
			isSpriteSheet = true;
		}

		if( frameWidthEnd == spriteName.length() - 1 && isSpriteSheet)
		{
			// Grab the number of frames
			std::string widthString = spriteName.substr( frameWidthStart + 1, frameWidthEnd - frameWidthStart );

			// Make sure the number is valid 
			size_t num = widthString.find_first_of( "0123456789" );
			PLAY_ASSERT_MSG( num == 0, std::string( "Incorrectly named sprite: " + filename ).c_str() );

			hCount = stoi( widthString );

			if( spriteName[frameWidthStart] == 'X' )
			{
				//Two dimensional sprite sheet so the width was actually the height: copy it over and work out the real width
				vCount = hCount;

				// Cut off the last number we just found plus an "x", then check for another number indicating a frame height (optional)
				std::string truncated = spriteName.substr( 0, frameWidthStart );
				frameWidthEnd = truncated.find_last_of( "0123456789" );
				frameWidthStart = truncated.find_last_not_of( "0123456789" );

				if( frameWidthEnd == truncated.length() - 1 && frameWidthStart != std::string::npos )
				{
					// Grab the number of images in the height
					widthString = truncated.substr( frameWidthStart + 1, frameWidthEnd - frameWidthStart );

					// Make sure the number is valid
					num = widthString.find_first_of( "0123456789" );
					PLAY_ASSERT_MSG( num == 0, std::string( "Incorrectly named sprite: " + filename ).c_str() );

					hCount = stoi( widthString );
				}
				else
				{
					PLAY_ASSERT_MSG( false, std::string( "Incorrectly named sprite: " + filename ).c_str() );
				}
			}
			else
			{
				vCount = 1;
			}
		}
		std::string fileAndPath( path + spriteName + ".PNG" );
		LoadPNGImage( fileAndPath, canvasBuffer ); // Allocates memory as we don't know the size
	
		return AddSprite( filename, canvasBuffer, hCount, vCount );
	}

	int AddSprite( const std::string& name, PixelData& pixelData, int hCount, int vCount )
	{
		ASSERT_GRAPHICS;

		// Switch everything to uppercase to avoid need to check case each time
		std::string spriteName = name;
		for( char& c : spriteName ) c = static_cast<char>( toupper( c ) );

		Sprite s;
		s.id = m_nTotalSprites++;
		s.name = spriteName;
		s.originX = s.originY = 0;
		s.hCount = hCount;
		s.vCount = vCount;
		s.canvasBuffer = pixelData; // copy including pointer to pixel data

		s.totalCount = s.hCount * s.vCount;
		s.width = s.canvasBuffer.width / s.hCount;
		s.height = s.canvasBuffer.height / s.vCount;

		// Create a separate buffer with the pre-multiplyied alpha
		s.preMultAlpha.pPixels = new Pixel[static_cast<size_t>( s.canvasBuffer.width ) * s.canvasBuffer.height];
		s.preMultAlpha.width = s.canvasBuffer.width;
		s.preMultAlpha.height = s.canvasBuffer.height;
		memset( s.preMultAlpha.pPixels, 0, sizeof( uint32_t ) * s.canvasBuffer.width * s.canvasBuffer.height );
		PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, 0x00FFFFFF );
		s.canvasBuffer.preMultiplied = true;

		// Add the sprite to our vector
		m_vSpriteData.push_back( s );
		return s.id;
	}

	int UpdateSprite( const std::string& name, PixelData& pixelData, int hCount, int vCount )
	{
		ASSERT_GRAPHICS; 

		// Switch everything to uppercase to avoid need to check case each time
		std::string spriteName = name;
		for( char& c : spriteName ) c = static_cast<char>( toupper( c ) );

		for( Sprite& s : m_vSpriteData )
		{
			if( s.name.find( spriteName ) != std::string::npos )
			{
				// delete the old premultiplied buffer
				delete s.preMultAlpha.pPixels;

				s.hCount = hCount;
				s.vCount = vCount;
				s.canvasBuffer = pixelData; // copy including pointer to pixel data

				s.totalCount = s.hCount * s.vCount;
				s.width = s.canvasBuffer.width / s.hCount;
				s.height = s.canvasBuffer.height / s.vCount;

				// Create a new buffer with the pre-multiplyied alpha
				s.preMultAlpha.pPixels = new Pixel[static_cast<size_t>( s.canvasBuffer.width ) * s.canvasBuffer.height];
				s.preMultAlpha.width = s.canvasBuffer.width;
				s.preMultAlpha.height = s.canvasBuffer.height;
				memset( s.preMultAlpha.pPixels, 0, sizeof( uint32_t ) * s.canvasBuffer.width * s.canvasBuffer.height );
				PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, 0x00FFFFFF );
				s.canvasBuffer.preMultiplied = true;

				return s.id;
			}
		}
		return -1;
	}

	int UpdateSprite( const std::string& name )
	{
		ASSERT_GRAPHICS;

		// Switch everything to uppercase to avoid need to check case each time
		std::string spriteName = name;
		for( char& c : spriteName ) c = static_cast<char>(toupper( c ));

		for( Sprite& s : m_vSpriteData )
		{
			if( s.name.find( spriteName ) != std::string::npos )
			{
				memset( s.preMultAlpha.pPixels, 0, sizeof( uint32_t ) * s.canvasBuffer.width * s.canvasBuffer.height );
				PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, 0x00FFFFFF );
				s.canvasBuffer.preMultiplied = true;

				return s.id;
			}
		}
		return -1;
	}


	int LoadBackground( const char* fileAndPath )
	{
		ASSERT_GRAPHICS;

		// The background image may not be the right size for the background so we make sure the buffer is 
		PixelData backgroundImage;
		Pixel* pSrc, * pDest;

		Pixel* correctSizeBuffer = new Pixel[static_cast<size_t>( m_playBuffer.width ) * m_playBuffer.height];
		PLAY_ASSERT( correctSizeBuffer );

		std::string pngFile( fileAndPath );
		PLAY_ASSERT_MSG( std::filesystem::exists( fileAndPath ), "The background png does not exist at the given location." );
		LoadPNGImage( pngFile, backgroundImage ); // Allocates memory in function as we don't know the size

		pSrc = backgroundImage.pPixels;
		pDest = correctSizeBuffer;

		//Copy the image to our background buffer clipping where necessary
		for( int h = 0; h < std::min( backgroundImage.height, m_playBuffer.height ); h++ )
		{
			for( int w = 0; w < std::min( backgroundImage.width, m_playBuffer.width ); w++ )
				*pDest++ = *pSrc++;

			// Skip pixels if we're clipping
			pDest += std::max( m_playBuffer.width - backgroundImage.width, 0 );
			pSrc += std::max( backgroundImage.width - m_playBuffer.width, 0 );
		}

		// Free up the loading buffer
		delete backgroundImage.pPixels;
		backgroundImage.pPixels = correctSizeBuffer;

		m_vBackgroundData.push_back( backgroundImage );

		return static_cast<int>( m_vBackgroundData.size() ) - 1;
	}

	//********************************************************************************************************************************
	// Sprite Getters and Setters
	//********************************************************************************************************************************
	int GetSpriteId( const char* name ) 
	{
		ASSERT_GRAPHICS;

		std::string tofind( name );
		for( char& c : tofind ) c = static_cast<char>( toupper( c ) );

		for( const Sprite& s : m_vSpriteData )
		{
			if( s.name.find( tofind ) != std::string::npos )
				return s.id;
		}
		PLAY_ASSERT_MSG( false, "The sprite name is invalid!" );
		return -1;
	}

	const std::string& GetSpriteName( int spriteId )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get name of invalid sprite id" );
		return m_vSpriteData[spriteId].name;
	}

	Vector2f GetSpriteSize( int spriteId )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get width of invalid sprite id" );
		return { m_vSpriteData[spriteId].width, m_vSpriteData[spriteId].height };
	}

	int GetSpriteFrames( int spriteId )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get frames of invalid sprite id" );
		return m_vSpriteData[spriteId].totalCount;
	}

	Vector2f GetSpriteOrigin( int spriteId ) 
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to get origin with invalid sprite id" );
		return { m_vSpriteData[spriteId].originX, m_vSpriteData[spriteId].originY };
	}

	void SetSpriteOrigin( int spriteId, Vector2f newOrigin, bool relative )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to set origin with invalid sprite id" );
		if( relative )
		{
			m_vSpriteData[spriteId].originX += static_cast<int>( newOrigin.x );
			m_vSpriteData[spriteId].originY += static_cast<int>( newOrigin.y );
		}
		else
		{
			m_vSpriteData[spriteId].originX = static_cast<int>( newOrigin.x );
			m_vSpriteData[spriteId].originY = static_cast<int>( newOrigin.y );
		}
	}

	void CentreSpriteOrigin( int spriteId )
	{
		ASSERT_GRAPHICS;
		SetSpriteOrigin( spriteId, GetSpriteSize( spriteId ) / 2, false );
	}

	void CentreAllSpriteOrigins( void )
	{
		ASSERT_GRAPHICS;
		for( Sprite& s : m_vSpriteData )
			CentreSpriteOrigin( s.id );
	}

	void FlipSpriteOriginVertically( int spriteId )
	{
		ASSERT_GRAPHICS;
		Vector2f origin = GetSpriteOrigin( spriteId );
		origin.y = GetSpriteSize( spriteId ).y - origin.y;
		SetSpriteOrigin( spriteId, origin, false );
	}

	void FlipAllSpriteOriginsVertically( void )
	{
		ASSERT_GRAPHICS;
		for( Sprite& s : m_vSpriteData )
			FlipSpriteOriginVertically( s.id );
	}

	void SetSpriteOrigins( const char* rootName, Vector2f newOrigin, bool relative )
	{
		ASSERT_GRAPHICS;
		std::string tofind( rootName );
		for( char& c : tofind ) c = static_cast<char>( toupper( c ) );

		for( Sprite& s : m_vSpriteData )
		{
			if( s.name.find( tofind ) != std::string::npos )
			{
				if( relative )
				{
					s.originX += static_cast<int>( newOrigin.x );
					s.originY += static_cast<int>( newOrigin.y );
				}
				else
				{
					s.originX = static_cast<int>( newOrigin.x );
					s.originY = static_cast<int>( newOrigin.y );
				}
			}
		}
	}

	int GetTotalLoadedSprites() 
	{ 
		ASSERT_GRAPHICS;
		return m_nTotalSprites; 
	}
	
	const PixelData* GetSpritePixelData(int spriteId) 
	{ 
		ASSERT_GRAPHICS;
		return &m_vSpriteData[spriteId].canvasBuffer; 
	}

	//********************************************************************************************************************************
	// Drawing functions
	//********************************************************************************************************************************

	void DrawTransparent( int spriteId, Point2f pos, int frameIndex, BlendColour globalMultiply)
	{
		ASSERT_GRAPHICS;
		const Sprite& spr = m_vSpriteData[spriteId];
		int destx = static_cast<int>( pos.x + 0.5f ) - spr.originX;
		int desty = static_cast<int>( pos.y + 0.5f ) + (spr.height - spr.originY);
		frameIndex = frameIndex % spr.totalCount;
		int frameX = frameIndex % spr.hCount;
		int frameY = frameIndex / spr.hCount;
		int pixelX = frameX * spr.width;
		int pixelY = frameY * spr.height;
		int frameOffset = pixelX + ( spr.canvasBuffer.width * pixelY );

		switch (blendMode)
		{
			case BLEND_NORMAL:
				Render::BlitPixels<Render::AlphaBlendPolicy>(spr.preMultAlpha, frameOffset, destx, desty, spr.width, spr.height, globalMultiply);
				break;
			case BLEND_ADD:
				Render::BlitPixels<Render::AdditiveBlendPolicy>(spr.preMultAlpha, frameOffset, destx, desty, spr.width, spr.height, globalMultiply);
				break;
			case BLEND_MULTIPLY:
				Render::BlitPixels<Render::MultiplyBlendPolicy>(spr.canvasBuffer, frameOffset, destx, desty, spr.width, spr.height, globalMultiply);
				break;
			default:
				PLAY_ASSERT_MSG(false, "Unsupported blend mode in DrawTransparent")
					break;
		}

	};

	void DrawRotated( int spriteId, Point2f pos, int frameIndex, float angle, float scale, BlendColour globalMultiply )
	{
		ASSERT_GRAPHICS;
		Matrix2D trans = MatrixScale( scale, scale ) * MatrixRotation( angle )  * MatrixTranslation( pos.x, pos.y );
		DrawTransformed( spriteId, trans, frameIndex, globalMultiply);
	}

	void DrawTransformed( int spriteId, const Matrix2D& trans, int frameIndex, BlendColour globalMultiply)
	{
		ASSERT_GRAPHICS;
		const Sprite& spr = m_vSpriteData[spriteId];
		frameIndex = frameIndex % spr.totalCount;
		int frameX = frameIndex % spr.hCount;
		int frameY = frameIndex / spr.hCount;
		int pixelX = frameX * spr.width;
		int pixelY = frameY * spr.height;
		int frameOffset = pixelX + ( spr.canvasBuffer.width * pixelY );

		Vector2f origin = { spr.originX, spr.height - spr.originY };

		switch (blendMode)
		{
		case BLEND_NORMAL:
			Render::TransformPixels<Render::AlphaBlendPolicy>(spr.preMultAlpha, frameOffset, spr.width, spr.height, origin, trans, globalMultiply);
			break;
		case BLEND_ADD:
			Render::TransformPixels<Render::AdditiveBlendPolicy>(spr.preMultAlpha, frameOffset, spr.width, spr.height, origin, trans, globalMultiply);
			break;
		case BLEND_MULTIPLY:
			Render::TransformPixels<Render::MultiplyBlendPolicy>(spr.preMultAlpha, frameOffset, spr.width, spr.height, origin, trans, globalMultiply);
			break;
		default:
			PLAY_ASSERT_MSG(false, "Unsupported blend mode in DrawTransparent")
				break;
		}
	}

	void DrawBackground( int backgroundId )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( m_playBuffer.pPixels, "Trying to draw background without initialising display!" );
		PLAY_ASSERT_MSG( m_vBackgroundData.size() > static_cast<size_t>(backgroundId), "Background image out of range!" );
		Render::BlitBackground( m_vBackgroundData[backgroundId] );
	}

	void ColourSprite( int spriteId, int r, int g, int b )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( spriteId >= 0 && spriteId < m_nTotalSprites, "Trying to colour invalid sprite id" );

		Sprite& s = m_vSpriteData[spriteId];
		uint32_t col = ( ( r & 0xFF ) << 16 ) | ( ( g & 0xFF ) << 8 ) | ( b & 0xFF );

		PreMultiplyAlpha( s.canvasBuffer.pPixels, s.preMultAlpha.pPixels, s.canvasBuffer.width, s.canvasBuffer.height, s.width, 1.0f, col );
		s.canvasBuffer.preMultiplied = true;
	}

	int DrawString( int fontId, Point2f pos, std::string text )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );

		int width = 0;

		for( char c : text )
		{
			Draw( fontId, { pos.x + width, pos.y }, c - 32 );
			width += GetFontCharWidth( fontId, c );
		}
		return width;
	}

	int DrawStringCentred( int fontId, Point2f pos, std::string text )
	{
		ASSERT_GRAPHICS;
		int totalWidth = 0;

		for( char c : text )
			totalWidth += GetFontCharWidth( fontId, c );

		pos.x -= totalWidth / 2;

		DrawString( fontId, pos, text );
		return totalWidth;
	}

	int DrawChar( int fontId, Point2f pos, char c )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
		Draw( fontId, { pos.x, pos.y }, c - 32 );
		return GetFontCharWidth( fontId, c );
	}

	int DrawCharRotated( int fontId, Point2f pos, float angle, float scale, char c )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
		DrawRotated( fontId, { pos.x, pos.y }, c - 32, angle, scale );
		return GetFontCharWidth( fontId, c );
	}

	int GetFontCharWidth( int fontId, char c )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( fontId >= 0 && fontId < m_nTotalSprites, "Trying to use invalid sprite id for font" );
		int glyphWidthDataOffset = m_vSpriteData[ fontId ].canvasBuffer.width * (m_vSpriteData[ fontId ].canvasBuffer.height - 1);
		return (m_vSpriteData[fontId].canvasBuffer.pPixels + glyphWidthDataOffset + ( c - 32 ))->b; // character width hidden in pixel data
	}

	//********************************************************************************************************************************
	// Function:	SpriteCollide: function that checks by pixel if two sprites collide
	// Parameters:	s1Xpos, s1Ypos, s2Xpos, s2Ypos = the origin of rotation for both sprites.
	//				s1Id, s2Id = the ids of both sprites
	//				s1angle, s2angle = the angle of rotation for both sprites, clockwise. 0 = unrotated.
	//				s1Pixelcoll, s2Pixelcoll = the top left and bottom right co-ordinates defining the collision rectangle of both sprites
	//				
	// Returns: true if a single pixel or more overlap between the two sprites and false if not.
	// Notes:	rounding errors may cause it not to be pixel perfect.	
	//********************************************************************************************************************************
	int SpriteCollide( int spriteIdA, int frameIndexA, Matrix2D& transA, int spriteIdB, int frameIndexB, Matrix2D& transB )
	{
		ASSERT_GRAPHICS;
		int overlapping_pixels = 0;

		// This algorithm transforms Sprite A's pixels into the co-ordinate space of Sprite B and iterates through all Sprite A's pixels checking for collisions
		// This only works reliably if the pixels of Sprite A are smaller or the same size as those of Sprite B
		
		// If Matrix A's pixels are larger than Matrix A's in both dimensions then swap the sprites around
		if( transA.row[0].Length() > transB.row[ 0 ].Length() && transA.row[ 1 ].Length() > transB.row[ 1 ].Length() )
			return SpriteCollide( spriteIdB, frameIndexB, transB, spriteIdA, frameIndexA, transA );

		PLAY_ASSERT_MSG( transA.row[ 0 ].Length() <= transB.row[ 0 ].Length() && transA.row[ 1 ].Length() <= transB.row[ 1 ].Length(), "Sprite Collide algorithm only works with uniform scaling" );

		const Sprite& spr_a = m_vSpriteData[ spriteIdA ];
		const Sprite& spr_b = m_vSpriteData[ spriteIdB ];

		Matrix2D a_trans_right{ transA };
		a_trans_right.row[ 0 ] = { transA.row[ 0 ].x, transA.row[ 1 ].x, 0.0f };
		a_trans_right.row[ 1 ] = { transA.row[ 0 ].y, transA.row[ 1 ].y, 0.0f };
		a_trans_right.row[ 2 ] = { transA.row[ 2 ].x, -transA.row[ 2 ].y, 1.0f };
		
		Matrix2D b_trans_right{ transB };
		b_trans_right.row[ 0 ] = { transB.row[ 0 ].x, transB.row[ 1 ].x, 0.0f };
		b_trans_right.row[ 1 ] = { transB.row[ 0 ].y, transB.row[ 1 ].y, 0.0f };
		b_trans_right.row[ 2 ] = { transB.row[ 2 ].x, -transB.row[ 2 ].y, 1.0f };

		frameIndexA = frameIndexA % spr_a.totalCount;
		int a_frame_x = frameIndexA % spr_a.hCount;
		int a_frame_y = frameIndexA / spr_a.hCount;
		int a_pixel_x = a_frame_x * spr_a.width;
		int a_pixel_y = a_frame_y * spr_a.height;
		int a_frame_offset = a_pixel_x + (spr_a.canvasBuffer.width * a_pixel_y);

		frameIndexB = frameIndexB % spr_b.totalCount;
		int b_frame_x = frameIndexB % spr_b.hCount;
		int b_frame_y = frameIndexB / spr_b.hCount;
		int b_pixel_x = b_frame_x * spr_b.width;
		int b_pixel_y = b_frame_y * spr_b.height;
		int b_frame_offset = b_pixel_x + (spr_b.canvasBuffer.width * b_pixel_y);

		Vector2f a_origin = { spr_a.originX, spr_a.height - spr_a.originY };
		Vector2f b_origin = { spr_b.originX, spr_b.height - spr_b.originY };

		Matrix2D b_inv_trans = Play::MatrixTranslation( -b_origin.x, -b_origin.y ) * b_trans_right;
		b_inv_trans.Inverse();
		Matrix2D a2b_trans = Play::MatrixTranslation( -a_origin.x, -a_origin.y ) * a_trans_right * b_inv_trans;

		float b_posx = a2b_trans.row[2].x;
		float b_posy = a2b_trans.row[2].y;

		float b_xincx = a2b_trans.row[ 0 ].x;
		float b_xincy = a2b_trans.row[ 0 ].y;
		float b_yincx = a2b_trans.row[ 1 ].x;
		float b_yincy = a2b_trans.row[ 1 ].y;
		float b_xresetx = b_xincx * spr_a.width; // This needs to be sprite a's width as that's the space we are iterating through
		float b_xresety = b_xincy * spr_a.width; // This needs to be sprite a's width as that's the space we are iterating through

		uint32_t* a_pixel = (uint32_t*)spr_a.preMultAlpha.pPixels + a_frame_offset;
		uint32_t* a_pixel_end = a_pixel + (spr_a.height * spr_a.canvasBuffer.width);

		// Iterate sequentially through pixels within the render target buffer
		while( a_pixel < a_pixel_end )
		{
			// For each row of pixels in turn
			uint32_t* dst_row_end = a_pixel + spr_a.width;
			while( a_pixel < dst_row_end )
			{
				if( *a_pixel < 0xFF000000 )
				{
					// The origin of a pixel is in its centre
					int roundX = static_cast<int>(b_posx + 0.5f);
					int roundY = static_cast<int>(b_posy + 0.5f);

					// Clip within the sprite boundaries
					if( roundX >= 0 && roundY >= 0 && roundX < spr_b.width && roundY < spr_b.height )
					{
						int b_pixel_index = roundX + (roundY * spr_b.canvasBuffer.width);
						uint32_t* b_pixel = ((uint32_t*)spr_b.canvasBuffer.pPixels + b_pixel_index + b_frame_offset);
						if( *b_pixel & 0xFF000000 )
							overlapping_pixels++; // Could also overwite to visualise: *b_pixel = 0xFFFFFFFF, but need to call UpdateSprite afterwards.	
					}
				}

				// Move one horizontal pixel in render target, which corresponds to the x axis of the inverse matrix in sprite space
				a_pixel++;
				b_posx += b_xincx;
				b_posy += b_xincy;
			}

			// Move render target pointer to the start of the next row
			a_pixel += spr_a.canvasBuffer.width - spr_a.width;

			// Move sprite buffer pointer back to the start of the current row
			b_posx -= b_xresetx;
			b_posy -= b_xresety;

			// One vertical pixel in the render target corresponds to the y axis of the inverse matrix in sprite space
			b_posx += b_yincx;
			b_posy += b_yincy;
		}
		return overlapping_pixels;
	}

	//********************************************************************************************************************************
	// Function:	PreMultiplyAlpha - calculates the (src*srcAlpha) alpha blending calculation in advance as it doesn't change
	// Parameters:	s = the sprite to pre-calculate data for
	// Notes:		Also inverts the alpha ready for the (dest*(1-srcAlpha)) calculation and stores information in the new
	//				buffer which provides the number of fully-transparent pixels in a row (so they can be skipped)
	//********************************************************************************************************************************
	void PreMultiplyAlpha( Pixel* source, Pixel* dest, int width, int height, int maxSkipWidth, float alphaMultiply = 1.0f, Pixel colourMultiply = 0x00FFFFFF )
	{
		ASSERT_GRAPHICS;

		Pixel* pSourcePixels = source;
		Pixel* pDestPixels = dest;

		// Iterate through all the pixels in the entire canvas
		for( int bh = 0; bh < height; bh++ )
		{
			for( int bw = 0; bw < width; bw++ )
			{
				Pixel src = *pSourcePixels;

				// Separate the channels and calculate src*srcAlpha
				int srcAlpha = static_cast<int>( ( src.bits >> 24 ) * alphaMultiply );

				int destRed = ( srcAlpha * ( ( src.bits >> 16 ) & 0xFF ) ) >> 8;
				int destGreen = ( srcAlpha * ( ( src.bits >> 8 ) & 0xFF ) ) >> 8;
				int destBlue = ( srcAlpha * ( src.bits & 0xFF ) ) >> 8;

				destRed = ( destRed * ( ( colourMultiply.bits >> 16 ) & 0xFF ) ) >> 8;
				destGreen = ( destGreen * ( ( colourMultiply.bits >> 8 ) & 0xFF ) ) >> 8;
				destBlue = ( destBlue * ( colourMultiply.bits & 0xFF ) ) >> 8;

				srcAlpha = 0xFF - srcAlpha; // invert the alpha ready to multiply with the destination pixels
				*pDestPixels = ( srcAlpha << 24 ) | ( destRed << 16 ) | ( destGreen << 8 ) | destBlue;

				if( srcAlpha == 0xFF ) // Completely transparent pixel
				{
					int repeats = 0;
					// We can only skip to the end of the row because the sprite frames are arranged on a continuous canvas
					int maxSkip = maxSkipWidth - ( bw % maxSkipWidth );

					for( int zw = 1; zw < maxSkip; zw++ )
					{
						if( ( pSourcePixels + zw )->bits >> 24 == 0x00 ) // Another transparent pixel
							repeats++;
						else
							break;
					}
					*pDestPixels = 0xFF000000 | repeats; // Doesn't matter what the colour was so we use it to store the skip value
				}
				pDestPixels++;
				pSourcePixels++;
			}
		}
	}

	//********************************************************************************************************************************
	// Basic drawing functions
	//********************************************************************************************************************************
	void DrawPixel( Point2f pos, Pixel srcPix )
	{
		ASSERT_GRAPHICS;

		pos.y = Window::GetHeight() - pos.y; //// Flip the y-coordinate to be consistant with a Cartesian co-ordinate system

		// Convert floating point co-ordinates to pixels
		switch (blendMode)
		{
		case BLEND_NORMAL:
			// Convert floating point co-ordinates to pixels
			Render::DrawPixelPreMult<Render::AlphaBlendPolicy>(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f), srcPix);
			break;
		case BLEND_ADD:
			// Convert floating point co-ordinates to pixels
			Render::DrawPixelPreMult<Render::AdditiveBlendPolicy>(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f), srcPix);
			break;
		case BLEND_MULTIPLY:
			// Convert floating point co-ordinates to pixels
			Render::DrawPixel<Render::MultiplyBlendPolicy>(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f), srcPix);
			break;
		default:
			PLAY_ASSERT_MSG(false, "Unsupported blend mode in PlayGraphics::DrawPixel")
				break;
		}
	}

	void DrawLine( Point2f startPos, Point2f endPos, Pixel pix )
	{
		ASSERT_GRAPHICS;
		// Convert floating point co-ordinates to pixels
		int x1 = static_cast<int>( startPos.x + 0.5f );
		int y1 = static_cast<int>( startPos.y + 0.5f );
		int x2 = static_cast<int>( endPos.x + 0.5f );
		int y2 = static_cast<int>( endPos.y + 0.5f );

		Render::DrawLine( x1, y1, x2, y2, pix );
	}

	void DrawRect( Point2f bottomLeft, Point2f topRight, Pixel pix, bool fill /*= false */ )
	{
		ASSERT_GRAPHICS;
		// Convert floating point co-ordinates to pixels
		int x1 = static_cast<int>( bottomLeft.x + 0.5f );
		int x2 = static_cast<int>( topRight.x + 0.5f );
		int y1 = static_cast<int>( bottomLeft.y + 0.5f );
		int y2 = static_cast<int>( topRight.y + 0.5f );

		if( fill )
		{
			for( int x = x1; x < x2; x++ )
			{
				for( int y = y1; y < y2; y++ )
					DrawPixel({ x, y }, pix);
			}
		}
		else
		{
			Render::DrawLine( x1, y1, x2, y1, pix );
			Render::DrawLine( x2, y1, x2, y2, pix );
			Render::DrawLine( x2, y2, x1, y2, pix );
			Render::DrawLine( x1, y2, x1, y1, pix );
		}
	}

	// Private function called by DrawCircle
	void DrawCircleOctants( int posX, int posY, int offX, int offY, Pixel pix )
	{
		ASSERT_GRAPHICS;
		DrawPixel( { posX + offX , posY + offY }, pix );
		DrawPixel( { posX - offX , posY + offY }, pix );
		DrawPixel( { posX + offX , posY - offY }, pix );
		DrawPixel( { posX - offX , posY - offY }, pix );
		DrawPixel( { posX - offY , posY + offX }, pix );
		DrawPixel( { posX + offY , posY - offX }, pix );
		DrawPixel( { posX - offY , posY - offX }, pix );
		DrawPixel( { posX + offY , posY + offX }, pix );
	}

	void DrawCircle( Point2f pos, int radius, Pixel pix )
	{
		ASSERT_GRAPHICS;
		// Convert floating point co-ordinates to pixels
		int x = static_cast<int>( pos.x + 0.5f );
		int y = static_cast<int>( pos.y + 0.5f );

		int dx = 0;
		int dy = radius;

		int d = 3 - 2 * radius;
		DrawCircleOctants( x, y, dx, dy, pix );

		while( dy >= dx )
		{
			dx++;
			if( d > 0 )
			{
				dy--;
				d = static_cast<int>( d + 4 * ( dx - dy ) + 10 );
			}
			else
			{
				d = static_cast<int>( d + 4 * dx + 6 );
			}
			DrawCircleOctants( x, y, dx, dy, pix );
		}
	};

	void DrawPixelData( PixelData* pixelData, Point2f pos, float alpha )
	{
		ASSERT_GRAPHICS;
		if( !pixelData->preMultiplied )
		{
			PreMultiplyAlpha( pixelData->pPixels, pixelData->pPixels, pixelData->width, pixelData->height, pixelData->width );
			pixelData->preMultiplied = true;
		}
		Render::BlitPixels<Render::AlphaBlendPolicy>(*pixelData, 0, static_cast<int>(pos.x), static_cast<int>(pos.y), pixelData->width, pixelData->height, { alpha, 1.0f, 1.0f, 1.0f });
	}

	//********************************************************************************************************************************
	// Debug font functions
	//********************************************************************************************************************************

	// Settings for the embedded debug font
	#define FONT_IMAGE_WIDTH	96
	#define FONT_IMAGE_HEIGHT	36
	#define FONT_CHAR_WIDTH		6
	#define FONT_CHAR_HEIGHT	12

	// A 96x36 font image @ 1bpp containing basic ASCII characters from 0x30 (0) to 0x5F (underscore) at 6x12 pixels each
	uint32_t debugFontData[] =
	{
		0x87304178, 0x0800861F, 0xFFFFFFE1, 0x7BBFBE79, 0xF7FE79EF, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EC, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EC, 0xFFFFFFFE, 0x7BBFBE79, 0xF7FE79EF, 0xFFEE3DFE, 0x6BB86080,	0x1078860F, 0xE3DFFEFE, 
		0x5BB7FEFB, 0xE7BE7BEF, 0xFFBFFF41, 0x7BB7FEFB, 0xE7BE7BEC, 0xFFDFFEDF, 0x7BB7FEFB, 0xE7BE7BEC, 0xFFEE3DDF, 0x7BB7FEFB, 0xE7BE7BEF, 0xFFFFFFFF, 0x7BB7BEF9, 0xE7BE7BEF,	0xFFFFFFDF, 0x87B001F8, 0x187E87EF, 0xFFFFFFDF, 
		0x86106104, 0x00007A30, 0x1E7C5061, 0x79E79E79, 0xF7DF7B7F, 0x5E7DA79E, 0x79E79F79, 0xF7DF7B7F, 0x5E7DA79E, 0x79E79F79, 0xF7DF7B7F, 0x5E7DA79E,	0x59E79F79, 0xF7DF7B7F, 0x597DA79E, 0x58005F78, 0x30D0037F, 0x477DA79E, 
		0x59E79F79, 0xF7DE7B7F, 0x597DA79E, 0x59E79F79, 0xF7DE7B7F, 0x5E7DA79E, 0x41E79F79, 0xF7DE7B7F, 0x5E7DA79E, 0x7DE79F79,	0xF7DE7B7F, 0x5E7DA79E, 0x7DE79E79, 0xF7DE7B7F, 0x5E7DA79E, 0x81E06104, 0x07C17A38, 0xDE01A7A1, 
		0x06106101, 0xE79E79E0, 0x337F3FFF, 0x79E79EED, 0xE79E79EF, 0xB77FBFFF, 0x79E79FED, 0xE79E79EF,	0xB7BFBFFF, 0x79E79FED, 0xE79A79EF, 0xB7BFBFFF, 0x79E75FED, 0xE79AB5EF, 0x77DFBFFF, 0x05E0E1ED, 0xE79ACC0E, 0xF7DFBFFF, 
		0x7DE77EED, 0xE79AB7ED, 0xF7EFBFFF, 0x7DE7BEED, 0xE79A7BEB, 0xF7EFBFFF,	0x7DE7BEED, 0xE79A7BE7, 0xF7F7BFFF, 0x7DA7BEED, 0xE79A7BE7, 0xF7F7BCFF, 0x7DD79EED, 0xEB5A7BE7, 0xF7FBBCFF, 0x7C27A1EE, 0x1CE57810, 0x33FB3EC0
	};

	void DecompressDubugFont( void )
	{
		ASSERT_GRAPHICS;

		m_pDebugFontBuffer = new uint8_t[FONT_IMAGE_WIDTH * FONT_IMAGE_HEIGHT];

		for( int y = 0; y < FONT_IMAGE_HEIGHT; y++ )
		{
			for( int x = 0; x < FONT_IMAGE_WIDTH; x++ )
			{
				int bufferIndex = ( y * FONT_IMAGE_WIDTH ) + x;
				int dataIndex = bufferIndex / 32;
				int dataShift = 31 - ( bufferIndex % 32 );

				// Convert 1bpp to an 8-bit array for easy data access
				if( ( debugFontData[dataIndex] >> dataShift ) & 0x01 )
					m_pDebugFontBuffer[( y * FONT_IMAGE_WIDTH ) + x] = 0; // not ARGB just 0 (no pixel)
				else
					m_pDebugFontBuffer[( y * FONT_IMAGE_WIDTH ) + x] = 1; // not ARGB just 1 (a pixel)
			}
		}
	}

	int DrawDebugCharacter( Point2f pos, char c, Pixel pix )
	{
		ASSERT_GRAPHICS;
		// Limited character set in the font (0x30-0x5F) so includes translation of useful chars outside that range
		switch( c )
		{
			case 0x2C: c = 0x5E; break; // Map full stop to comma (nearly the same!)
			case 0x2D: c = 0x3B; break; // Map minus to semi-colon (rarely used)
			case 0x28: c = 0x5B; break; // Map brackets to square brackets
			case 0x29: c = 0x5D; break; // Map brackets to square brackets
			case 0x2E: c = 0x5E; break; // Map full stop to carot (never used)
		}

		if( c < 0x30 || c > 0x5F )
			return FONT_CHAR_WIDTH;

		int sourceX = ( ( c - 0x30 ) % 16 ) * FONT_CHAR_WIDTH;
		int sourceY = ( ( c - 0x30 ) / 16 ) * FONT_CHAR_HEIGHT;

		// Loop over the bounding box of the glyph
		for( int x = 0; x < FONT_CHAR_WIDTH; x++ )
		{
			for( int y = 0; y < FONT_CHAR_HEIGHT; y++ )
			{
				if( m_pDebugFontBuffer[( ( sourceY + y ) * FONT_IMAGE_WIDTH ) + ( sourceX + x )] > 0 )
					DrawPixel( { pos.x + x, pos.y + (FONT_CHAR_HEIGHT - y - 1) }, pix ); 
			}
		}
		return FONT_CHAR_WIDTH;
	}

	int DrawDebugString( Point2f pos, const std::string& s, Pixel pix, bool centred )
	{
		ASSERT_GRAPHICS;

		if( m_pDebugFontBuffer == nullptr )
			DecompressDubugFont();

		if( centred )
			pos.x -= GetDebugStringWidth( s ) / 2;

		pos.y -= 6; // half the height of the debug font

		for( char c : s )
			pos.x += DrawDebugCharacter( pos, static_cast<char>( toupper( c ) ), pix ) + 1;

		// Return horizontal position at the end of the string so strings can be concatenated easily
		return static_cast<int>( pos.x );
	}

	int GetDebugStringWidth( const std::string& s )
	{
		ASSERT_GRAPHICS;
		return static_cast<int>( s.length() ) * ( FONT_CHAR_WIDTH + 1 );
	}

	//********************************************************************************************************************************
	// Miscellaneous functions
	//********************************************************************************************************************************
	PixelData* GetDrawingBuffer(void) 
	{ 
		ASSERT_GRAPHICS;
		return &m_playBuffer; 
	}

	LARGE_INTEGER EndTimingSegment()
	{
		ASSERT_GRAPHICS;

		int size = static_cast<int>( m_vTimings.size() );

		LARGE_INTEGER now, freq;
		QueryPerformanceCounter( &now );
		QueryPerformanceFrequency( &freq );

		if( size > 0 )
		{
			m_vTimings[size - 1].end = now.QuadPart;
			m_vTimings[size - 1].millisecs = static_cast<float>( m_vTimings[size - 1].end - m_vTimings[size - 1].begin );
			m_vTimings[size - 1].millisecs = static_cast<float>( ( m_vTimings[size - 1].millisecs * 1000.0f ) / freq.QuadPart );
		}
		return now;
	}

	int SetTimingBarColour( Pixel pix )
	{
		ASSERT_GRAPHICS;

		TimingSegment newData;
		newData.pix = pix;
		newData.begin = EndTimingSegment().QuadPart;

		m_vTimings.push_back( newData );

		return static_cast<int>( m_vTimings.size() );
	};

	void DrawTimingBar( Point2f pos, Point2f size )
	{
		ASSERT_GRAPHICS;

		EndTimingSegment();

		int startPixel{ 0 };
		int endPixel{ 0 };
		for( const TimingSegment& t : m_vPrevTimings )
		{
			endPixel += static_cast<int>( ( size.width * t.millisecs ) / 16.667f );
			DrawRect( { pos.x + startPixel, pos.y }, { pos.x + endPixel, pos.y + size.height }, t.pix, true );
			startPixel = endPixel;
		}

		DrawRect( { pos.x, pos.y }, { pos.x + size.width, pos.y + size.height }, PIX_BLACK, false );
		DrawRect( { pos.x - 1, pos.y - 1 }, { pos.x + size.width + 1 , pos.y + size.height + 1 }, PIX_WHITE, false );
	}

	float GetTimingSegmentDuration( int id )
	{
		ASSERT_GRAPHICS;
		PLAY_ASSERT_MSG( static_cast<size_t>(id) < m_vTimings.size(), "Invalid id for timing data." );
		return m_vTimings[id].millisecs;
	}

	void TimingBarBegin( Pixel pix )
	{
		ASSERT_GRAPHICS;
		EndTimingSegment();
		m_vPrevTimings = m_vTimings;
		m_vTimings.clear();
		SetTimingBarColour( pix );
	}
}
//********************************************************************************************************************************
// File:		PlayAudio.cpp
// Description:	Implementation of a very simple audio manager using XAudio2
// Platform:	Windows
// Notes:		Uses WAV format (uncompressed, so audio file sizes can be large)
//********************************************************************************************************************************


using namespace std;

#define ASSERT_AUDIO PLAY_ASSERT_MSG( Play::Audio::m_bCreated, "Audio Manager not initialised. Call Audio::CreateManager() before using the Play::Audio library functions.")

namespace Play::Audio
{
	// Flag to record whether the manager has been created
	bool m_bCreated = false;

	// XAudio2 objects
	IXAudio2* m_pXAudio2 = nullptr;
	IXAudio2MasteringVoice* m_pMasterVoice = nullptr;
	std::mutex m_voiceMutex; // XAudio2 is multi-threaded so we will need to take precautions!

	// Each WAV file in the audio directory is loaded into a SoundEffect structure
	struct SoundEffect
	{
		std::string fileAndPath;
		uint8_t* pFileBuffer{ nullptr }; // The whole file is loaded into here
		bool isXWMA = false;
		XAUDIO2_BUFFER xAudio2Buffer{ 0 }; // Pointer to the WAV data within the file buffer
		XAUDIO2_BUFFER_WMA xAudio2BufferWMA{ 0 }; // Pointer to XWMA data.
		WAVEFORMATEXTENSIBLE format{ 0 }; 
	};
	std::vector< SoundEffect > m_vSoundEffects; // Vector of all the loaded sound effects

	// A new audio voice is created each time you play a sound effect
	struct AudioVoice
	{
		int id{ 0 };
		IXAudio2SourceVoice* pSourceVoice{ nullptr };
		SoundEffect* pSoundEffect{ nullptr };

		AudioVoice()
		{
			static int uniqueId = 0;
			id = uniqueId++;
		}
	};

	// A map is used internally to store all the playing sound effects and their unique ids
	static std::map<int, AudioVoice&> m_audioVoiceMap;

	// Internal (private) functions
	bool LoadSoundEffect( std::string& filename, SoundEffect& sf );
	bool DestroyVoice( int voiceId );
	
	// An XAudio2 callback is required to clean up audio voices when they have finished playing
	class VoiceCallback : public IXAudio2VoiceCallback
	{
	public:
		void STDMETHODCALLTYPE OnStreamEnd() override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart( UINT32 ) override {}
		void STDMETHODCALLTYPE OnBufferStart( void* ) override {}
		void STDMETHODCALLTYPE OnLoopEnd( void* ) override {}
		void STDMETHODCALLTYPE OnVoiceError( void*, HRESULT ) override {}
		void STDMETHODCALLTYPE OnBufferEnd( void* pBufferContext ) override { DestroyVoice( *((int*)pBufferContext) ); }
	};

	//********************************************************************************************************************************
	// Create and Destroy functions
	//********************************************************************************************************************************
	bool CreateManager( const char* path )
	{
		PLAY_ASSERT_MSG( !m_bCreated, "Audio manager has already been created!" );

		// Does the Audio folder exist?
		if (std::filesystem::is_directory(path)) {

			HRESULT hr;

			// Initialise XAudio2
			hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
			PLAY_ASSERT_MSG( hr == S_OK, "CoInitializeEx failed" );

			hr = XAudio2Create( &m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR );
			PLAY_ASSERT_MSG( hr == S_OK, "XAudio2Create failed" );

			hr = m_pXAudio2->CreateMasteringVoice( &m_pMasterVoice );
			PLAY_ASSERT_MSG( hr == S_OK, "CreateMasteringVoice failed" );

			// Iterate through the directory loading all the sound effects
			for( auto& p : std::filesystem::directory_iterator( path ) )
			{
				// Switch everything to uppercase to avoid need to check case each time
				std::string filename = p.path().string();
				for( char& c : filename ) c = static_cast<char>(toupper( c ));

				// Only load .wav files
				if( filename.find( ".WAV" ) != std::string::npos )
				{
					SoundEffect soundEffect;
					LoadSoundEffect( filename, soundEffect );
					m_vSoundEffects.push_back( soundEffect );
				}
			}
		}

		m_bCreated = true;
		return true;
	}

	bool DestroyManager( void )
	{
		ASSERT_AUDIO;

		// Stop and destroy all the voices
		while( m_audioVoiceMap.size() > 0 )
			DestroyVoice( m_audioVoiceMap.begin()->first );

		// Delete all the sound effects
		for( SoundEffect& soundEffect : m_vSoundEffects )
			delete[] soundEffect.pFileBuffer; // The XAudio2Buffer is within the pFileBuffer data

		// Close down XAudio2
		if( m_pMasterVoice )
			m_pMasterVoice->DestroyVoice();

		m_bCreated = false;
		return true;
	}

	//********************************************************************************************************************************
	// Sound playing functions
	//********************************************************************************************************************************
	int StartSound( const char* name, bool bLoop, float volume ,float freqMod )
	{
		ASSERT_AUDIO;

		static VoiceCallback voiceCallback;

		// Switch everything to uppercase to avoid need to check case each time
		std::string filename( name );
		for( char& c : filename ) c = static_cast<char>(toupper( c ));

		// Iterate through the sound effect data looking for the requested effect
		for( SoundEffect& soundEffect : m_vSoundEffects )
		{
			if( soundEffect.fileAndPath.find( filename ) != std::string::npos )
			{
				m_voiceMutex.lock(); // XAudio2 is multi-threaded so we need to protect this code from being run in multiple threads at the same time.

				// Create a new voice to play the sound
				AudioVoice* pAudioVoice = new AudioVoice;
				m_pXAudio2->CreateSourceVoice( &pAudioVoice->pSourceVoice, (WAVEFORMATEX*)&soundEffect.format, 0u, 2.0f, &voiceCallback );
				pAudioVoice->pSoundEffect = &soundEffect;
				soundEffect.xAudio2Buffer.pContext = &pAudioVoice->id;
				soundEffect.xAudio2Buffer.LoopCount = bLoop ? XAUDIO2_LOOP_INFINITE : 0;
				pAudioVoice->pSourceVoice->SubmitSourceBuffer( &soundEffect.xAudio2Buffer, soundEffect.isXWMA? &soundEffect.xAudio2BufferWMA : 0);
				pAudioVoice->pSourceVoice->SetVolume( volume );
				pAudioVoice->pSourceVoice->SetFrequencyRatio( freqMod );
				pAudioVoice->pSourceVoice->Start( 0 );

				m_audioVoiceMap.insert( std::map<int, AudioVoice&>::value_type( pAudioVoice->id, *pAudioVoice ) );// Copy voice data pointer to the persistent map

				m_voiceMutex.unlock(); // Should be fine for concurrent threads to operate safely from this point
				return pAudioVoice->id;
			}
		}
		PLAY_ASSERT_MSG( false, std::string( "Trying to play unknown sound effect: " + std::string( name ) + "\nTry checking the 'Audio' folder").c_str());
		return -1;
	}

	bool StopSound( int voiceId )
	{
		ASSERT_AUDIO;
		return DestroyVoice( voiceId );
	}

	bool StopSound( const char* name )
	{
		ASSERT_AUDIO;

		// Switch everything to uppercase to avoid need to check case each time
		std::string filename = name;
		for( char& c : filename ) c = static_cast<char>(toupper( c ));

		// Iterate through all the audio voices and stop the requested effect
		for( std::pair<const int, AudioVoice&>& i : m_audioVoiceMap )
		{
			PLAY_ASSERT( i.second.pSoundEffect && i.second.pSourceVoice );

			if( i.second.pSoundEffect->fileAndPath.find( filename ) != std::string::npos )
				return DestroyVoice( i.first );
		}
		return false;
	}

	void SetLoopingSoundVolume( const char* name, float volume )
	{
		ASSERT_AUDIO;

		// Switch everything to uppercase to avoid need to check case each time
		std::string filename(name);
		for (char& c : filename) c = static_cast<char>(toupper(c));

		// Iterate through all the audio voices and stop the requested effect
		for( std::pair<const int, AudioVoice&>& i : m_audioVoiceMap )
		{
			PLAY_ASSERT( i.second.pSoundEffect && i.second.pSourceVoice );

			if( i.second.pSoundEffect->fileAndPath.find( filename ) != std::string::npos )
				i.second.pSourceVoice->SetVolume(volume);
		}
	}

	void SetLoopingSoundVolume(int voiceId, float volume)
	{
		ASSERT_AUDIO;

		// Try and find the voice to see if it exists
		if( m_audioVoiceMap.find( voiceId ) == m_audioVoiceMap.end() )
			return;

		AudioVoice* voice = &m_audioVoiceMap.find( voiceId )->second;
		voice->pSourceVoice->SetVolume(volume);
	}

	void SetLoopingSoundPitch( const char* name, float freqMod )
	{
		ASSERT_AUDIO;

		// Switch everything to uppercase to avoid need to check case each time
		std::string filename(name);
		for (char& c : filename) c = static_cast<char>(toupper(c));

		// Iterate through all the audio voices and stop the requested effect
		for( std::pair<const int, AudioVoice&>& i : m_audioVoiceMap )
		{
			PLAY_ASSERT( i.second.pSoundEffect && i.second.pSourceVoice );

			if( i.second.pSoundEffect->fileAndPath.find( filename ) != std::string::npos )
			{
				i.second.pSourceVoice->SetFrequencyRatio(freqMod);
			}
		}
	}

	void SetLoopingSoundPitch(int voiceId, float freqMod)
	{
		ASSERT_AUDIO;

		// Try and find the voice to see if it exists
		if( m_audioVoiceMap.find( voiceId ) == m_audioVoiceMap.end() )
			return;

		AudioVoice* voice = &m_audioVoiceMap.find( voiceId )->second;
		voice->pSourceVoice->SetFrequencyRatio(freqMod);
	}

	bool LoadSoundEffect( std::string& filename, SoundEffect& soundEffect )
	{
		// RIFF (Resource Interchange File Format) is a tagged file structure for multimedia resource files. 
		// The RIFF structure identifies supported file formats using four-character codes, and groups their data into chunks. 
		struct RiffChunk
		{
			uint32_t m_id; // The type of data (4x char)
			uint32_t m_size; // The size of the chunk
		};

		// Open the file
		std::ifstream file;
		file.open( filename, std::ios::binary ); 
		PLAY_ASSERT( file.is_open() );

		// Get the size of file
		file.seekg( 0, std::ios::end );
		int fileSize = static_cast<int>(file.tellg());
		file.seekg( 0, std::ios::beg );

		// Allocate and read in the file
		soundEffect.pFileBuffer = new uint8_t[ fileSize ];
		file.read( (char*)soundEffect.pFileBuffer, fileSize );
		file.close();

		// Start working through the file data using a pointer
		const BYTE* p( static_cast<const BYTE*>(soundEffect.pFileBuffer) );
		const BYTE* pEnd( p + fileSize );

		// The first chunk is the root entry and must have a ID of 'RIFF' 
		const RiffChunk* pRiffChunk( reinterpret_cast<const RiffChunk*>(p) ); 
		PLAY_ASSERT_MSG( pRiffChunk->m_id == 'FFIR', "RIFF chunk invalid" ); // Reverse byte order = 'FFIR'
		p += sizeof( RiffChunk );


		// Next we're looking for a WAVE chunk (.WAV file) or a XWMA chunk (xWMA compressed audio).
		const uint32_t* pWave( reinterpret_cast<const uint32_t*>(p) ); // WAVE
		PLAY_ASSERT_MSG( *pWave == 'EVAW' || *pWave == 'AMWX', "WAVE/XWMA chunk invalid" );

		p += sizeof( uint32_t );

		// Subsequent chunks should be the sound data
		const RiffChunk* pChunk( reinterpret_cast<const RiffChunk*>(p) );
		p += sizeof( RiffChunk );

		bool bFoundFormat = false;
		bool bFoundData = false;
		bool bFounddpds = false;

		// Work through all the data
		while( p < pEnd )
		{
			switch( pChunk->m_id )
			{
			case ' tmf': // format chunk (fmt backwards)

				PLAY_ASSERT_MSG( sizeof( PCMWAVEFORMAT ) <= pChunk->m_size, "_fmt chunk invalid" );
				memcpy( &soundEffect.format, p, sizeof( PCMWAVEFORMAT ) );
				if (soundEffect.format.Format.wFormatTag == WAVE_FORMAT_WMAUDIO2 || soundEffect.format.Format.wFormatTag == WAVE_FORMAT_WMAUDIO3)
					soundEffect.isXWMA = true;
				bFoundFormat = true;
				break;

			case 'atad': // Data chunk (data backwards)
				soundEffect.xAudio2Buffer.pAudioData = p;
				soundEffect.xAudio2Buffer.AudioBytes = pChunk->m_size;
				bFoundData = true;
				break;
			case 'sdpd': // DPDS XWMA chunk 
				{
					if (soundEffect.isXWMA)
					{
						soundEffect.xAudio2BufferWMA.pDecodedPacketCumulativeBytes = reinterpret_cast<const uint32_t*>(p);
						soundEffect.xAudio2BufferWMA.PacketCount = pChunk->m_size / 4;
						bFounddpds = true;
					}
					break;
				}
			}

			p += pChunk->m_size;

			pChunk = reinterpret_cast<const RiffChunk*>(p);
			p += sizeof( RiffChunk );
		}

		
		PLAY_ASSERT_MSG( bFoundFormat && bFoundData, "Invalid Sound Data" );
		if (soundEffect.isXWMA)
			PLAY_ASSERT_MSG( bFounddpds, "Invalid XWMA sound")

		// Initialise typical flags (some overwritten on play)
		soundEffect.xAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
		soundEffect.xAudio2Buffer.LoopBegin = 0u;
		soundEffect.xAudio2Buffer.LoopLength = 0u;
		soundEffect.xAudio2Buffer.LoopCount = 0;
		soundEffect.fileAndPath = filename;

		return true;
	}

	bool DestroyVoice( int voiceId )
	{
		// Only delete audio voices that exist!
		if( m_audioVoiceMap.find( voiceId ) == m_audioVoiceMap.end() )
			return false;

		m_voiceMutex.lock(); // XAudio2 is multi-threaded so we need to protect this code from being run in multiple threads at the same time.

		AudioVoice* voice = &m_audioVoiceMap.find( voiceId )->second;

		// Stop and destroy the audio voice
		voice->pSourceVoice->Stop();
		voice->pSourceVoice->FlushSourceBuffers();
		voice->pSoundEffect = nullptr;
		voice->pSourceVoice->DestroyVoice();
		m_audioVoiceMap.erase( m_audioVoiceMap.find( voiceId ) );
		delete voice;

		m_voiceMutex.unlock(); // Should be fine for concurrent threads to operate safely from this point

		return true;
	}
}
//********************************************************************************************************************************
// File:		PlayInput.cpp
// Description:	Manages keyboard and mouse input 
// Platform:	Windows
// Notes:		Obtains mouse data from PlayWindow via MouseData structure
//********************************************************************************************************************************


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
//********************************************************************************************************************************
// File:		PlayManager.cpp
// Description:	A manager for providing simplified access to the PlayBuffer framework
// Platform:	Independent
// Notes:		The GameObject management is "opt in" as many will want to create their own GameObject class
//********************************************************************************************************************************

namespace Play
{
	// A set of default colour definitions
	Colour cBlack{ 0.0f, 0.0f, 0.0f };
	Colour cRed{ 100.0f, 0.0f, 0.0f };
	Colour cGreen{ 0.0f, 100.0f, 0.0f };
	Colour cBlue{ 0.0f, 0.0f, 100.0f };
	Colour cMagenta{ 100.0f, 0.0f, 100.0f };
	Colour cCyan{ 0.0f, 100.0f, 100.0f };
	Colour cYellow{ 100.0f, 100.0f, 0.0f };
	Colour cOrange{ 100.0f, 50.0f, 0.0f };
	Colour cWhite{ 100.0f, 100.0f, 100.0f };
	Colour cGrey{ 50.0f, 50.0f, 50.0f };

	int frameCount = 0; // Updated in Play::Present

	// The camera
	Point2f cameraPos{ 0.0f, 0.0f };
	// Spaces and co-ordinate systems
	DrawingSpace drawSpace = DrawingSpace::WORLD;

	//**************************************************************************************************
	// Manager creation and deletion
	//**************************************************************************************************

	void CreateManager( int displayWidth, int displayHeight, int displayScale )
	{
		Play::Graphics::CreateManager( displayWidth, displayHeight, "Data\\Sprites\\" );
		Play::Window::CreateManager( Play::Graphics::GetDrawingBuffer(), displayScale );
		Play::Window::RegisterMouse( Play::Input::CreateManager() );
		Play::Audio::CreateManager( "Data\\Audio\\" );
		// Seed the game's random number generator based on the time
		srand( (int)time( NULL ) );
	}

	void DestroyManager()
	{
		Play::Audio::DestroyManager();
		Play::Graphics::DestroyManager();
		Play::Window::DestroyManager();
		Play::Input::DestroyManager();
#ifdef PLAY_USING_GAMEOBJECT_MANAGER
		Play::DestroyAllGameObjects();
#endif
	}

	//**************************************************************************************************
	// PlayGraphics functions
	//**************************************************************************************************

	void ClearDrawingBuffer( Colour c )
	{
		int r = static_cast<int>( c.red * 2.55f );
		int g = static_cast<int>( c.green * 2.55f );
		int b = static_cast<int>( c.blue * 2.55f );
		Play::Graphics::ClearBuffer( { r, g, b } );
	}

	void DrawDebugText( Point2D pos, const char* text, Colour c, bool centred )
	{
		Play::Graphics::DrawDebugString( TRANSFORM_SPACE( pos ), text, { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f }, centred );
	}

	void PresentDrawingBuffer()
	{
		static bool debugInfo = false;
		DrawingSpace originalDrawSpace = drawSpace;

		if( KeyPressed( KEY_F1 ) )
			debugInfo = !debugInfo;

		if( debugInfo )
		{
			drawSpace = DrawingSpace::SCREEN;

			int textX = 10;
			int textY = 10;
			std::string s = "PlayBuffer Version:" + std::string( PLAY_VERSION );
			Play::Graphics::DrawDebugString( { textX - 1, textY - 1 }, s, PIX_BLACK, false );
			Play::Graphics::DrawDebugString( { textX + 1, textY + 1 }, s, PIX_BLACK, false );
			Play::Graphics::DrawDebugString( { textX + 1, textY - 1 }, s, PIX_BLACK, false );
			Play::Graphics::DrawDebugString( { textX - 1, textY + 1 }, s, PIX_BLACK, false );
			Play::Graphics::DrawDebugString( { textX, textY }, s, PIX_YELLOW, false );

			drawSpace = DrawingSpace::WORLD;

#ifdef PLAY_USING_GAMEOBJECT_MANAGER	
			DrawGameObjectsDebug();
#endif
		}

		Play::Window::Present();
		frameCount++;

		drawSpace = originalDrawSpace;
	}

	//**************************************************************************************************
	// PlayAudio functions
	//**************************************************************************************************

	int PlayAudioPitch( const char* fileName, int minPercent, int maxPercent )
	{
		PLAY_ASSERT_MSG( minPercent > 1.0f && maxPercent < 10000.0f, "Invalid audio pitch range: must be between 1 and 10,000 %" );
		float freq = (minPercent + rand() % (maxPercent - minPercent)) / 100.0f;
		return Play::Audio::StartSound( fileName, false, 1.0f, freq);
	}

	//**************************************************************************************************
	// PlayGraphics functions
	//**************************************************************************************************

	void ColourSprite( const char* spriteName, Colour c )
	{
		int spriteId = Play::Graphics::GetSpriteId( spriteName );
		Play::Graphics::ColourSprite( spriteId, static_cast<int>( c.red * 2.55f ), static_cast<int>( c.green * 2.55f), static_cast<int>( c.blue * 2.55f ) );
	}

	void CentreSpriteOrigin( const char* spriteName )
	{
		int spriteId = Play::Graphics::GetSpriteId( spriteName );
		Play::Graphics::SetSpriteOrigin( spriteId, Play::Graphics::GetSpriteSize( spriteId ) / 2, false );
	}

	void CentreMatchingSpriteOrigins( const char* rootName )
	{
		int spriteId = Play::Graphics::GetSpriteId( rootName ); // Finds the first matching sprite and assumes same dimensions
		Play::Graphics::SetSpriteOrigins( rootName, Play::Graphics::GetSpriteSize( spriteId ) / 2, false );
	}

	void CentreAllSpriteOrigins()
	{
		for( int i = 0; i < Play::Graphics::GetTotalLoadedSprites(); i++ )
			Play::Graphics::SetSpriteOrigin( i, Play::Graphics::GetSpriteSize( i ) / 2, false );
	}

	void MoveSpriteOrigin( const char* spriteName, int xOffset, int yOffset )
	{
		int spriteId = Play::Graphics::GetSpriteId( spriteName );
		Play::Graphics::SetSpriteOrigin( spriteId, { xOffset, yOffset }, true ); // relative option set
	}


	void MoveAllSpriteOrigins(int xoffset, int yoffset)
	{
		for (int i = 0; i < Play::Graphics::GetTotalLoadedSprites(); i++)
			Play::Graphics::SetSpriteOrigin(i, { xoffset, yoffset }, true ); // relative option set
	}

	void SetSpriteOrigin( const char* spriteName, int xOrigin, int yOrigin )
	{
		int spriteId = Play::Graphics::GetSpriteId( spriteName );
		Play::Graphics::SetSpriteOrigin( spriteId, { xOrigin, yOrigin } ); 
	}
	
	void DrawSpriteTransparent(const char* spriteName, Point2D pos, int frameIndex, float opacity, Colour colour)
	{
		Graphics::DrawTransparent(Graphics::GetSpriteId(spriteName), TRANSFORM_SPACE(pos), frameIndex, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f });
	}

	void DrawSpriteTransparent(int spriteID, Point2D pos, int frameIndex, float opacity, Colour colour)
	{
		Graphics::DrawTransparent(spriteID, TRANSFORM_SPACE(pos), frameIndex, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f } );
	}

	void DrawSpriteRotated( const char* spriteName, Point2D pos, int frameIndex, float angle, float scale, float opacity, Colour colour)
	{
		Play::Graphics::DrawRotated( Play::Graphics::GetSpriteId( spriteName ), TRANSFORM_SPACE( pos ), frameIndex, angle, scale, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f });
	}

	void DrawSpriteRotated( int spriteID, Point2D pos, int frameIndex, float angle, float scale, float opacity, Colour colour )
	{
		Play::Graphics::DrawRotated( spriteID, TRANSFORM_SPACE( pos ), frameIndex, angle, scale, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f });
	}

	void DrawSpriteTransformed( int spriteID, const Matrix2D& transform, int frameIndex, float opacity, Colour colour )
	{
		Play::Graphics::DrawTransformed( spriteID, TRANSFORM_MATRIX_SPACE( transform ), frameIndex, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f });
	}

	void DrawLine( Point2f start, Point2f end, Colour c )
	{
		return Play::Graphics::DrawLine( TRANSFORM_SPACE( start), TRANSFORM_SPACE( end ), { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f }  );
	}

	void DrawCircle( Point2D pos, int radius, Colour c )
	{
		Play::Graphics::DrawCircle( TRANSFORM_SPACE( pos ), radius, { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f } );
	}

	void DrawRect(  Point2D bottomLeft, Point2D topRight, Colour c, bool fill )
	{
		Play::Graphics::DrawRect( TRANSFORM_SPACE( bottomLeft ), TRANSFORM_SPACE( topRight ), { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f }, fill );
	}

	void DrawSpriteLine( Point2D startPos, Point2D endPos, const char* penSprite, Colour col /*= cWhite */ )
	{
		int spriteId = Play::Graphics::GetSpriteId( penSprite );
		ColourSprite( penSprite, col );

		//Draws a line in any angle
		int x1 = static_cast<int>( startPos.x );
		int y1 = static_cast<int>( startPos.y );
		int x2 = static_cast<int>( endPos.x );
		int y2 = static_cast<int>( endPos.y );

		//Implementation of Bresenham's Line Drawing Algorithm
		int dx = abs( x2 - x1 );
		int sx = 1;
		if( x2 < x1 ) { sx = -1; }

		int dy = -abs( y2 - y1 );
		int sy = 1;
		if( y2 < y1 ) { sy = -1; }
		
		int err = dx + dy;

		if( dx == 0 && dy == 0 ) return;

		while( true )
		{
			Play::DrawSprite( spriteId, { x1, y1 }, 0 );
			
			if( x1 == x2 && y1 == y2 )
				break;

			int e2 = 2 * err;
			if( e2 >= dy )
			{
				err += dy;
				x1 += sx;
			}
			if( e2 <= dx )
			{
				err += dx;
				y1 += sy;
			}
		}
	}

	// Not exposed externally
	void DrawCircleOctants( int spriteId, int x, int y, int ox, int oy )
	{
		//displaying all 8 coordinates of(x,y) residing in 8-octants
		Play::DrawSprite( spriteId, { x + ox, y + oy }, 0 );
		Play::DrawSprite( spriteId, { x - ox, y + oy }, 0 );
		Play::DrawSprite( spriteId, { x + ox, y - oy }, 0 );
		Play::DrawSprite( spriteId, { x - ox, y - oy }, 0 );
		Play::DrawSprite( spriteId, { x + oy, y + ox }, 0 );
		Play::DrawSprite( spriteId, { x - oy, y + ox }, 0 );
		Play::DrawSprite( spriteId, { x + oy, y - ox }, 0 );
		Play::DrawSprite( spriteId, { x - oy, y - ox }, 0 );
	}

	void DrawSpriteCircle( Point2D pos, int radius, const char* penSprite, Colour col )
	{
		int spriteId = Play::Graphics::GetSpriteId( penSprite );
		ColourSprite( penSprite, col );

		pos = TRANSFORM_SPACE( pos );

		int ox = 0, oy = radius;
		int d = 3 - 2 * radius;
		DrawCircleOctants( spriteId, static_cast<int>(pos.x), static_cast<int>(pos.y), ox, oy );

		while( oy >= ox )
		{
			ox++;
			if( d > 0 )
			{
				oy--;
				d = d + 4 * ( ox - oy ) + 10;
			}
			else
			{
				d = d + 4 * ox + 6;
			}
			DrawCircleOctants( spriteId, static_cast<int>(pos.x), static_cast<int>(pos.y), ox, oy );
		}
	};

	void DrawFontText( const char* fontId, std::string text, Point2D pos, Align justify )
	{
		int font = Play::Graphics::GetSpriteId( fontId );

		int totalWidth{ 0 };

		for( char c : text )
			totalWidth += Play::Graphics::GetFontCharWidth( font, c );

		switch( justify )
		{
			case Align::CENTRE:
				pos.x -= totalWidth / 2;
				break;
			case Align::RIGHT:
				pos.x -= totalWidth;
				break;
			default:
				break;
		}

		pos.x += Play::Graphics::GetSpriteOrigin( font ).x;
		Play::Graphics::DrawString( font, TRANSFORM_SPACE( pos ), text );
	}

	void DrawPixel(Point2D pos, Colour col)
	{
		return Play::Graphics::DrawPixel(TRANSFORM_SPACE( pos ), { col.red * 2.55f, col.green * 2.55f, col.blue * 2.55f });
	}

	void BeginTimingBar( Colour c )
	{
		Play::Graphics::TimingBarBegin( Pixel( c.red*2.55f, c.green*2.55f, c.blue*2.55f ) );
	}

	int ColourTimingBar( Colour c )
	{
		return Play::Graphics::SetTimingBarColour( Pixel( c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f ) );
	}

	//**************************************************************************************************
	// Miscellaneous functions
	//**************************************************************************************************

	int RandomRollRange( int begin, int end )
	{
		int range = abs( end - begin );
		int rnd = ( rand() % ( range + 1 ) );
		if( end > begin )
			return begin + rnd;
		else
			return end + rnd;
	}
}
//********************************************************************************************************************************
// File:		PlayManager.cpp
// Description:	A manager for providing simplified access to the PlayBuffer framework
// Platform:	Independent
// Notes:		The GameObject management is "opt in" as many will want to create their own GameObject class
//********************************************************************************************************************************

// Define this to opt in to the PlayManager
#ifdef PLAY_USING_GAMEOBJECT_MANAGER
namespace Play
{
	// Constructor for the GameObject struct - kept as simple as possible
	GameObject::GameObject(int type, Point2f newPos, int collisionRadius, int spriteId = 0)
		: type(type), pos(newPos), radius(collisionRadius), spriteId(spriteId)
	{
		// Member variables are assigned default values in the class header
		static int uniqueId = 0;
		m_id = uniqueId++;
	}

	// A map is used internally to store all the GameObjects and their unique ids
	static std::map<int, GameObject&> objectMap;

	// Used instead of Null return values, PlayMangager operations performed on this GameObject should fail transparently
	static GameObject noObject{ -1,{ 0, 0 }, 0, -1 };


	//**************************************************************************************************
	// GameObject functions
	//**************************************************************************************************
	int CreateGameObject(int type, Point2f newPos, int collisionRadius, const char* spriteName)
	{
		int spriteId = Play::Graphics::GetSpriteId(spriteName);
		// Deletion is handled in DestroyGameObject()
		GameObject* pObj = new GameObject(type, newPos, collisionRadius, spriteId);
		int id = pObj->GetId();
		objectMap.insert(std::map<int, GameObject&>::value_type(id, *pObj));
		return id;
	}

	GameObject& GetGameObject(int ID)
	{
		std::map<int, GameObject&>::iterator i = objectMap.find(ID);

		if (i == objectMap.end())
			return noObject;

		return i->second;
	}

	GameObject& GetGameObjectByType(int type)
	{
		int count = 0;

		for (std::pair<const int, GameObject&>& i : objectMap)
			if (i.second.type == type) { count++; }

		PLAY_ASSERT_MSG(count <= 1, "Multiple objects of type found, use CollectGameObjectIDsByType instead");

		for (std::pair<const int, GameObject&>& i : objectMap)
		{
			if (i.second.type == type)
				return i.second;
		}

		return noObject;
	}

	std::vector<int> CollectGameObjectIDsByType(int type)
	{
		std::vector<int> vec;
		for (std::pair<const int, GameObject&>& i : objectMap)
		{
			if (i.second.type == type)
				vec.push_back(i.first);
		}
		return vec; // Returning a copy of the vector
	}

	std::vector<int> CollectAllGameObjectIDs()
	{
		std::vector<int> vec;

		for (std::pair<const int, GameObject&>& i : objectMap)
			vec.push_back(i.first);

		return vec; // Returning a copy of the vector
	}

	void UpdateGameObject(GameObject& obj, bool bWrap, int wrapBorderSize, bool allowMultipleUpdatesPerFrame)
	{
		if (obj.type == -1) return; // Don't update noObject

		// We allow multiple updates if the object type has changed
		PLAY_ASSERT_MSG(obj.lastFrameUpdated != Play::frameCount || obj.type != obj.oldType || allowMultipleUpdatesPerFrame, "Trying to update the same GameObject more than once in the same frame!");
		obj.lastFrameUpdated = Play::frameCount;

		// Save the current position in case we need to go back
		obj.oldPos = obj.pos;
		obj.oldRot = obj.rotation;

		// Move the object according to a very simple physical model
		obj.velocity += obj.acceleration;
		obj.pos += obj.velocity;
		obj.rotation += obj.rotSpeed;

		// Handle the animation frame update
		obj.framePos += obj.animSpeed;
		if (obj.framePos > 1.0f)
		{
			obj.frame++;
			obj.framePos -= 1.0f;
		}

		// Wrap objects around the screen
		if (bWrap)
		{
			int dWidth = Play::Window::GetWidth();
			int dHeight = Play::Window::GetHeight();
			Vector2f origin = Play::Graphics::GetSpriteOrigin(obj.spriteId);
			Vector2f spriteSize = Play::Graphics::GetSpriteSize(obj.spriteId);

			if (obj.pos.x - origin.x + spriteSize.x - wrapBorderSize > dWidth)
				obj.pos.x = 0.0f - wrapBorderSize + origin.x;
			else if (obj.pos.x - origin.x + wrapBorderSize < 0)
				obj.pos.x = dWidth + wrapBorderSize + origin.x - spriteSize.x;

			if (obj.pos.y - origin.y + spriteSize.y - wrapBorderSize > dHeight)
				obj.pos.y = 0.0f - wrapBorderSize + origin.y;
			else if (obj.pos.y - origin.y + wrapBorderSize < 0)
				obj.pos.y = dHeight + wrapBorderSize + origin.y - spriteSize.y;
		}

	}

	void DestroyGameObject(int ID)
	{
		if (objectMap.find(ID) == objectMap.end())
		{
			PLAY_ASSERT_MSG(false, "Unable to find object with given ID");
		}
		else
		{
			GameObject* go = &objectMap.find(ID)->second;
			delete go;
			objectMap.erase(ID);
		}
	}

	void DestroyAllGameObjects(void)
	{
		for (std::pair<const int, GameObject&>& p : objectMap)
			delete& p.second;
		objectMap.clear();
	}

	void DestroyGameObjectsByType(int objType)
	{
		std::vector<int> typeVec = CollectGameObjectIDsByType(objType);
		for (size_t i = 0; i < typeVec.size(); i++)
			DestroyGameObject(typeVec[i]);
	}

	bool IsColliding(GameObject& object1, GameObject& object2)
	{
		//Don't collide with noObject
		if (object1.type == -1 || object2.type == -1)
			return false;

		int xDiff = int(object1.pos.x) - int(object2.pos.x);
		int yDiff = int(object1.pos.y) - int(object2.pos.y);
		int radii = object2.radius + object1.radius;

		// Game progammers don't do square root!
		return((xDiff * xDiff) + (yDiff * yDiff) < radii * radii);
	}

	bool IsVisible(GameObject& obj)
	{
		if (obj.type == -1) return false; // Not for noObject

		int spriteID = obj.spriteId;
		Vector2f spriteSize = Play::Graphics::GetSpriteSize(obj.spriteId);
		Vector2f spriteOrigin = Play::Graphics::GetSpriteOrigin(spriteID);

		Point2f pos = TRANSFORM_SPACE( obj.pos );

		return(pos.x + spriteSize.width - spriteOrigin.x > 0 && pos.x - spriteOrigin.x < Window::GetWidth() &&
			pos.y + spriteSize.height - spriteOrigin.y > 0 && pos.y - spriteOrigin.y < Window::GetHeight());
	}

	bool IsLeavingDisplayArea(GameObject& obj, Direction dirn)
	{
		if (obj.type == -1) return false; // Not for noObject

		int spriteID = obj.spriteId;
		Vector2f spriteSize = Play::Graphics::GetSpriteSize(obj.spriteId);
		Vector2f spriteOrigin = Play::Graphics::GetSpriteOrigin(spriteID);

		Point2f pos = TRANSFORM_SPACE( obj.pos );

		if (dirn != Direction::VERTICAL)
		{
			if (pos.x - spriteOrigin.x < 0 && obj.velocity.x < 0)
				return true;

			if (pos.x + spriteSize.width - spriteOrigin.x > Window::GetWidth() && obj.velocity.x > 0)
				return true;
		}

		if (dirn != Direction::HORIZONTAL)
		{
			if (pos.y - spriteOrigin.y < 0 && obj.velocity.y < 0)
				return true;

			if (pos.y + spriteSize.height - spriteOrigin.y > Window::GetHeight() && obj.velocity.y > 0)
				return true;
		}

		return false;
	}

	bool IsAnimationComplete(GameObject& obj)
	{
		if (obj.type == -1) return false; // Not for noObject
		return obj.frame >= Play::Graphics::GetSpriteFrames(obj.spriteId) - 1;
	}

	void SetGameObjectDirection(GameObject& obj, int speed, float angle)
	{
		if (obj.type == -1) return; // Not for noObject

		obj.rotation = angle;

		obj.velocity.x = speed * cos(obj.rotation);
		obj.velocity.y = speed * sin(obj.rotation);
	}

	void PointGameObject(GameObject& obj, int targetX, int targetY)
	{
		if (obj.type == -1) return; // Not for noObject

		float xdiff = targetX - obj.pos.x;
		float ydiff = targetY - obj.pos.y;

		obj.rotation = atan2( ydiff, xdiff );
	}

	void SetSprite(GameObject& obj, const char* spriteName, float animSpeed)
	{
		int newSprite = Play::Graphics::GetSpriteId(spriteName);
		// Only reset the animation back to the start when it is new
		if (newSprite != obj.spriteId)
			obj.frame = 0;
		obj.spriteId = newSprite;
		obj.animSpeed = animSpeed;
	}

	void SetSprite( GameObject& obj, int newSprite, float animSpeed )
	{
		// Only reset the animation back to the start when it is new
		if( newSprite != obj.spriteId )
			obj.frame = 0;
		obj.spriteId = newSprite;
		obj.animSpeed = animSpeed;
	}


	void DrawObject(GameObject& obj)
	{
		if (obj.type == -1) return; // Don't draw noObject
		Play::Graphics::Draw(obj.spriteId, TRANSFORM_SPACE( obj.pos ), obj.frame);
	}

	void DrawObjectTransparent(GameObject& obj, float opacity)
	{
		if (obj.type == -1) return; // Don't draw noObject
		Play::Graphics::DrawTransparent(obj.spriteId, TRANSFORM_SPACE( obj.pos ), obj.frame, { opacity, 1.0f, 1.0f, 1.0f });
	}

	void DrawObjectRotated(GameObject& obj, float opacity)
	{
		if (obj.type == -1) return; // Don't draw noObject
		Play::Graphics::DrawRotated(obj.spriteId, TRANSFORM_SPACE( obj.pos ), obj.frame, obj.rotation, obj.scale, { opacity, 1.0f, 1.0f, 1.0f });
	}

	void DrawGameObjectsDebug()
	{
		for( std::pair<const int, GameObject&>& i : objectMap )
		{
			GameObject& obj = i.second;
			int id = obj.spriteId;
			Play::Vector2D size = Play::Graphics::GetSpriteSize( obj.spriteId );
			Play::Vector2D origin = Play::Graphics::GetSpriteOrigin( id );

			// Corners of sprite drawing area
			Play::Point2D p0 = obj.pos - origin;
			Play::Point2D p2 = { obj.pos.x + size.width - origin.x, obj.pos.y + size.height - origin.y };
			Play::Point2D p1 = { p2.x, p0.y };
			Play::Point2D p3 = { p0.x, p2.y };

			DrawLine( p0, p1, Play::cRed );
			DrawLine( p1, p2, Play::cRed );
			DrawLine( p2, p3, Play::cRed );
			DrawLine( p3, p0, Play::cRed );

			DrawCircle( obj.pos, obj.radius, Play::cBlue );

			Play::DrawLine( { obj.pos.x - 20,  obj.pos.y - 20 }, { obj.pos.x + 20, obj.pos.y + 20 }, Play::cWhite );
			Play::DrawLine( { obj.pos.x + 20, obj.pos.y - 20 }, { obj.pos.x - 20, obj.pos.y + 20 }, Play::cWhite );

			std::string s = Play::Graphics::GetSpriteName( obj.spriteId ) + " f[" + std::to_string( obj.frame % Play::Graphics::GetSpriteFrames( obj.spriteId ) ) + "]";
			Play::DrawDebugText( { (p0.x + p1.x) / 2.0f, p0.y - 20 }, s.c_str() );
		}
	}
}
#endif
#endif // PLAY_IMPLEMENTATION

#ifdef PLAY_IMPLEMENTATION
#undef PLAY_IMPLEMENTATION // try to prevent multiple implementations
#endif
