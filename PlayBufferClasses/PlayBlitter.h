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
	void DrawPixel( int posX, int posY, Pixel pix ) const;
	// Draws a line of pixels into the render target
	void DrawLine( int startX, int startY, int endX, int endY, Pixel pix ) const;
	// Draws pixel data to the render target using a direct copy
	// > Setting alphaMultiply < 1 forces a less optimal rendering approach (~50% slower) 
	void BlitPixels( const PixelData& srcImage, int srcOffset, int blitX, int blitY, int blitWidth, int blitHeight, float alphaMultiply ) const;
	// Draws rotated and scaled pixel data to the render target (much slower than BlitPixels)
	// > Setting alphaMultiply < 1 is not much slower overall (~10% slower) 
	void TransformPixels( const PixelData& srcPixelData, int srcFrameOffset, int srcWidth, int srcHeight, const Point2f& origin, const Matrix2D& m, float alphaMultiply = 1.0f ) const;
	// Clears the render target using the given pixel colour
	void ClearRenderTarget( Pixel colour ) const;
	// Copies a background image of the correct size to the render target
	void BlitBackground( PixelData& backgroundImage ) const;

private:

	PixelData* m_pRenderTarget{ nullptr };

};

#endif