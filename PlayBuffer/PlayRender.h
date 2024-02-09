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