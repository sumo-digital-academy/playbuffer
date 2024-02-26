#ifndef PLAY_PLAYMANAGER_H
#define PLAY_PLAYMANAGER_H
//********************************************************************************************************************************
// File:		PlayManager.h
// Description:	A manager for providing simplified access to the PlayBuffer framework with managed GameObjects
// Platform:	Independent
// Notes:		The GameObject management is "opt in" as many will want to create their own GameObject class
//********************************************************************************************************************************

#define TRANSFORM_SPACE( p )  (Play::drawSpace == Play::DrawingSpace::WORLD ? p - Play::cameraPos : p)
#define TRANSFORM_MATRIX_SPACE( t ) (Play::drawSpace == Play::DrawingSpace::WORLD ? t * (MatrixTranslation( -Play::cameraPos.x, -Play::cameraPos.y )) : t)

//! @brief Main Namespace for PlayBuffer
namespace Play
{
	//! @brief Alignment for font drawing operations
	//! @ingroup graphics
	enum Align
	{
		//! @brief Text is aligned to the left of the origin.
		LEFT = 0, 
		//! @brief Text is aligned to the right of the origin.
		RIGHT, 
		//! @brief Text is aligned to the middle of the origin.
		CENTRE,
	};

	//! @brief Used to indicate which edge of the screen you want to check if a GameObject is moving off.
	//! @ingroup gameobject
	enum Direction
	{
		//! @brief Check if it's moving off the left or right sides of the screen.
		HORIZONTAL = 0, 
		//! @brief Check if it's moving off the top or bottom sides of the screen.
		VERTICAL, 
		//! @brief Check if it's moving off any side of the screen.
		ALL,
	};


	//! @brief The drawing space values.
	//! @ingroup graphics
	//! The drawing space determines where things are drawn on the screen. There are two values for this, WORLD and SCREEN.
		enum DrawingSpace
	{
		//! @brief This takes the camera position into account when things are drawn. Essentially, the position that is used is DrawingPosition - CameraPosition.
		WORLD = 0, 
		//! @brief This ignores the camera position, and purely uses the position that gets passed through. This is most useful for user interface elements that should always be drawn in the same position on the screen.
		SCREEN,
	};

	//! @brief The Blend modes for sprite drawing.
	//! @ingroup graphics
	enum BlendMode
	{
		//! @brief This blends the sprite normally. If using a transparent sprite draw, this also uses the transparency of the sprite.
		BLEND_NORMAL = 0, 
		//! @brief This uses an additive blend, where the colour values of the sprite being drawn are added to the buffer underneath. This has the effect of brightening what is underneath the sprite.
		BLEND_ADD, 
		//! @brief This uses an multiplicative blend, where the colour values of the sprite being drawn are multiplied with what is in the buffer underneath. This will darken what is underneath the sprite.
		BLEND_MULTIPLY
	};

	//! @brief A PlayBuffer colour value. Colours are defined in percentages of red, green and blue. All zero is black, All 100 is white.
	//! @ingroup graphics
	struct Colour
	{
		//! Create a colour using float values.
		//! @param r Amount of red.
		//! @param g Amount of green.
		//! @param b Amount of blue.
		Colour( float r, float g, float b ) : red( r ), green( g ), blue( b ) {}
		//! Create a colour using int values.
		//! @param r Amount of red.
		//! @param g Amount of green.
		//! @param b Amount of blue.
		Colour( int r, int g, int b ) : red( static_cast<float>( r ) ), green( static_cast<float>( g ) ), blue( static_cast<float>( b ) ) {}
		//! @var red 
		//! Red value, from 0 to 100.
		//! @var green 
		//! Green value, from 0 to 100.
		//! @var blue
		//! Blue value, from 0 to 100.
		float red, green, blue;
	};

	extern Colour cBlack, cRed, cGreen, cBlue, cMagenta, cCyan, cYellow, cOrange, cWhite, cGrey;
	extern int frameCount;
	extern Point2f cameraPos;
	extern DrawingSpace drawSpace;


	// [DOXYGEN_EXCLUDE]
	//! @defgroup manager Manager Setup and Shutdown
	//! @brief These functions start and shutdown the PlayBuffer.
	//! 
	//! @defgroup drawingbuffer Drawing Buffer
	//! @brief These functions allow you to present the buffer for display on the screen, clear it to a single colour and get the size of the buffer.
	//! 
	//! @defgroup input Input
	//! @brief These functions let you get what keys have been pressed or held down. These also allow you to get where the mouse is and what mouse buttons are pressed or held down.
	//! 
	//! @defgroup sound Sound
	//! @brief These functions allow you to play sounds in PlayBuffer. You can play 'one-shot' sounds or looping sounds.
	//! 
	//! @defgroup camera Camera
	//! @brief These functions control the built in camera. The camera allows you to set up a "world" with tiles and then pan around it by moving the camera, rather than adjusting the position of every tile and object. It also allows you to set the drawing space.
	//! 
	//! @defgroup graphics Graphics
	//! @brief These functions allow you to draw things on screen. You can draw sprites, lines, text or even individual pixels! These functions also allow you to load sprites, get any details (such as how many animation frames or their size), and manipulate them (change their colour or their origin).
	//! 
	//! @defgroup misc Misc
	//! @brief These functions don't really fit into any of the other category. They're mainly the random number generation functions, that let you get a random value, so everything isn't identical on every run.
	// [DOXYGEN_EXCLUDE]

	// Manager creation and deletion
	//**************************************************************************************************

	//! @brief Initialises the manager and creates a window of the required dimensions.
	//! @ingroup manager
	//! @snippet PlayExamples.cpp CreateManager
	//! @param width The width of the window in pixels.
	//! @param height The height of the window in pixels.
	//! @param scale Pixel scale. One-pixel equals (scale x scale) pixels in final window.
	void CreateManager( int width, int height, int scale );
	//! @brief Shuts down the manager and closes the window.
	//! @snippet PlayExamples.cpp DestroyManager
	//! @ingroup manager
	void DestroyManager();
	// Get the width of the play buffer
	inline int GetBufferWidth() { return Play::Window::GetWidth(); }
	// Get the height of the play buffer
	inline int GetBufferHeight() { return Play::Window::GetHeight(); }

	// PlayWindow functions
	//**************************************************************************************************

	//! @brief Copies the contents of the drawing buffer to the window
	//! @snippet PlayExamples.cpp PresentDrawingBuffer
	//! @ingroup drawingbuffer
	void PresentDrawingBuffer();
	//! @brief Gets the co-ordinates of the mouse cursor within the display buffer
	//! @snippet PlayExamples.cpp GetMousePos
	//! @ingroup input
	//! @return The x/y coordinates of the mouse cursor in pixels.
	inline Point2D GetMousePos() { return Play::Input::GetMousePos(); }
	//! @brief Gets the status of the mouse buttons
	//! @snippet PlayExamples.cpp GetMouseButton
	//! @ingroup input
	//! @param button Mouse button to check
	//! @return The x/y coordinates of the mouse cursor in pixels.
	inline bool GetMouseButton( Align button ) { return Play::Input::GetMouseDown( static_cast<Play::Input::MouseButton>(button) ); }
	//! @brief Gets the width of the display buffer
	//! @snippet PlayExamples.cpp GetBufferWidth
	//! @ingroup drawingbuffer
	//! @return Width of the display buffer in pixels.
	int GetBufferWidth();
	//! @brief Gets the height of the display buffer
	//! @snippet PlayExamples.cpp GetBufferHeight
	//! @ingroup drawingbuffer
	//! @return Height of the display buffer in pixels.
	int GetBufferHeight();

	// PlayAudio functions
	//**************************************************************************************************

	//! @brief Plays an audio file from the "Data/Audio" directory, in WAV format.
	//! @snippet PlayExamples.cpp PlayAudio
	//! @ingroup sound
	//! @param wavFilename The filename of the audio file you want to play.
	//! @return The id of the sound that is playing.
	inline int PlayAudio( const char* wavFilename ) { return Play::Audio::StartSound( wavFilename, false, 1.0f, 1.0f ); }
	//! @brief Plays an audio file from the "Data/Audio" directory, in WAV format, at a random pitch from min to max percent.
	//! @snippet PlayExamples.cpp PlayAudioPitch
	//! @ingroup sound
	//! @param wavFilename The filename of the audio file you want to play.
	//! @param minPercent Minimum random pitch, as a percentage.
	//! @param maxPercent Maximum random pitch, as a percentage.
	//! @return The id of the sound that is playing.
	int PlayAudioPitch( const char* wavFilename, int minPercent, int maxPercent );
	//! @brief Plays an audio file from the "Data/Audio" directory, in WAV format, and loops it repeatedly until you stop it
	//! @snippet PlayExamples.cpp StartAudioLoop
	//! @ingroup sound
	//! @param wavFilename The filename of the audio file you want to play.
	//! @return The id of the sound that is playing.
	inline int StartAudioLoop( const char* wavFilename ) { return Play::Audio::StartSound( wavFilename, true, 1.0f, 1.0f ); }
	//! @brief Stops a looping audio file, using the sound ID that the StartAudioLoop function returned.
	//! @snippet PlayExamples.cpp StopAudioLoop
	//! @ingroup sound
	//! @param soundId The ID of the sound you want to stop.
	//! @return Whether the sound stopped playing successfully.
	inline bool StopAudioLoop( int soundId ) { return Play::Audio::StopSound( soundId ); }
	//! @brief Stops a sound, based upon the filename
	//! @snippet PlayExamples.cpp StopAudio
	//! @ingroup sound
	//! @param fileName The filename of the audio file you want to stop. This can be either a normal sound or a looping sound.
	//! @return Whether the sound stopped playing successfully.
	inline bool StopAudio( const char* fileName ) { return Play::Audio::StopSound( fileName ); }

	// Camera functions
	//**************************************************************************************************

	//! @brief Move the camera to the position specified
	//! @snippet PlayExamples.cpp SetCameraPosition
	//! @ingroup camera
	//! @param pos The desired position of the camera
	inline void SetCameraPosition( Point2f pos ) { cameraPos = pos; }
	//! @brief Changes the drawing space for all drawing functions.
	//! @snippet PlayExamples.cpp SetDrawingSpace
	//! @ingroup camera
	//! @param space What drawing space should the item be drawn in.
	inline void SetDrawingSpace( DrawingSpace space ) { drawSpace = space; }
	//! @brief Get the camera's current position
	//! @snippet PlayExamples.cpp GetCameraPosition
	//! @ingroup camera
	//! @return The position of the camera
	inline Point2f GetCameraPosition( void ) { return cameraPos; }
	//! @brief Gets the current drawing space.
	//! @snippet PlayExamples.cpp GetDrawingSpace
	//! @ingroup camera
	//! @return What drawing space PlayBuffer is currently using
	inline DrawingSpace GetDrawingSpace( void ) { return drawSpace; }

	// PlayGraphics functions
	//**************************************************************************************************

	//! @brief Clears the display buffer using the colour provided
	//! @snippet PlayExamples.cpp ClearDrawingBuffer
	//! @ingroup drawingbuffer
	//! @param col The colour to clear the drawing buffer with.
	void ClearDrawingBuffer( Colour col );
	//! @brief Loads a PNG file as a background image for the window.
	//! @snippet PlayExamples.cpp LoadBackground
	//! @ingroup drawingbuffer
	//! The image must be the same size as the drawing buffer. It cannot be smaller or larger. Multiple backgrounds can be loaded, and will have it's own index (from zero).
	//! @param pngFilename The full path and filename of the background image in PNG format.
	//! @return The index of the newly loaded background
	inline int LoadBackground( const char* pngFilename ) { return Play::Graphics::LoadBackground( pngFilename ); }
	//! @brief Draws the background image previously loaded with Play::LoadBackground() into the drawing buffer.
	//! @snippet PlayExamples.cpp DrawBackground
	//! @ingroup drawingbuffer
	//! @param background Optional argument to specify the background index. Defaults to zero (the first loaded background).
	inline void DrawBackground( int background = 0 ) { Play::Graphics::DrawBackground( background ); }
	//! @brief Draws text to the screen, using the built in debug font.
	//! @snippet PlayExamples.cpp DrawDebugText
	//! @ingroup graphics
	//! @param pos Position of the text on screen. This position is affected by the Drawing Space.
	//! @param text The text you want to draw, as a string literal.
	//! @param col Optional argument to set the colour of the text (defaults to white).
	//! @param centred Optional argument to centre the text (defaults to true).
	void DrawDebugText( Point2D pos, const char* text, Colour col = cWhite, bool centred = true );

	//! @brief Gets the sprite ID of the first matching sprite whose filename contains the given text.
	//! @snippet PlayExamples.cpp GetSpriteId
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to find the ID for. 
	//! @return The ID of the sprite, if PlayBuffer could find it. If PlayBuffer couldn't find the sprite, then it returns -1
	inline int GetSpriteId( const char* spriteName ) { return Play::Graphics::GetSpriteId( spriteName ); }
	//! @brief Gets the pixel height of a sprite
	//! @snippet PlayExamples.cpp GetSpriteHeight
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to find the height of. 
	//! @return The height of the sprite in pixels
	inline int GetSpriteHeight( const char* spriteName ) { return static_cast<int>(Play::Graphics::GetSpriteSize( GetSpriteId( spriteName ) ).height); }
	//! @brief Gets the pixel width of a sprite
	//! @snippet PlayExamples.cpp GetSpriteWidth
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to find the width of. 
	//! @return The width of the sprite in pixels
	inline int GetSpriteWidth( const char* spriteName ) { return static_cast<int>(Play::Graphics::GetSpriteSize( GetSpriteId( spriteName ) ).width); }
	//! @brief Gets the pixel height of a sprite
	//! @snippet PlayExamples.cpp GetSpriteHeightID
	//! @ingroup graphics
	//! @param spriteId The Id number of the sprite you want to find the height of. 
	//! @return The height of the sprite in pixels.
	inline int GetSpriteHeight( int spriteId ) { return static_cast<int>(Play::Graphics::GetSpriteSize( spriteId ).height); }
	//! @brief Gets the pixel width of a sprite
	//! @snippet PlayExamples.cpp GetSpriteWidthID
	//! @ingroup graphics
	//! @param spriteId The Id number of the sprite you want to find the width of. 
	//! @return The width of the sprite in pixels
	inline int GetSpriteWidth( int spriteId ) { return static_cast<int>(Play::Graphics::GetSpriteSize( spriteId ).width); }
	//! @brief Gets the stem filename for the sprite this does not include the path or the extension.
	//! @snippet PlayExamples.cpp GetSpriteName
	//! @ingroup graphics
	//! @param spriteId The Id number of the sprite you want to get the filename of.
	//! @return The stem filename for the sprite
	inline const char* GetSpriteName( int spriteId ) { return Play::Graphics::GetSpriteName( spriteId ).c_str(); }
	//! @brief Gets the total number of frames in the sprite.
	//! @snippet PlayExamples.cpp GetSpriteFrames
	//! @ingroup graphics
	//! @param spriteId The Id number of the sprite you want to find the number of frames of. 
	//! @return The number of frames that the sprite has
	inline int GetSpriteFrames( int spriteId ) { return static_cast<int>(Play::Graphics::GetSpriteFrames( spriteId )); }
	//! @brief Blends the sprite with the given colour.
	//! @snippet PlayExamples.cpp ColourSprite
	//! This function works best with sprites that are coloured white, as it multiplies the colour of the sprite with the colour that you provide.
	//! @note Sprite colouring changes the loaded in sprite. This will result in the sprite being permanently coloured for the remainder of the game. (This doesn't affect the sprite on disk!)<br>You can reset the colouring by calling this again, using a white colour.
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to colour.
	//! @param col The colour you want to blend with the sprite.
	void ColourSprite( const char* spriteName, Colour col );

	//! @brief Centres the origin of the first sprite found matching the given name
	//! @snippet PlayExamples.cpp CentreSpriteOrigin
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to centre the origin of.
	void CentreSpriteOrigin( const char* spriteName );
	//! @brief Centres the origins of all sprites found with matching parts of their name.
	//! @snippet PlayExamples.cpp CentreMatchingSpriteOrigins
	//! @ingroup graphics
	//! @param partName A common part of a sprite name to centre all the origins of.
	void CentreMatchingSpriteOrigins( const char* partName );
	//! @brief Centres the origins of all loaded sprites.
	//! @snippet PlayExamples.cpp CentreAllSpriteOrigins
	//! @ingroup graphics
	void CentreAllSpriteOrigins();
	//! @brief Moves the origins of a sprite by a given number of pixels in x and y. A sprite's origin does not have to be inside the sprite itself and can be moved any distance.
	//! @snippet PlayExamples.cpp MoveSpriteOrigin
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to centre the origin of.
	//! @param xOffset The number of pixels you want to move the origin to the right (negative value moves left).
	//! @param yOffset The number of pixels you want to move the origin up (negative value moves down).
	void MoveSpriteOrigin( const char* spriteName, int xOffset, int yOffset );
	//! @brief Moves the origins of all sprites found matching the given name by a given number of pixels. A sprite's origin does not have to be inside the sprite itself and can be moved any distance.
	//! @snippet PlayExamples.cpp MoveMatchingSpriteOrigins
	//! @ingroup graphics
	//! @param partName A common part of a sprite name to centre all the origins of.
	//! @param xoffset The number of pixels you want to move the origin to the right (negative value moves left).
	//! @param yoffset The number of pixels you want to move the origin up (negative value moves down).
	inline void MoveMatchingSpriteOrigins( const char* partName, int xoffset, int yoffset ) { Play::Graphics::SetSpriteOrigins( partName, { xoffset, yoffset }, true ); }
	//! @brief Moves the origins of all sprites by a given number of pixels in x and y. A sprite's origin does not have to be inside the sprite itself and can be moved any distance.
	//! @snippet PlayExamples.cpp MoveAllSpriteOrigins	
	//! @ingroup graphics
	//! @param xoffset The number of pixels you want to move the origin to the right (negative value moves left).
	//! @param yoffset The number of pixels you want to move the origin up (negative value moves down).
	void MoveAllSpriteOrigins( int xoffset, int yoffset );
	//! @brief Sets the origins of a sprite, to the x and y coordinates, relative to the bottom left corner of the sprite. A sprite's origin does not have to be inside the sprite itself and can be positioned anywhere.
	//! @snippet PlayExamples.cpp SetSpriteOrigin	
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to set the origin of.
	//! @param xOrigin Where you want to move the origin on the x axis, relative to the left of the sprite.
	//! @param yOrigin Where you want to move the origin on the y axis, relative to the bottom of the sprite.
	void SetSpriteOrigin( const char* spriteName, int xOrigin, int yOrigin );
	//! @brief Sets the origins of a sprite, to the x and y coordinates, relative to the bottom left corner of the sprite. A sprite's origin does not have to be inside the sprite itself and can be positioned anywhere.
	//! @snippet PlayExamples.cpp SetSpriteOriginID	
	//! @ingroup graphics
	//! @param spriteId The id of the sprite you want to set the origin of. 
	//! @param xOrigin Where you want to move the origin on the x axis, relative to the left of the sprite.
	//! @param yOrigin Where you want to move the origin on the y axis, relative to the bottom of the sprite.
	inline void SetSpriteOrigin( int spriteId, int xOrigin, int yOrigin ) { Play::Graphics::SetSpriteOrigin( spriteId, { xOrigin, yOrigin } ); }
	//! @brief Gets the origin of the first sprite found matching the given name. This value is relative to the bottom left corner of the sprite.
	//! @snippet PlayExamples.cpp GetSpriteOrigin	
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to get the origin of.
	inline Point2D GetSpriteOrigin( const char* spriteName ) { return Play::Graphics::GetSpriteOrigin( GetSpriteId( spriteName ) ); }
	//! @brief Gets the origin of the sprite with the given ID. This value is relative to the bottom left corner of the sprite.
	//! @snippet PlayExamples.cpp GetSpriteOriginID		
	//! @ingroup graphics
	//! @param spriteId The ID of the sprite you want to get the origin of.
	//! @return The origin of the sprite, relative to it's bottom left corner.
	inline Point2D GetSpriteOrigin( int spriteId ) { return Play::Graphics::GetSpriteOrigin( spriteId ); }
	//! @brief Gets a pointer to the raw pixel data of the sprite. The pixel data is read-only, and cannot be edited.
	//! @snippet PlayExamples.cpp GetSpritePixelData	
	//! @ingroup graphics
	//! @param spriteId The ID of the sprite you want to get the pixel data of.
	//! @return A pointer to the raw pixel data. Read-only.
	inline const PixelData* GetSpritePixelData( int spriteId ) { return Play::Graphics::GetSpritePixelData( spriteId ); }

	//! @brief Set the blend mode for all subsequent drawing operations that support different blend modes.
	//! @snippet PlayExamples.cpp SetDrawingBlendMode
	//! @ingroup graphics
	//! @param blendMode The blend mode that you want to draw things with.
	inline void SetDrawingBlendMode( BlendMode blendMode ) { Graphics::SetBlendMode( static_cast<Graphics::BlendMode>(blendMode) ); }
	//! @brief Draws the first matching sprite whose filename contains the given text.
	//! @snippet PlayExamples.cpp DrawSprite
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frameIndex When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	inline void DrawSprite( const char* spriteName, Point2D pos, int frameIndex ) { Play::Graphics::Draw( Play::Graphics::GetSpriteId( spriteName ), TRANSFORM_SPACE( pos ), frameIndex ); }
	//! @brief Draws the sprite with the matching sprite ID. Using this is more efficient than drawing it using the sprite name.
	//! @snippet PlayExamples.cpp DrawSpriteID
	//! @ingroup graphics
	//! @param spriteID The ID of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frameIndex When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	inline void DrawSprite( int spriteID, Point2D pos, int frameIndex ) { Play::Graphics::Draw( spriteID, TRANSFORM_SPACE( pos ), frameIndex ); }
	//! @brief Draws the first matching sprite whose filename contains the given text, using transparency. This is slower than DrawSprite and should only be used if you need transparency.
	//! @snippet PlayExamples.cpp DrawSpriteTransparent
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all).
	//! @param colour The colour tint of the sprite.
	void DrawSpriteTransparent( const char* spriteName, Point2D pos, int frame, float opacity, Colour colour = cWhite );
	//! @brief Draws the sprite with the matching sprite ID, using transparency. This is slower than DrawSprite and should only be used if you need transparency.
	//! @snippet PlayExamples.cpp DrawSpriteTransparentID
	//! @ingroup graphics
	//! @param spriteID The ID of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all).
	//! @param colour The colour tint of the sprite. Defaults to white.
	void DrawSpriteTransparent( int spriteID, Point2D pos, int frame, float opacity, Colour colour = cWhite );
	//! @brief Draws the first matching sprite whose filename contains the given text, using the specified angle, scale, and opacity. Note that this is the slowest sprite draw function and so should only be used when you need rotation or scale.
	//! @snippet PlayExamples.cpp DrawSpriteRotated
	//! @ingroup graphics
	//! @param spriteName The name of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param angle Angle in radians to rotate the sprite clockwise.
	//! @param scale Amount to scale the sprite, with 1.0f being full size, 0.5f half size, 2.0f double sized and so on. Defaults to 1.0f (normal scale).
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all). Defaults to 1.0f (completely opaque).
	//! @param colour The colour tint of the sprite. Defaults to white.
	void DrawSpriteRotated( const char* spriteName, Point2D pos, int frame, float angle, float scale = 1.0f, float opacity = 1.0f, Colour colour = cWhite );
	//! @brief Draws the sprite with the matching sprite ID, using the specified angle, scale, and opacity. Note that this is the slowest sprite draw function and so should only be used when you need rotation or scale.
	//! @snippet PlayExamples.cpp DrawSpriteRotatedID
	//! @ingroup graphics
	//! @param spriteID The ID of the sprite you want to draw. 
	//! @param pos The x/y position on the display you want to draw the sprite. Specifically, the point where the origin of the sprite will be drawn.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param angle Angle in radians to rotate the sprite clockwise.
	//! @param scale Amount to scale the sprite, with 1.0f being full size, 0.5f half size, 2.0f double sized and so on. Defaults to 1.0f (normal scale).
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all). Defaults to 1.0f (completely opaque).
	//! @param colour The colour tint of the sprite. Defaults to white.
	void DrawSpriteRotated( int spriteID, Point2D pos, int frame, float angle, float scale = 1.0f, float opacity = 1.0f, Colour colour = cWhite );
	//! @brief Draws the sprite with the matching sprite ID, using a transformation matrix. This can be slower or faster depending on the contents of the matrix.
	//! @snippet PlayExamples.cpp DrawSpriteTransformed
	//! @ingroup graphics
	//! @param spriteID The ID of the sprite you want to draw.
	//! @param transform The transformation matrix that you want to use to draw the sprite with.
	//! @param frame When sprites consist of multiple frames the frame index determines which frame is drawn, starting at frame 0. Where a sprite has only one frame, this argument has no effect.
	//! @param opacity Controls how transparent the sprite should be. 0 is completely transparent and 1 is fully opaque (unable to see through it at all). Defaults to 1.0f (completely opaque).
	//! @param colour The colour tint of the sprite. Defaults to white.
	void DrawSpriteTransformed( int spriteID, const Matrix2D& transform, int frame, float opacity = 1.0f, Colour colour = cWhite );
	//! @brief Draws a single-pixel wide line between two points in the given colour.
	//! @snippet PlayExamples.cpp DrawLine
	//! @ingroup graphics
	//! @param start The x/y coordinate for the start point of the line.
	//! @param end The x/y coordinate for the end point of the line.
	//! @param col The colour of the line.
	void DrawLine( Point2D start, Point2D end, Colour col );
	//! @brief Draws a single-pixel wide circle at a given origin.
	//! @snippet PlayExamples.cpp DrawCircle
	//! @ingroup graphics
	//! @param pos The x/y coordinate for the origin of the circle.
	//! @param radius The length of the circle's radius in pixels.
	//! @param col The colour of the circle.
	void DrawCircle( Point2D pos, int radius, Colour col );
	//! @brief Draws a rectangle, defined by the bottom left and top right corners, in the given colour.
	//! @snippet PlayExamples.cpp DrawRect
	//! @ingroup graphics
	//! @param bottomLeft The x/y coordinate for the bottom left corner.
	//! @param topRight The x/y coordinate for the top right corner.
	//! @param col The colour of the rectangle.
	//! @param fill Is the rectangle filled in? Defaults to not filled in.
	void DrawRect( Point2D bottomLeft, Point2D topRight, Colour col, bool fill = false );
	//! @brief Draws a line between two points using a sprite as a 'pen', blended with the given colour. Note that colouring affects subsequent DrawSprite calls using the same sprite!
	//! @snippet PlayExamples.cpp DrawSpriteLine
	//! @ingroup graphics
	//! @param startPos The x/y coordinate for the start point of the line.
	//! @param endPos The x/y coordinate for the end point of the line.
 	//! @param penSprite The sprite to use as the 'pen'.
	//! @param col The colour of the sprites drawing the line.
	void DrawSpriteLine( Point2D startPos, Point2D endPos, const char* penSprite, Colour col = cWhite );
	//! @brief Draws a circle using a sprite as a 'pen', blended with the given colour. Note that colouring affects subsequent DrawSprite calls using the same sprite!
	//! @snippet PlayExamples.cpp DrawSpriteCircle
	//! @ingroup graphics
	//! @param pos The x/y coordinate for the origin of the circle.
	//! @param radius The length of the circle's radius in pixels.
	//! @param penSprite The sprite to use as the 'pen'.
	//! @param col The colour of the sprites drawing the circle.
	void DrawSpriteCircle( Point2D pos, int radius, const char* penSprite, Colour col = cWhite );
	//! @brief Draws text using a sprite-based font exported from PlayFontTool.
	//! @snippet PlayExamples.cpp DrawFontText
	//! @ingroup graphics
	//! @param fontId The unique sprite id of the font you want to use.
	//! @param text The string containing the text you want to draw.
	//! @param pos The x/y coordinate for the location for text to be drawn at.
	//! @param justify Optional argument determining whether the text is left, right, or centre justified (defaults to left justified).
	void DrawFontText( const char* fontId, std::string text, Point2D pos, Align justify = Align::LEFT );
	//! @brief Draws a single pixel on screen.
	//! @snippet PlayExamples.cpp DrawPixel
	//! @ingroup graphics
	//! @param pos The x/y coordinate of the pixel you wish to draw.
	//! @param col The colour of the pixel.
	void DrawPixel( Point2D pos, Colour col );

	//! @brief Resets the timing bar data and sets the current timing bar segment to a specific colour
	//! @snippet PlayExamples.cpp BeginTimingBar
	//! @ingroup graphics
	//! @param colour Colour of the timing bar segment.
	void BeginTimingBar( Colour colour );
	// Sets the current timing bar segment to a specific colour
	//! @snippet PlayExamples.cpp ColourTimingBar
	//! @ingroup graphics
	//! @param colour Colour of the timing bar segment.
	//! @return The number of timing segments
	int ColourTimingBar( Colour colour );
	//! @brief Draws the timing bar for the previous frame at the given position and size
	//! @snippet PlayExamples.cpp DrawTimingBar
	//! @ingroup graphics
	//! @param pos The x/y coordinate you want to draw the timing bar at.
	//! @param size The size of the timing bar in pixels.
	inline void DrawTimingBar( Point2f pos, Point2f size ) { Play::Graphics::DrawTimingBar( pos, size ); }

	// Miscellaneous functions
	//**************************************************************************************************

	//! @brief Returns true if the key has been pressed since it was last released.
	//! @snippet PlayExamples.cpp KeyPressed
	//! @ingroup input
	//! @param key The key that you want to check for.
	//! @return If the key has been pressed. This will return true on the frame the key is pushed down, but after that point will return false, even if held.
	inline bool KeyPressed( KeyboardButton key ) { return Play::Input::KeyPress( key, frameCount ); }
	//! @brief Returns true if the key is currently pressed down.
	//! @snippet PlayExamples.cpp KeyDown
	//! @ingroup input
	//! @param key The key that you want to check for.
	//! @return If the key has been pressed. This will return true while it is down, and false if not pressed
	inline bool KeyDown( KeyboardButton key ) { return Play::Input::KeyHeld( key ); }
	//! @brief Returns a random number as if you rolled a die with this many sides.
	//! @snippet PlayExamples.cpp RandomRoll
	//! @ingroup misc
	//! @param sides How many sides the 'die' has.
	//! @return The number that was rolled.
	inline int RandomRoll( int sides ) { return (rand() % sides) + 1; }
	//! @brief Returns a random number, between the min and max parameters.
	//! @snippet PlayExamples.cpp RandomRollRange
	//! @ingroup misc
	//! @param min The smallest number that the roll can return.
	//! @param max The largest number that the roll can return.
	//! @return The number that was rolled.
	int RandomRollRange( int min, int max );
}
#endif // PLAY_PLAYMANAGER_H