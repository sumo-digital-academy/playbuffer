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
	//! @ingroup misc
	using Point2f = Vector2f;
	//! @brief A Point on screen, with an x/y coordinate.
	//! @ingroup misc
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