#ifndef PLAY_PLAYBLITTER_H
#define PLAY_PLAYBLITTER_H
//********************************************************************************************************************************
// File:		PlayBlitter.h
// Description:	A software pixel renderer for drawing 2D primitives into a PixelData buffer
// Platform:	Independent
//********************************************************************************************************************************

// A software pixel renderer for drawing 2D primitives into a PixelData buffer
// > A singleton class accessed using PlayBlitter::Instance()
class PlayBlitter
{
public:

	// Constructor and initialisation
	//********************************************************************************************************************************

	// Constructor
	PlayBlitter( PixelData* pRenderTarget = nullptr );
	// Set the render target for all subsequent drawing operations
	// Returns a pointer to any previous render target
	PixelData* SetRenderTarget( PixelData* pRenderTarget ) { PixelData* old = m_pRenderTarget; m_pRenderTarget = pRenderTarget; return old; }

	// Primitive drawing functions
	//********************************************************************************************************************************

	// Sets the colour of an individual pixel on the render target
	void DrawPixel( int posX, int posY, Pixel pix );
	// Draws a line of pixels into the render target
	void DrawLine( int startX, int startY, int endX, int endY, Pixel pix );
	// Draws pixel data to the render target using a direct copy
	// > Setting alphaMultiply < 1 forces a less optimal rendering approach (~50% slower) 
	void BlitPixels( const PixelData& srcImage, int srcOffset, int blitX, int blitY, int blitWidth, int blitHeight, float alphaMultiply ) const;
	// Draws rotated and scaled pixel data to the render target (much slower than BlitPixels)
	// > Setting alphaMultiply isn't a signfiicant additional slow down on RotateScalePixels
	void RotateScalePixels( const PixelData& srcPixelData, int srcOffset, int blitX, int blitY, int blitWidth, int blitHeight, int originX, int originY, float angle, float scale, float alphaMultiply = 1.0f ) const;
	// Clears the render target using the given pixel colour
	void ClearRenderTarget( Pixel colour );
	// Copies a background image of the correct size to the render target
	void BlitBackground( PixelData& backgroundImage );

private:

	PixelData* m_pRenderTarget{ nullptr };

};

#endif