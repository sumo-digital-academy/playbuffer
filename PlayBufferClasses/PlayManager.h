#ifndef PLAY_PLAYMANAGER_H
#define PLAY_PLAYMANAGER_H
//********************************************************************************************************************************
// File:		PlayManager.h
// Description:	A manager for providing simplified access to the PlayBuffer framework with managed GameObjects
// Platform:	Independent
// Notes:		The GameObject management is "opt in" as many will want to create their own GameObject class
//********************************************************************************************************************************

#ifdef PLAY_USING_GAMEOBJECT_MANAGER

#ifndef PLAY_ADD_GAMEOBJECT_MEMBERS
#define PLAY_ADD_GAMEOBJECT_MEMBERS 
#endif

#ifdef PLAY_IMPLEMENTATION

// PlayManager manges a map of GameObject structures
// > Additional member variables can be added with PLAY_ADD_GAMEOBJECT_MEMBERS 
struct GameObject
{
	GameObject( int type, Point2D pos, int collisionRadius, int spriteId );

	// Default member variables: don't change these!
	int type{ -1 };
	int spriteId{ -1 };
	Point2D pos{ 0.0f, 0.0f };
	Point2D oldPos{ 0.0f, 0.0f };
	Vector2D velocity{ 0.0f, 0.0f };
	Vector2D acceleration{ 0.0f, 0.0f };
	float rotation{ 0.0f };
	float rotSpeed{ 0.0f };
	float oldRot{ 0.0f };
	int frame{ 0 };
	float framePos{ 0.0f };
	float animSpeed{ 0.0f };
	int radius{ 0 };
	float scale{ 1 };
	PLAY_ADD_GAMEOBJECT_MEMBERS

	int GetId() { return m_id; }

private:
	// The GameObject's id should never be changed manually so we make it private!
	int m_id{ -1 };

	// Preventing assignment and copying reduces the potential for bugs
	GameObject& operator=( const GameObject& ) = delete;
	GameObject( const GameObject& ) = delete;
};
#else

struct GameObject;

#endif
#endif

namespace Play
{
	// Alignment for font drawing operations
	enum Align
	{
		LEFT = 0,
		RIGHT,
		CENTRE,
	};

	enum Direction
	{
		HORIZONTAL = 0,
		VERTICAL,
		ALL,
	};

	// PlayManager uses colour values from 0-100 for red, green, blue and alpha
	struct Colour
	{
		Colour( float r, float g, float b ) : red( r ), green( g ), blue( b ) {}
		Colour( int r, int g, int b ) : red( static_cast<float>( r ) ), green( static_cast<float>( g ) ), blue( static_cast<float>( b ) ) {}
		float red, green, blue;
	};

	extern Colour cBlack, cRed, cGreen, cBlue, cMagenta, cCyan, cYellow, cOrange, cWhite, cGrey;

	// Manager creation and deletion
	//**************************************************************************************************

	// Initialises the managers and creates a window of the required dimensions
	void CreateManager( int width, int height, int scale );
	// Shuts down the managers and closes the window
	void DestroyManager();

	// PlayWindow functions
	//**************************************************************************************************

	// Copies the contents of the drawing buffer to the window
	void PresentDrawingBuffer();
	// Gets the co-ordinates of the mouse cursor within the display buffer
	Point2D GetMousePos();
	// Gets the width of the display buffer
	int GetBufferWidth();
	// Gets the height of the display buffer
	int GetBufferHeight();

	// PlayAudio functions
	//**************************************************************************************************

	// Plays an mp3 audio file from the "Data\Sounds" directory
	void PlayAudio( const char* mp3Filename );
	// Loops an mp3 audio file from the "Data\Sounds" directory
	void StartAudioLoop( const char* mp3Filename );
	// Stops a looping mp3 audio file started with Play::StartSoundLoop()
	void StopAudioLoop( const char* mp3Filename );

	// PlayGraphics functions
	//**************************************************************************************************

	// Clears the display buffer using the colour provided
	void ClearDrawingBuffer( Colour col );
	// Loads a PNG file as the background image for the window
	int LoadBackground( const char* pngFilename );
	// Draws the background image previously loaded with Play::LoadBackground() into the drawing buffer
	void DrawBackground( int background = 0 );
	// Draws text to the screen using the built-in debug font
	void DrawDebugText( Point2D pos, const char* text, Colour col = cWhite, bool centred = true );

	// Gets the sprite id of the first matching sprite whose filename contains the given text
	int GetSpriteId( const char* spriteName );
	// Gets the pixel height of a sprite
	int GetSpriteHeight( const char* spriteName );
	// Gets the pixel width of a sprite
	int GetSpriteWidth( const char* spriteName );
	// Gets the pixel height of a sprite
	int GetSpriteHeight( int spriteId );
	// Gets the pixel width of a sprite
	int GetSpriteWidth( int spriteId );
	// Gets the stem filename for the sprite (without path or extension)
	const char* GetSpriteName( int spriteId );
	// Blends the sprite with the given colour (works best on white sprites)
	// > Note that colouring affects subsequent DrawSprite calls using the same sprite!!
	void ColourSprite( const char* spriteName, Colour col );

	// Centres the origin of the first sprite found matching the given name
	void CentreSpriteOrigin( const char* spriteName );
	// Centres the origin of all sprites found matching the given name
	void CentreMatchingSpriteOrigins( const char* partName );
	// Centres the origins of all loaded sprites
	void CentreAllSpriteOrigins();
	// Moves the origin of the first sprite found matching the given name
	void MoveSpriteOrigin( const char* spriteName, int xOffset, int yOffset );
	// Moves the origin of all sprites found matching the given name
	void MoveMatchingSpriteOrigins( const char* partName, int xoffset, int yoffset );
	// Moves the origin of all loaded sprites
	void MoveAllSpriteOrigins( int xoffset, int yoffset );
	// Sets the origin of the first sprite found matching the given name
	void SetSpriteOrigin( const char* spriteName, int xOrigin, int yOrigin );
	// Sets the origin of the sprite with a specific ID
	void SetSpriteOrigin( int spriteId, int xOrigin, int yOrigin );
	// Gets the origin of the first sprite found matching the given name
	Point2D GetSpriteOrigin( const char* spriteName );
	// Gets the origin of the sprite with a specific ID
	Point2D GetSpriteOrigin( int spriteId );

	// Draws the first matching sprite whose filename contains the given text
	void DrawSprite( const char* spriteName, Point2D pos, int frameIndex );
	// Draws the sprite using its unique sprite ID
	void DrawSprite( int spriteID, Point2D pos, int frame );
	// Draws the sprite with transparency (slower than DrawSprite)
	void DrawSpriteTransparent( const char* spriteName, Point2D pos, int frame, float opacity );
	// Draws the sprite with transparency (slower than DrawSprite)
	void DrawSpriteTransparent( int spriteID, Point2D pos, int frame, float opacity );
	// Draws the sprite with rotation and transparency (slowest DrawSprite)
	void DrawSpriteRotated( const char* spriteName, Point2D pos, int frame, float angle, float scale = 1.0f, float opacity = 1.0f );
	// Draws the sprite with rotation and transparency (slowest DrawSprite)
	void DrawSpriteRotated( int spriteID, Point2D pos, int frame, float angle, float scale, float opacity = 1.0f );
	// Draws a single-pixel wide line between two points in the given colour
	void DrawLine( Point2D start, Point2D end, Colour col );
	// Draws a single-pixel wide circle in the given colour
	void DrawCircle( Point2D pos, int radius, Colour col );
	// Draws a line between two points using a sprite
	// > Note that colouring affects subsequent DrawSprite calls using the same sprite!!
	void DrawSpriteLine( Point2D startPos, Point2D endPos, const char* penSprite, Colour c = cWhite );
	// Draws a circle using a sprite
	// > Note that colouring affects subsequent DrawSprite calls using the same sprite!!
	void DrawSpriteCircle( int x, int y, int radius, const char* penSprite, Colour c = cWhite );
	// Draws text using a sprite-based font exported from PlayFontTool
	void DrawFontText( const char* fontId, std::string text, Point2D pos, Align justify = LEFT );

	// GameObject functions
	//**************************************************************************************************

#ifdef PLAY_USING_GAMEOBJECT_MANAGER

	// Creates a new GameObject and adds it to the managed list.
	// > Returns the new object's unique id
	int CreateGameObject( int type, Point2D pos, int collisionRadius, const char* spriteName );
	// Retrieves a GameObject based on its id
	// > Returns an object with a type of -1 if no object can be found
	GameObject& GetGameObject( int id );
	// Retrieves the first GameObject matching the given type
	// > Returns an object with a type of -1 if no object can be found
	GameObject& GetGameObjectByType( int type );
	// Collects the IDs of all of the GameObjects with the matching type
	std::vector<int> CollectGameObjectIDsByType( int type );
	// Collects the IDs of all of the GameObjects
	std::vector<int> CollectAllGameObjectIDs();
	// Performs a typical update of the object's position and animation
	void UpdateGameObject( GameObject& object );
	// Deletes the GameObject with the corresponding id
	//> Use GameObject.GetId() to find out its unique id
	void DestroyGameObject( int id );
	// Deletes all GameObjects with the corresponding type
	void DestroyGameObjectsByType( int type );
	
	// Checks whether the two objects are within each other's collision radii
	bool IsColliding( GameObject& obj1, GameObject& obj2 );
	// Checks whether any part of the object is visible within the DisplayBuffer
	bool IsVisible( GameObject& obj );
	// Checks whether the object is overlapping the edge of the screen and moving outwards 
	bool IsLeavingDisplayArea( GameObject& obj, Direction dirn = ALL );
	// Checks whether the animation has completed playing
	bool IsAnimationComplete( GameObject& obj );

	// Sets the velocity of the object based on a target rotation angle
	void SetGameObjectDirection( GameObject& obj, int speed, float rotation );
	// Set the velocity of the object based on a target point
	void PointGameObject( GameObject& obj, int speed, int targetX, int targetY );

	// Changes the object's current spite and resets its animation frame to the start
	void SetSprite( GameObject& obj, const char* spriteName, float animSpeed );
	// Draws the object's sprite without rotation or transparency (fastest)
	void DrawObject( GameObject& obj );
	// Draws the object's sprite with transparency (slower than DrawObject)
	void DrawObjectTransparent( GameObject& obj, float opacity );
	// Draws the object's sprite with rotation and transparency (slower than DrawObject)
	void DrawObjectRotated( GameObject& obj, float opacity = 1.0f );

#endif

	// Miscellaneous functions
	//**************************************************************************************************

	// Returns true if the key has been pressed since it was last released
	// > https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	bool KeyPressed( int vKey );
	// Returns true if the key is currently being held down
	// > https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	bool KeyDown( int vKey );

	// Returns a random number as if you rolled a die with this many sides
	int RandomRoll( int sides );
	// Returns a random number from min to max inclusive
	int RandomRollRange( int min, int max );

	// Converts radians to degrees
	constexpr float RadToDeg( float radians )
	{
		return ( radians / PLAY_PI ) * 180.0f;
	}

	// Converts radians to degrees
	constexpr float DegToRad( float degrees )
	{
		return ( degrees / 180.0f ) * PLAY_PI;
	}

}

#endif


