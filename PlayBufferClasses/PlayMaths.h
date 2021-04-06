#ifndef PLAY_PLAYMATHS_H
#define PLAY_PLAYMATHS_H
//********************************************************************************************************************************
// File:		PlayMaths.h
// Description:	A very simple set of 2D maths structures and operations
// Platform:	Independent
//********************************************************************************************************************************

#pragma warning (push)
#pragma warning (disable : 4201) // nonstandard extension used: nameless struct/union

// The main 2D structure used in the library
struct Vector2f
{
	Vector2f() {}
	// We're encouraging implicit type conversions between float and int with the same number of parameters
	Vector2f( float x, float y ) : x( x ), y( y ) {}
	Vector2f( int x, int y ) : x( static_cast<float>( x ) ), y( static_cast<float>( y ) ) {}
	Vector2f( float x, int y ) : x( x ), y( static_cast<float>( y ) ) {}
	Vector2f( int x, float y ) : x( static_cast<float>( x ) ), y( y ) {}

	// Different ways of accessing member data
	union
	{
		float v[2];
		struct { float x; float y; };
		struct { float width; float height; };
	};

};

#pragma warning(pop)

// A point is conceptually different to a vector, but maps to Vector2f for ease of use
using Point2f = Vector2f;
using Point2D = Vector2f;
using Vector2D = Vector2f;

// Vector component operations
//**************************************************************************************************

// Vector component addition
inline Vector2f operator + ( const Vector2f& lhs, const Vector2f& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		ret.v[i] = lhs.v[i] + rhs.v[i];
	
	return ret;
}

// Vector component assignment addition
inline Vector2f& operator += ( Vector2f& lhs, const Vector2f& rhs )
{
	for( int i = 0; i < 2; ++i )
		lhs.v[i] += rhs.v[i];
	
	return lhs;
}

// Vector component subtraction
inline Vector2f operator - ( const Vector2f& lhs, const Vector2f& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		ret.v[i] = lhs.v[i] - rhs.v[i];
	
	return ret;
}

// Vector component assignment subtraction
inline Vector2f& operator -= ( Vector2f& lhs, const Vector2f& rhs )
{
	for( int i = 0; i < 2; ++i )
		lhs.v[i] -= rhs.v[i];
	
	return lhs;
}

// Vector component unary negation
inline Vector2f operator - ( const Vector2f& op )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		ret.v[i] = -op.v[i];
	
	return ret;
}

// Vector component multiplication
inline Vector2f operator * ( const Vector2f& lhs, const Vector2f& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		ret.v[i] = lhs.v[i] * rhs.v[i];
	
	return ret;
}

// Vector component assignment multiplication
inline Vector2f operator *= ( Vector2f& lhs, const Vector2f& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		lhs.v[i] *= rhs.v[i];
	
	return lhs;
}

// Vector component division
inline Vector2f operator / ( const Vector2f& lhs, const Vector2f& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		ret.v[i] = lhs.v[i] / rhs.v[i];
	
	return ret;
}

// Vector component assignment division
inline Vector2f operator /= ( Vector2f& lhs, const Vector2f& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		lhs.v[i] /= rhs.v[i];
	
	return lhs;
}

// Vector scalar operations
//**************************************************************************************************

// Vector scalar multiplication
inline Vector2f operator * ( const Vector2f& lhs, const float rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		ret.v[i] = lhs.v[i] * rhs;
	
	return ret;
}

// Vector scalar multiplication (reverse operands)
inline Vector2f operator * ( const float lhs, const Vector2f& rhs )
{
	return rhs * lhs;
}

// Vector scalar assignment multiplication
inline Vector2f operator *= ( Vector2f& lhs, const float& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		lhs.v[i] *= rhs;
	
	return lhs;
}

// Vector scalar division
inline Vector2f operator / ( const Vector2f& lhs, const float rhs )
{
	return lhs * ( float( 1 ) / rhs );
}

// Vector scalar division (reverse operands)
inline Vector2f operator / ( const float lhs, const Vector2f& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		ret.v[i] = lhs / rhs.v[i];
	
	return ret;
}

// Vector scalar assignment multiplication
inline Vector2f operator /= ( Vector2f& lhs, const float& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
		lhs.v[i] /= rhs;
	
	return lhs;
}

// Vector component comparisons
//**************************************************************************************************

// Vector components exactly equal
inline bool operator == ( const Vector2f& lhs, const Vector2f& rhs )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
	{
		if( lhs.v[i] != rhs.v[i] )
			return false;
	}
	return true;
}

// Vector components not equal
inline bool operator != ( const Vector2f& lhs, const Vector2f& rhs )
{
	return !( lhs == rhs );
}

// Vector components equal to within specified tolerance.
inline bool EqualTol( const Vector2f& lhs, const Vector2f& rhs, const float tolerance )
{
	Vector2f ret;
	for( int i = 0; i < 2; ++i )
	{
		if( std::abs( lhs.v[i] - rhs.v[i] ) > tolerance )
			return false;
	}
	return true;
}

// Common maths functions
//**************************************************************************************************

// Dot product
inline float dot( const Vector2f& lhs, const Vector2f& rhs )
{
	float ret = 0.f;
	for( int i = 0; i < 2; ++i )
		ret += lhs.v[i] * rhs.v[i];
	
	return ret;
}

// Orthogonal vector
inline Vector2f normal( const Vector2f& lhs )
{
	Vector2f ret;
	ret.v[0] = -lhs.v[1];
	ret.v[1] = lhs.v[0];
	return ret;
}

// Vector length squared
inline float lengthSqr( const Vector2f& v )
{
	return dot( v, v );
}

// Vector length
inline float length( const Vector2f& v )
{
	return sqrt( dot( v, v ) );
}

// Vector normalize
inline Vector2f normalize( const Vector2f& v )
{
	return v / length( v );
}

#endif

