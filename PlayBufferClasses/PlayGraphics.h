#ifndef PLAY_PLAYGRAPHICS_H
#define PLAY_PLAYGRAPHICS_H
//********************************************************************************************************************************
// File:		PlayGraphics.h
// Description:	Manages 2D graphics operations on a PixelData buffer 
// Platform:	Independent
// Notes:		Uses PNG format. The end of the filename indicates the number of frames e.g. "bat_4.png" or "tiles_10x10.png"
//********************************************************************************************************************************

// Manages 2D graphics operations on a PixelData buffer 
// > Singleton class accessed using PlayGraphics::Instance()
class PlayGraphics
{
public:
	// Instance functions
	//********************************************************************************************************************************

	// Creates the PlayGraphics instance and generates sprites from all the PNGs in the directory indicated
	static PlayGraphics& Instance( int bufferWidth, int bufferHeight, const char* path );
	// Returns the PlayGraphics instance
	static PlayGraphics& Instance();
	// Destroys the PlayGraphics instance
	static void Destroy();

	// Basic drawing functions
	//********************************************************************************************************************************

	// Sets the colour of an individual pixel in the display buffer
	void DrawPixel( Point2f pos, Pixel pix );
	// Draws a line of pixels into the display buffer
	void DrawLine( Point2f startPos, Point2f endPos, Pixel pix );
	// Draws a rectangle into the display buffer
	void DrawRect( Point2f topLeft, Point2f bottomRight, Pixel pix, bool fill = false );
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
	// > All sprites are normally created by the PlayGraphics constructor
	int LoadSpriteSheet( const std::string& path, const std::string& filename );
	// Adds a sprite sheet dynamically from memory (custom asset pipelines)
	// > All sprites are normally created by the PlayGraphics constructor
	int AddSprite( const std::string& name, PixelData& pixelData, int hCount = 1, int vCount = 1 );
	// Updates a sprite sheet dynamically from memory (custom asset pipelines)
	// > Left to caller to release old PixelData
	int UpdateSprite( const std::string& name, PixelData& pixelData, int hCount = 1, int vCount = 1 );
	
	// Loads a background image which is assumed to be the same size as the display buffer
	// > Returns the index of the loaded background
	int LoadBackground( const char* fileAndPath );

	// Sprite Getters and Setters
	//********************************************************************************************************************************

	// Gets the sprite id of the first matching sprite whose filename contains the given text
	// > Returns -1 if not found
	int GetSpriteId( const char* spriteName ) const;
	// Gets the root filename of a specific sprite
	const std::string& GetSpriteName( int spriteId );
	// Gets the size of the sprite with the given id
	Vector2f GetSpriteSize( int spriteId ) const;
	// Gets the number of frames in the sprite with the given id
	int GetSpriteFrames( int spriteId ) const;
	// Gets the origin of the sprite with the given id (offset from top left)
	Vector2f GetSpriteOrigin( int spriteId ) const;
	// Sets the origin of the sprite with the given id (offset from top left)
	void SetSpriteOrigin( int spriteId, Vector2f newOrigin, bool relative = false );
	// Centres the origin of the sprite with the given id
	void CentreSpriteOrigin( int spriteId );
	// Centres the origins of all the sprites
	void CentreAllSpriteOrigins();
	// Sets the origin of all sprites found matching the given name (offset from top left)
	void SetSpriteOrigins( const char* rootName, Vector2f newOrigin, bool relative = false );
	// Gets the number of sprites which have been loaded and created by PlayGraphics
	int GetTotalLoadedSprites() const { return m_nTotalSprites; }

	// Sprite Drawing functions
	//********************************************************************************************************************************

	// Draw the sprite without rotation or transparency (fastest draw)
	inline void Draw( int spriteId, Point2f pos, int frameIndex ) const { DrawTransparent( spriteId, pos, frameIndex, 1.0f ); }
	// Draw the sprite with transparency (slower than without transparency)
	void DrawTransparent( int spriteId, Point2f pos, int frameIndex, float alphaMultiply ) const; // This just to force people to consider when they use an explicit alpha multiply
	// Draw the sprite rotated with transparency (slowest draw)
	void DrawRotated( int spriteId, Point2f pos, int frameIndex, float angle, float scale = 1.0f, float alphaMultiply = 1.0f ) const;
	// Draws a previously loaded background image
	void DrawBackground( int backgroundIndex = 0 );
	// Multiplies the sprite image buffer by the colour values
	// > Applies to all subseqent drawing calls for this sprite, but can be reset by calling agin with rgb set to white
	void ColourSprite( int spriteId, int r, int g, int b );

	// Draws a string using a sprite-based font exported from PlayFontTool
	int DrawString( int fontId, Point2f pos, std::string text ) const;
	// Draws a centred string using a sprite-based font exported from PlayFontTool
	int DrawStringCentred( int fontId, Point2f pos, std::string text ) const;
	// Draws an individual text character using a sprite-based font 
	int DrawChar( int fontId, Point2f pos, char c ) const;
	// Draws a rotated text character using a sprite-based font 
	int DrawCharRotated( int fontId, Point2f pos, float angle, float scale, char c ) const;
	// Gets the width of an individual text character from a sprite-based font
	int GetFontCharWidth( int fontId, char c ) const;

	// A pixel-based sprite collision test based on drawing
	bool SpriteCollide( int s1Id, Point2f s1Pos, int s1FrameIndex, float s1Angle, int s1PixelColl[4], int s2Id, Point2f s2pos, int s2FrameIndex, float s2Angle, int s2PixelColl[4] ) const;

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
	PixelData* GetDrawingBuffer( void ) { return &m_playBuffer; }
	// Resets the timing bar data and sets the current timing bar segment to a specific colour
	void TimingBarBegin( Pixel pix );
	// Sets the current timing bar segment to a specific colour
	// > Returns the number of timing segments
	int SetTimingBarColour( Pixel pix );
	// Draws the timing bar for the previous frame at the given position and size
	void DrawTimingBar( Point2f pos, Point2f size );
	// Gets the duration (in milliseconds) of a specific timing segment
	float GetTimingSegmentDuration( int id ) const;
	// Clears the display buffer using the given pixel colour
	void ClearBuffer( Pixel colour ) { m_blitter.ClearRenderTarget( colour ); }
	// Sets the render target for drawing operations
	PixelData* SetRenderTarget( PixelData* renderTarget ) { return m_blitter.SetRenderTarget( renderTarget ); }



private:

	// Constructors / destructors
	//********************************************************************************************************************************

	// Loads all the PNGs from the directory provided and sets them up as Sprites
	PlayGraphics( int bufferWidth, int bufferHeight, const char* path );
	// Frees up all the sprites and shuts down the manager
	~PlayGraphics(); 
	// The assignment operator is removed to prevent copying of a singleton class
	PlayGraphics& operator=( const PlayGraphics& ) = delete;
	// The copy constructor is removed to prevent copying of a singleton class
	PlayGraphics( const PlayGraphics& ) = delete; 

	// Internal functions relating to drawing
	//********************************************************************************************************************************

	// Multiplies the sprite image by its own alpha transparency values to save repeating this calculation on every draw
	// > A colour multiplication can also be applied at this stage, which affects all subseqent drawing operations on the sprite
	void PreMultiplyAlpha( Pixel* source, Pixel* dest, int width, int height, int maxSkipWidth, float alphaMultiply, Pixel colourMultiply );

	// Count of the total number of sprites loaded
	int m_nTotalSprites{ 0 };
	// Whether the singleton has been initialised yet
	bool m_bInitialised{ false };

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

	// The PlayBlitter used for drawing
	PlayBlitter m_blitter;

	// Buffer pointers
	PixelData m_playBuffer;
	uint8_t* m_pDebugFontBuffer{ nullptr };

	// A vector of all the loaded sprites
	std::vector< Sprite > vSpriteData;
	// A vector of all the loaded backgrounds
	std::vector< PixelData > vBackgroundData;

	// A pointer to the static instance
	static PlayGraphics* s_pInstance;

};

#endif