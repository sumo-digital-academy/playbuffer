//********************************************************************************************************************************
// File:		PlayRender.cpp
// Description:	A software pixel renderer for drawing 2D primitives into a PixelData buffer
// Platform:	Independent
//********************************************************************************************************************************

#include "PlayBufferPCH.h"

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