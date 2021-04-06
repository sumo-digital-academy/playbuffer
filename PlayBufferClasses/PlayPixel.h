#ifndef PLAY_PLAYPIXEL_H
#define PLAY_PLAYPIXEL_H
//********************************************************************************************************************************
// File:		PlayPixel.h
// Platform:	Independent
// Description:	Pixel types for holding image data
//********************************************************************************************************************************

// A pixel structure to represent an ARBG format pixel
struct Pixel
{
	Pixel() {}
	Pixel( uint32_t bits ) : bits( bits ) {}
	Pixel( float r, float g, float b ) :
		a( 0xFF ), r( static_cast<uint8_t>( r ) ), g( static_cast<uint8_t>( g ) ), b( static_cast<uint8_t>( b ) )
	{
	}
	Pixel( int r, int g, int b ) :
		a( 0xFF ), r( static_cast<uint8_t>( r ) ), g( static_cast<uint8_t>( g ) ), b( static_cast<uint8_t>( b ) )
	{
	}
	Pixel( int a, int r, int g, int b ) :
		a( static_cast<uint8_t>( a ) ), r( static_cast<uint8_t>( r ) ), g( static_cast<uint8_t>( g ) ), b( static_cast<uint8_t>( b ) )
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
};

#endif