//********************************************************************************************************************************
// File:		PlayBlitter.cpp
// Description:	A software pixel renderer for drawing 2D primitives into a PixelData buffer
// Platform:	Independent
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

PlayBlitter::PlayBlitter( PixelData* pRenderTarget )
{
	m_pRenderTarget = pRenderTarget;
}


void PlayBlitter::DrawPixel( int posX, int posY, Pixel srcPix )
{
	if( srcPix.a == 0x00 || posX < 0 || posX >= m_pRenderTarget->width || posY < 0 || posY >= m_pRenderTarget->height )
		return;

	Pixel* destPix = &m_pRenderTarget->pPixels[( posY * m_pRenderTarget->width ) + posX];

	if( srcPix.a == 0xFF ) // Completely opaque pixel - no need to blend
	{
		*destPix = srcPix.bits;
	}
	else
	{
		Pixel blendPix = *destPix;
		float srcAlpha = srcPix.a / 255.0f;
		float oneMinusSrcAlpha = 1.0f - srcAlpha;

		blendPix.a = 0xFF;
		blendPix.r = static_cast<uint8_t>( ( srcAlpha * srcPix.r ) + ( oneMinusSrcAlpha * blendPix.r ) );
		blendPix.g = static_cast<uint8_t>( ( srcAlpha * srcPix.g ) + ( oneMinusSrcAlpha * blendPix.g ) );
		blendPix.b = static_cast<uint8_t>( ( srcAlpha * srcPix.b ) + ( oneMinusSrcAlpha * blendPix.b ) );

		*destPix = blendPix.bits;
	}

	return;
}

void PlayBlitter::DrawLine( int startX, int startY, int endX, int endY, Pixel pix )
{
	//Implementation of Bresenham's Line Drawing Algorithm
	int dx = abs( endX - startX );
	int sx = 1;
	if( endX < startX ) { sx = -1; }

	int dy = -abs( endY - startY );
	int sy = 1;
	if( endY < startY ) { sy = -1; }

	int err = dx + dy;

	if( err == 0 ) return;

	int x = startX;
	int y = startY;

	while( true )
	{
		DrawPixel( x, y, pix );

		if( x == endX && y == endY )
			break;

		int e2 = 2 * err;
		if( e2 >= dy )
		{
			err += dy;
			x += sx;
		}
		if( e2 <= dx )
		{
			err += dx;
			y += sy;
		}
	}
}

//********************************************************************************************************************************
// Function:	BlitPixels - draws image data with and without a global alpha multiply
// Parameters:	spriteId = the id of the sprite to draw
//				xpos, ypos = the position you want to draw the sprite
//				frameIndex = which frame of the animation to draw (wrapped)
// Notes:		Alpha multiply approach is relatively unoptimized
//********************************************************************************************************************************
void PlayBlitter::BlitPixels( const PixelData& srcPixelData, int srcOffset, int blitX, int blitY, int blitWidth, int blitHeight, float alphaMultiply ) const
{
	PLAY_ASSERT_MSG( m_pRenderTarget, "Render target not set for PlayBlitter" );

	// Nothing within the display buffer to draw
	if( blitX > m_pRenderTarget->width || blitX + blitWidth < 0 || blitY > m_pRenderTarget->height || blitY + blitHeight < 0 )
		return;

	// Work out if we need to clip to the display buffer (and by how much)
	int xClipStart = -blitX;
	if( xClipStart < 0 ) { xClipStart = 0; }

	int xClipEnd = ( blitX + blitWidth ) - m_pRenderTarget->width;
	if( xClipEnd < 0 ) { xClipEnd = 0; }

	int yClipStart = -blitY;
	if( yClipStart < 0 ) { yClipStart = 0; }

	int yClipEnd = ( blitY + blitHeight ) - m_pRenderTarget->height;
	if( yClipEnd < 0 ) { yClipEnd = 0; }

	// Set up the source and destination pointers based on clipping
	int destOffset = ( m_pRenderTarget->width * ( blitY + yClipStart ) ) + ( blitX + xClipStart );
	uint32_t* destPixels = &m_pRenderTarget->pPixels->bits + destOffset;

	int srcClipOffset = ( srcPixelData.width * yClipStart ) + xClipStart;
	uint32_t* srcPixels = &srcPixelData.pPixels->bits + srcOffset + srcClipOffset;

	// Work out in advance how much we need to add to src and dest to reach the next row 
	int destInc = m_pRenderTarget->width - blitWidth + xClipEnd + xClipStart;
	int srcInc = srcPixelData.width - blitWidth + xClipEnd + xClipStart;

	//Work out final pixel in destination.
	int destColOffset = ( m_pRenderTarget->width * ( blitHeight - yClipEnd - yClipStart - 1 ) ) + ( blitWidth - xClipEnd - xClipStart );
	uint32_t* destColEnd = destPixels + destColOffset;

	//How many pixels per row in sprite.
	int endRow = blitWidth - xClipEnd - xClipStart;

	if( alphaMultiply < 1.0f )
	{
		// *******************************************************************************************************************************************************
		// A basic (unoptimized) approach which separates the channels and performs a 'typical' alpha blending operation: (src * srcAlpha)+(dest * (1-srcAlpha))
		// Has the advantage that a global alpha multiplication can be easily added over the top, so we use this method when a global multiply is required
		// *******************************************************************************************************************************************************

		// Slightly more optimised iterations without the additions in the loop
		while( destPixels < destColEnd )
		{
			uint32_t* destRowEnd = destPixels + endRow;

			while( destPixels < destRowEnd )
			{
				uint32_t src = *srcPixels++;
				uint32_t dest = *destPixels;

				// If this isn't a fully transparent pixel 
				if( src < 0xFF000000 )
				{
					int srcAlpha = static_cast<int>( ( 0xFF - ( src >> 24 ) ) * alphaMultiply );
					int constAlpha = static_cast<int>( 255 * alphaMultiply );

					// Source pixels are already multiplied by srcAlpha so we just apply the constant alpha multiplier
					int destRed = constAlpha * ( ( src >> 16 ) & 0xFF );
					int destGreen = constAlpha * ( ( src >> 8 ) & 0xFF );
					int destBlue = constAlpha * ( src & 0xFF );

					int invSrcAlpha = 0xFF - srcAlpha;

					// Apply a standard Alpha blend [ src*srcAlpha + dest*(1-SrcAlpha) ]
					destRed += invSrcAlpha * ( ( dest >> 16 ) & 0xFF );
					destGreen += invSrcAlpha * ( ( dest >> 8 ) & 0xFF );
					destBlue += invSrcAlpha * ( dest & 0xFF );

					// Bring back to the range 0-255
					destRed >>= 8;
					destGreen >>= 8;
					destBlue >>= 8;

					// Put ARGB components back together again
					*destPixels++ = 0xFF000000 | ( destRed << 16 ) | ( destGreen << 8 ) | destBlue;
				}
				else
				{
					// If this is a fully transparent pixel then the low bits store how many there are in a row
					// This means we can skip to the next pixel which isn't fully transparent
					uint32_t skip = static_cast<uint32_t>( destRowEnd - destPixels ) - 1;
					src = src & 0x00FFFFFF;
					if( skip > src ) skip = src;

					srcPixels += skip;
					++destPixels += skip;
				}
			}
			// Increase buffers by pre-calculated amounts
			destPixels += destInc;
			srcPixels += srcInc;
		}

	}
	else
	{
		// *******************************************************************************************************************************************************
		// An optimized approach which uses pre-multiplied alpha, parallel channel multiplication and pixel skipping to achieve the same 'typical' alpha 
		// blending operation (src * srcAlpha)+(dest * (1-srcAlpha)). Not easy to apply a global alpha multiplication over the top, but used everywhere else.
		// *******************************************************************************************************************************************************

		// Slightly more optimised iterations without the additions in the loop
		while( destPixels < destColEnd )
		{
			uint32_t* destRowEnd = destPixels + endRow;

			while( destPixels < destRowEnd )
			{
				uint32_t src = *srcPixels++;
				uint32_t dest = *destPixels;

				// If this isn't a fully transparent pixel 
				if( src < 0xFF000000 )
				{
					// This performes the dest*(1-srcAlpha) calculation for all channels in parallel with minor accuracy loss in dest colour.
					// It does this by shifting all the destination channels down by 4 bits in order to "make room" for the later multiplication.
					// After shifting down, it masks out the bits which have shifted into the adjacent channel data.
					// This causes the RGB data to be rounded down to their nearest 16 producing a reduction in colour accuracy.
					// This is then multiplied by the inverse alpha (inversed in PreMultiplyAlpha), also divided by 16 (hence >> 8+8+8+4).
					// The multiplication brings our RGB values back up to their original bit ranges (albeit rounded to the nearest 16).
					// As the colour accuracy only affects the destination pixels behind semi-transparent source pixels and so isn't very obvious.
					dest = ( ( ( dest >> 4 ) & 0x000F0F0F ) * ( src >> 28 ) );
					// Add the (pre-multiplied Alpha) source to the destination and force alpha to opaque
					*destPixels++ = ( src + dest ) | 0xFF000000;
				}
				else
				{
					// If this is a fully transparent pixel then the low bits store how many there are in a row
					// This means we can skip to the next pixel which isn't fully transparent
					uint32_t skip = static_cast<uint32_t>( destRowEnd - destPixels ) - 1;
					src = src & 0x00FFFFFF;
					if( skip > src ) skip = src;

					srcPixels += skip;
					++destPixels += skip;
				}
			}
			// Increase buffers by pre-calculated amounts
			destPixels += destInc;
			srcPixels += srcInc;
		}

	}

	return;
}

//********************************************************************************************************************************
// Function:	RotateScaleSprite - draws a rotated and scaled sprite with global alpha multiply
// Parameters:	s = the sprite to draw
//				xpos, ypos = the position of the center of rotation.
//				frameIndex = which frame of the animation to draw (wrapped)
//				angle = rotation angle
//				scale = parameter to magnify the sprite.
//				rotOffX, rotOffY = offset of centre of rotation to the top left of the sprite
//				alpha = the fraction defining the amount of sprite and background that is draw. 255 = all sprite, 0 = all background.
// Notes:		Pre-calculates roughly where the sprite will be in the display buffer and only processes those pixels. 
//				Approx 15 times slower than not rotating.
//********************************************************************************************************************************
void PlayBlitter::RotateScalePixels( const PixelData& srcPixelData, int srcOffset, int blitX, int blitY, int blitWidth, int blitHeight, int originX, int originY, float angle, float scale, float alphaMultiply ) const
{
	PLAY_ASSERT_MSG( m_pRenderTarget, "Render target not set for PlayBlitter" );

	//pointers to start of source and destination buffers
	uint32_t* pSrcBase = &srcPixelData.pPixels->bits + srcOffset;
	uint32_t* pDstBase = &m_pRenderTarget->pPixels->bits;

	//the centre of rotation in the sprite frame relative to the top corner
	float fRotCentreU = static_cast<float>( originX );
	float fRotCentreV = static_cast<float>( originY );

	//u/v are co-ordinates in the rotated sprite frame. x/y are screen buffer co-ordinates.
	//change in u/v for a unit change in x/y.
	float dUdX = static_cast<float>( cos( -angle ) ) * ( 1.0f / scale );
	float dVdX = static_cast<float>( sin( -angle ) ) * ( 1.0f / scale );
	float dUdY = -dVdX;
	float dVdY = dUdX;

	//Position in the sprite rotated frame with origin at the center of rotation of the sprite corners.
	float leftU = -fRotCentreU * scale;
	float rightU = ( blitWidth + -fRotCentreU ) * scale;
	float topV = ( -fRotCentreV ) * scale;
	float bottomV = ( blitHeight - fRotCentreV ) * scale;

	//Scale added in to cancel out the scale in dUdX.
	float boundingBoxCorners[4][2]
	{
		{ ( dUdX * leftU + dVdX * topV ) * scale,			( dUdY * leftU + dVdY * topV ) * scale		},	// Top left
		{ ( dUdX * leftU + dVdX * bottomV ) * scale,		( dUdY * leftU + dVdY * bottomV ) * scale		},	// Bottom left
		{ ( dUdX * rightU + dVdX * bottomV ) * scale,		( dUdY * rightU + dVdY * bottomV ) * scale	},	// Bottom right
		{ ( dUdX * rightU + dVdX * topV ) * scale,			( dUdY * rightU + dVdY * topV ) * scale,		},	// Top right
	};

	float minX = std::numeric_limits<float>::infinity();
	float minY = std::numeric_limits<float>::infinity();
	float maxX = -std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();

	//calculate the extremes of the rotated corners.
	for( int i = 0; i < 4; i++ )
	{
		minX = std::min( minX, boundingBoxCorners[i][0] );
		maxX = std::max( maxX, boundingBoxCorners[i][0] );
		minY = std::min( minY, boundingBoxCorners[i][1] );
		maxY = std::max( maxY, boundingBoxCorners[i][1] );
	}

	//clip the starting and finishing positions.
	int startY = blitY + static_cast<int>( minY );
	if( startY < 0 ) { startY = 0; minY = static_cast<float>( -blitY ); }

	int endY = blitY + static_cast<int>( maxY );
	if( endY > m_pRenderTarget->height ) { endY = m_pRenderTarget->height; }

	int startX = blitX + static_cast<int>( minX );
	if( startX < 0 ) { startX = 0; minX = static_cast<float>( -blitX ); }

	int endX = blitX + static_cast<int>( maxX );
	if( endX > m_pRenderTarget->width ) { endX = m_pRenderTarget->width; }

	//rotate the basis so we get the edge of the bounding box in the sprite frame.
	float startingU = dUdX * minX + dUdY * minY + fRotCentreU;
	float startingV = dVdY * minY + dVdX * minX + fRotCentreV;

	float rowU = startingU;
	float rowV = startingV;

	uint32_t* destPixels = pDstBase + ( static_cast<size_t>( m_pRenderTarget->width ) * startY ) + startX;
	int nextRow = m_pRenderTarget->width - ( endX - startX );

	uint32_t* srcPixels = pSrcBase;

	//Start of double for loop. 
	for( int y = startY; y < endY; y++ )
	{
		float u = rowU;
		float v = rowV;

		for( int x = startX; x < endX; x++ )
		{
			//Check to see if u and v correspond to a valid pixel in sprite.
			if( u > 0 && v > 0 && u < blitWidth && v < blitHeight )
			{
				srcPixels = pSrcBase + static_cast<size_t>( u ) + ( static_cast<size_t>( v ) * srcPixelData.width );
				uint32_t src = *srcPixels;

				if( src < 0xFF000000 )
				{
					int srcAlpha = static_cast<int>( ( 0xFF - ( src >> 24 ) ) * alphaMultiply );
					int constAlpha = static_cast<int>( 255 * alphaMultiply );

					// Source pixels are already multiplied by srcAlpha so we just apply the constant alpha multiplier
					int destRed = constAlpha * ( ( src >> 16 ) & 0xFF );
					int destGreen = constAlpha * ( ( src >> 8 ) & 0xFF );
					int destBlue = constAlpha * ( src & 0xFF );

					uint32_t dest = *destPixels;
					int invSrcAlpha = 0xFF - srcAlpha;

					// Apply a standard Alpha blend [ src*srcAlpha + dest*(1-SrcAlpha) ]
					destRed += invSrcAlpha * ( ( dest >> 16 ) & 0xFF );
					destGreen += invSrcAlpha * ( ( dest >> 8 ) & 0xFF );
					destBlue += invSrcAlpha * ( dest & 0xFF );

					// Bring back to the range 0-255
					destRed >>= 8;
					destGreen >>= 8;
					destBlue >>= 8;

					// Put ARGB components back together again
					*destPixels = 0xFF000000 | ( destRed << 16 ) | ( destGreen << 8 ) | destBlue;
				}
			}

			destPixels++;

			// Change the position in the sprite frame for changing X in the display
			u += dUdX;
			v += dVdX;
		}

		// Work out the change in the sprite frame for changing Y in the display
		rowU += dUdY;
		rowV += dVdY;
		// Next row
		destPixels += nextRow;
	}

}


void PlayBlitter::ClearRenderTarget( Pixel colour )
{
	Pixel* pBuffEnd = m_pRenderTarget->pPixels + ( m_pRenderTarget->width * m_pRenderTarget->height );
	for( Pixel* pBuff = m_pRenderTarget->pPixels; pBuff < pBuffEnd; *pBuff++ = colour.bits );
}

void PlayBlitter::BlitBackground( PixelData& backgroundImage )
{
	PLAY_ASSERT_MSG( backgroundImage.height == m_pRenderTarget->height && backgroundImage.width == m_pRenderTarget->width, "Background size doesn't match render target!" );
	// Takes about 1ms for 720p screen on i7-8550U
	memcpy( m_pRenderTarget->pPixels, backgroundImage.pPixels, sizeof( Pixel ) * m_pRenderTarget->width * m_pRenderTarget->height );
}

