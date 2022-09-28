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


void PlayBlitter::DrawPixel( int posX, int posY, Pixel srcPix ) const
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

void PlayBlitter::DrawLine( int startX, int startY, int endX, int endY, Pixel pix ) const
{
	//Implementation of Bresenham's Line Drawing Algorithm
	int dx = abs( endX - startX );
	int sx = 1;
	if( endX < startX ) { sx = -1; }

	int dy = -abs( endY - startY );
	int sy = 1;
	if( endY < startY ) { sy = -1; }

	int err = dx + dy;

	if( dx == 0 && dy == 0 ) return;

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
// Parameters:	srcPixelData = the pixel data you want to draw
//				srcOffset = the horizontal pixel offset for the required animation frame within the PixelData
//				blitX, blitY = the position you want to draw the sprite within the buffer
//				blitWidth, blitHeight = the width and height of the animation frame
//				alphaMultiply = additional transparancy applied to the whole sprite
// Notes:		Alpha multiply approach is ~50% slower
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
// Function:	TransformPixels - draws the image data transforming each screen pixel into image space
// Parameters:	srcPixelData = the pixel data you want to draw
//				srcFrameOffset = the horizontal pixel offset for the required animation frame within the PixelData
//				srcDrawWidth, srcDrawHeight = the width and height of the source image frame
//				srcOrigin = the centre of rotation for the source image
//				alphaMultiply = additional transparancy applied to the whole sprite
// Notes:		Much slower than BlitPixels, alphaMultiply is a negligable overhead compared to the rotation
//********************************************************************************************************************************
void PlayBlitter::TransformPixels( const PixelData& srcPixelData, int srcFrameOffset, int srcDrawWidth, int srcDrawHeight, const Point2f& srcOrigin, const Matrix2D& transform, float alphaMultiply ) const
{ 
	static float inf = std::numeric_limits<float>::infinity();
	float tgt_minx{ inf }, tgt_miny{ inf }, tgt_maxx{ -inf }, tgt_maxy{ -inf };

	float x[2] = { -srcOrigin.x, srcDrawWidth - srcOrigin.x };
	float y[2] = { -srcOrigin.y, srcDrawHeight - srcOrigin.y };
	Point2f vertices[4] = { { x[0], y[0] }, { x[1], y[0] }, { x[1], y[1] }, { x[0], y[1] } };

	//calculate the extremes of the rotated corners.
	for( int i = 0; i < 4; i++ )
	{
		vertices[i] = transform.Transform( vertices[i] );
		tgt_minx = floor( tgt_minx < vertices[i].x ? tgt_minx : vertices[i].x );
		tgt_maxx = ceil( tgt_maxx > vertices[i].x ? tgt_maxx : vertices[i].x );
		tgt_miny = floor( tgt_miny < vertices[i].y ? tgt_miny : vertices[i].y );
		tgt_maxy = ceil( tgt_maxy > vertices[i].y ? tgt_maxy : vertices[i].y );
	}

	if( Determinant( transform ) == 0.0f ) return;
	Matrix2D invTransform = transform;
	invTransform.Inverse();

	int tgt_draw_width = static_cast<int>(tgt_maxx - tgt_minx);
	int tgt_draw_height = static_cast<int>(tgt_maxy - tgt_miny);
	int tgt_buffer_width = m_pRenderTarget->width;
	int tgt_buffer_height = m_pRenderTarget->height;

	if( tgt_miny < 0 ) { tgt_draw_height += (int)tgt_miny; tgt_miny = 0; }
	if( tgt_maxy > (float)tgt_buffer_height ) { tgt_draw_height -= (int)tgt_maxy - tgt_buffer_height; tgt_maxy = (float)tgt_buffer_height; }
	if( tgt_minx < 0 ) { tgt_draw_width += (int)tgt_minx; tgt_minx = 0; }
	if( tgt_maxx > (float)tgt_buffer_width ) { tgt_draw_width -= (int)tgt_maxx - tgt_buffer_width;  tgt_maxx = (float)tgt_buffer_width; }

	Point2f tgt_pixel_start{ tgt_minx, tgt_miny };
	Point2f src_pixel_start = invTransform.Transform( tgt_pixel_start ) + srcOrigin;

	float src_posx = src_pixel_start.x;
	float src_posy = src_pixel_start.y;

	int tgt_posx = static_cast<int>( tgt_pixel_start.x );
	int tgt_posy = static_cast<int>( tgt_pixel_start.y );

	float src_xincx = invTransform.row[0].x;
	float src_xincy = invTransform.row[0].y;
	float src_yincx = invTransform.row[1].x;
	float src_yincy = invTransform.row[1].y;
	float src_xresetx = src_xincx * tgt_draw_width;
	float src_xresety = src_xincy * tgt_draw_width;

	int tgt_start_pixel_index = tgt_posx + ( tgt_posy * tgt_buffer_width );
	uint32_t* tgt_pixel = (uint32_t*)m_pRenderTarget->pPixels + tgt_start_pixel_index;
	uint32_t* tgt_column_end = tgt_pixel + (tgt_draw_height * tgt_buffer_width );

	// Iterate through each pixel on the screen in turn
	while( tgt_pixel < tgt_column_end )
	{
		uint32_t* tgt_row_end = tgt_pixel + tgt_draw_width;

		while( tgt_pixel < tgt_row_end )
		{
			int roundX = static_cast<int>( src_posx + 0.5f );
			int roundY = static_cast<int>( src_posy + 0.5f );

			if( roundX >= 0 && roundY >= 0 && roundX < srcDrawWidth && roundY < srcDrawHeight )
			{
				int src_pixel_index = roundX + ( roundY * srcPixelData.width );
				uint32_t src = *( (uint32_t*)srcPixelData.pPixels + src_pixel_index + srcFrameOffset );

				// If this isn't a fully transparent pixel 
				if( src < 0xFF000000 )
				{
					int srcAlpha = static_cast<int>( ( 0xFF - ( src >> 24 ) ) * alphaMultiply );
					int constAlpha = static_cast<int>( 255 * alphaMultiply );

					// Source pixels are already multiplied by srcAlpha so we just apply the constant alpha multiplier
					int destRed = constAlpha * ( ( src >> 16 ) & 0xFF );
					int destGreen = constAlpha * ( ( src >> 8 ) & 0xFF );
					int destBlue = constAlpha * ( src & 0xFF );

					uint32_t dest = *tgt_pixel;
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
					*tgt_pixel = 0xFF000000 | ( destRed << 16 ) | ( destGreen << 8 ) | destBlue;
				}
			}

			tgt_pixel++;
			src_posx += src_xincx;
			src_posy += src_xincy;
		}

		tgt_pixel += tgt_buffer_width - tgt_draw_width;

		src_posx -= src_xresetx;
		src_posy -= src_xresety;

		src_posx += src_yincx;
		src_posy += src_yincy;
	}
}


void PlayBlitter::ClearRenderTarget( Pixel colour ) const
{
	Pixel* pBuffEnd = m_pRenderTarget->pPixels + ( m_pRenderTarget->width * m_pRenderTarget->height );
	for( Pixel* pBuff = m_pRenderTarget->pPixels; pBuff < pBuffEnd; *pBuff++ = colour.bits );
	m_pRenderTarget->preMultiplied = false;
}

void PlayBlitter::BlitBackground( PixelData& backgroundImage ) const
{
	PLAY_ASSERT_MSG( backgroundImage.height == m_pRenderTarget->height && backgroundImage.width == m_pRenderTarget->width, "Background size doesn't match render target!" );
	// Takes about 1ms for 720p screen on i7-8550U
	memcpy( m_pRenderTarget->pPixels, backgroundImage.pPixels, sizeof( Pixel ) * m_pRenderTarget->width * m_pRenderTarget->height );
}

