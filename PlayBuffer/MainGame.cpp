#include "PlayBufferPCH.h"

using namespace Play; // Don't ever do this in a header file!

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int MINI_WIDTH = 240;
constexpr int MINI_HEIGHT = 144;
constexpr int DISPLAY_SCALE = 1;

// Function declarations
void DrawSprite( int spriteId, Point2f pos, int frame, BlendColour blend, float rot, float scale, bool transparent, bool rotated );
void DrawTimingData( float yPos, float elapsedTime );
void DrawMiniScreen( Point2f pos, float elapsedTime );
void DrawSpiders( float elapsedTime );
bool UnitTests();

PixelData g_miniScreen;
bool g_loopingMusicOn = false;

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	// The graphics object creates a drawing buffer and loads any sprites in the directory provided
	Graphics::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, "Data\\Sprites\\" );
	Graphics::LoadBackground( "Data\\Backgrounds\\background.png" );
	Graphics::CentreAllSpriteOrigins();
	Graphics::SetSpriteOrigin( Graphics::GetSpriteId( "dead" ), { 50, 0 }, true );

	// The window object manages the window for us using the drawing buffer from the graphics object
	Window::CreateManager( Graphics::GetDrawingBuffer(), DISPLAY_SCALE );

	// The audio manager handles WAV sounds and music
	Audio::CreateManager( "Data\\Audio\\" ); 


	// The input manager handles mouse and keyboard input
	Input::CreateManager();

	// We allocate our own memory for a separate mini drawing buffer
	g_miniScreen = { MINI_WIDTH, MINI_HEIGHT, new Pixel[MINI_WIDTH * MINI_HEIGHT] };
	// Once we've added this as a sprite its buffer will be freed up automatically by PlayGraphics
	Graphics::AddSprite( "MINI SCREEN", g_miniScreen );
}

// Called by the PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	constexpr float TIMING_YPOS = 700; // Vertical position of the timing bar
	const Point2f MINI_POS( 1008, 532 ); // Position of the mini screen 
	static Graphics::BlendMode blendMode = Graphics::BLEND_NORMAL;
	static int walkSoundId = Audio::StartSound( "walk", true );

	static int spr_font = Graphics::GetSpriteId( "font" );

	// Reset the timing bar data and use green for the background image draw
	Graphics::TimingBarBegin( PIX_GREEN );
	Graphics::DrawBackground();

	// Switch the timing bar colour to red for drawing the spider sprites
	Graphics::SetTimingBarColour( PIX_RED );
	DrawSpiders( elapsedTime );

	// Switch the timing bar colour to blue for drawing the mini screen
	Graphics::SetTimingBarColour( PIX_BLUE );
	DrawMiniScreen( MINI_POS, elapsedTime );

	// Switch the timing bar colour to orange for drawing the user interface
	Graphics::SetTimingBarColour( PIX_ORANGE );
	Graphics::DrawStringCentred(spr_font, { 640, 60 }, "Press 'T', 'R' or 'S' to toggle transparency, rotation or mini-screen.");
	Graphics::DrawStringCentred(spr_font, { 640, 22 }, "Press 'N', 'A' or 'M' to toggle blending between normal, additive or multiply.");
	DrawTimingData( TIMING_YPOS, elapsedTime );

	// Switch the timing bar colour to white for copying the drawing buffer to the window
	Graphics::SetTimingBarColour( PIX_WHITE );
	Window::Present();

	// Switch the timing bar colour to black for any waiting time until the next frame
	Graphics::SetTimingBarColour( PIX_BLACK );

	// Toggle the blend modes
	if (Input::KeyPress( KEY_A ))
		blendMode = Graphics::BLEND_ADD;

	if (Input::KeyPress( KEY_N ))
		blendMode = Graphics::BLEND_NORMAL;

	if (Input::KeyPress( KEY_M ))
		blendMode = Graphics::BLEND_MULTIPLY;

	Graphics::SetBlendMode(blendMode);

	if( Input::KeyPress( KEY_1 ) )
		Audio::StartSound( "boing", false, 1.0f, 0.75f + (rand() % 125) / 50.0f );

	if( Input::KeyPress( KEY_2 ) )
		Audio::StartSound( "explode", false, 1.0f,  0.5f + (rand() % 150) / 50.0f );

	if( Input::KeyPress( KEY_3 ) )
		Audio::StartSound( "error", false, 1.0f, 0.5f + (rand() % 150) / 50.0f );

	if( Input::KeyPress( KEY_4 ) )
	{
		if (g_loopingMusicOn)
		{
			Audio::StopSound("music");
			g_loopingMusicOn = false;
		}
		else
		{
			Audio::StartSound( "music", true, 1.0f );
			g_loopingMusicOn = true;
		}
	}

	if( Input::KeyPress( KEY_0 ) )
		Audio::StopSound( walkSoundId );
	
	// Return true to quit when escape key is pressed
	return Input::KeyPress( KEY_ESCAPE  );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Graphics::DestroyManager();
	Audio::DestroyManager();
	Window::DestroyManager();
	Input::DestroyManager();

	return PLAY_OK;
}

// Draws the squads of spiders
// > Supports toggling for transparancy and rotation to demonstrate the speed difference
void DrawSpiders( float elapsedTime )
{
	constexpr int ROWS = 5; // Number of rows of spiders in each squad
	constexpr int COLUMNS = 11; // Number of columns of spiders in each squad
	constexpr int ROW_DIST = 110; // Vertical separation between spiders
	constexpr int COLUMN_DIST = 128; // Horizontal separation between spiders

	static Vector2D rot_spid_pos = { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 };
	static Point2f squad1_pos = { -1400, 125 };
	static Point2f squad2_pos = { 1400, 180 };
	static float rot_angle = 0.0f;
	static float r, g, b;

	static bool transparent = false;
	static bool rotated = false;

	static float lifetime = 0;
	lifetime += elapsedTime;
	int frameIndex = static_cast<int>(( lifetime * FRAMES_PER_SECOND ) / 4);

	r = abs(sin(lifetime));
	g = abs(cos(lifetime));
	b = abs(-sin(lifetime));

	// We use the sprite names to pull the ids out for use later
	static int spr_walk_left = Graphics::GetSpriteId( "agent8_left" );
	static int spr_walk_right = Graphics::GetSpriteId( "agent8_right" );
	static int spr_collideA = Graphics::GetSpriteId( "collideA" );
	static int spr_collideB = Graphics::GetSpriteId( "collideB" );
	static bool collision = false;

	// Let the player control the vertical position of squad1 using the keyboard
	if( Input::KeyHeld( KEY_UP ) )
		squad1_pos.y += 3;

	if( Input::KeyHeld( KEY_DOWN ) )
		squad1_pos.y -= 3;

	// Toggle the transparent and rotated drawing modes
	if( Input::KeyPress( KEY_T ) )
		transparent = !transparent;

	if (Input::KeyPress( KEY_R ))
	{
		rotated = !rotated;
		rot_angle = 0;
		rot_spid_pos = { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 };
	}

	if( Input::KeyHeld( KEY_Q ) )
		rot_angle += 0.01f;

	if( Input::KeyHeld( KEY_W ) )
		rot_angle -= 0.01f;

	if( Input::KeyPress( KEY_C ) )
		collision = !collision;

	if( Input::KeyHeld( KEY_RIGHT ) )
		rot_spid_pos.x += 1.0f;

	if( Input::KeyHeld( KEY_LEFT ) )
		rot_spid_pos.x -= 1.0f;

	if( Input::KeyHeld( KEY_UP ) )
		rot_spid_pos.y += 1.0f;

	if( Input::KeyHeld( KEY_DOWN ) )
		rot_spid_pos.y -= 1.0f;

	// Update the positions of the squads
	squad1_pos += { 3, 0 };
	squad2_pos -= { 3, 0 };
	rot_angle += 0.01f;

	// Wrap the positions around the edge of the screen
	if( squad1_pos.x > 1400 ) 
	{
		squad1_pos.x = -1400; 
		squad2_pos.x = 1400; 
	}

	if( collision )
	{

		// Draw a rotating sprite with axes.
		Matrix2D m1 = MatrixScale( 1.5f + cos( rot_angle ), 1.5f + cos( rot_angle ) ) * MatrixRotation( rot_angle ) * MatrixTranslation( rot_spid_pos.x, sin( rot_angle ) * (rot_spid_pos.y / 2) + DISPLAY_HEIGHT / 2 );
		Matrix2D m2 = MatrixScale( 1.5f + sin( rot_angle ), 1.5f + sin( rot_angle ) ) * MatrixRotation( -rot_angle ) * MatrixTranslation( rot_spid_pos.x, sin( rot_angle ) * (rot_spid_pos.y / 2) + DISPLAY_HEIGHT / 2 );

		DrawLine( m1.row[ 2 ].As2D(), m1.row[ 2 ].As2D() + m1.row[ 0 ].As2D() * 50.0f, cRed );
		DrawLine( m1.row[ 2 ].As2D(), m1.row[ 2 ].As2D() + m1.row[ 1 ].As2D() * 50.0f, cBlue );

		int collidingPixels = Graphics::SpriteCollide( spr_collideA, frameIndex, m1, spr_collideB, frameIndex, m2 );
		DrawSpriteTransformed( spr_collideB, m2, frameIndex, 1.0f );
		DrawSpriteTransformed( spr_collideA, m1, frameIndex, 1.0f );

		Graphics::UpdateSprite( "collideA" );
		Graphics::UpdateSprite( "collideB" );

		Graphics::DrawStringCentred( Graphics::GetSpriteId( "font" ), { 640, 500 }, std::to_string( collidingPixels ) );

		DrawLine( { (DISPLAY_WIDTH / 2) - 50, (DISPLAY_HEIGHT / 2) }, { (DISPLAY_WIDTH / 2) + 50, (DISPLAY_HEIGHT / 2) }, Play::cRed );
		DrawLine( { (DISPLAY_WIDTH / 2), (DISPLAY_HEIGHT / 2) - 50 }, { (DISPLAY_WIDTH / 2), (DISPLAY_HEIGHT / 2) + 50 }, Play::cRed );

	}
	else
	{
		// Draw the many rows and columns of spiders
		for( int row = 0; row < ROWS; row++ )
		{
			for( int column = 0; column < COLUMNS; column++ )
				DrawSprite( spr_walk_right, squad1_pos + Vector2f( column * COLUMN_DIST, row * ROW_DIST ), frameIndex, { 0.1f * column, r, g, b }, rot_angle, sin( rot_angle * 5 ), transparent, rotated );

			for( int column = 0; column < COLUMNS; column++ )
				DrawSprite( spr_walk_left, squad2_pos + Vector2f( column * COLUMN_DIST, row * ROW_DIST ), frameIndex, { 0.1f * column, r, g, b }, rot_angle, cos( rot_angle * 5 ), transparent, rotated );
		}
	}
	
}

// Draws a sprite using the specified method
void DrawSprite( int spriteId, Point2f pos, int frame, BlendColour globalMultiply, float rot, float scale, bool transparent, bool rotated )
{
	if( !rotated )
	{
		if( !transparent )
			Graphics::Draw( spriteId, pos, frame ); // Fastest method
		else
			Graphics::DrawTransparent( spriteId, pos, frame, globalMultiply); // Slightly slower
	}
	else
	{
		if( !transparent )
			Graphics::DrawRotated( spriteId, pos, frame, rot, 0.75f + (scale*0.25f), globalMultiply); // A lot slower
		else
			Graphics::DrawRotated( spriteId, pos, frame, rot, 1.0f, globalMultiply); // Not much slower
	}

}

// Draws the timing bar and key 
void DrawTimingData( float yPos, float elapsedTime )
{
	constexpr int ITEM_WIDTH = 96; // Width of each item in the key
	constexpr int ITEM_HEIGHT = 14; // Height of each item in the key
	constexpr int LABEL_X = 720; // Horizontal origin of the key
	constexpr int TIME_BAR_WIDTH = 192; 
	constexpr int TIME_BAR_HEIGHT = 10;

	std::vector< std::string > labels = { "BACKGROUND", "SPIDERS", "MINI", "INTERFACE", "PRESENT", "WAIT" };
	std::vector< Pixel> bgColours = { PIX_GREEN, PIX_RED, PIX_BLUE, PIX_ORANGE, PIX_WHITE, PIX_BLACK };
	std::vector< Pixel> textColours = { PIX_WHITE, PIX_WHITE, PIX_WHITE, PIX_BLACK, PIX_BLACK, PIX_WHITE };
	Point2f pos = { LABEL_X, yPos };

	int fps = static_cast<int>( round( ( 1.0f / elapsedTime ) ) );
	Graphics::DrawDebugString( { ITEM_WIDTH / 2, pos.y + 1 }, std::to_string( fps ).c_str() + std::string( " FPS" ), PIX_WHITE, true );
	Graphics::DrawTimingBar( { ITEM_WIDTH, pos.y - 5 }, { TIME_BAR_WIDTH, TIME_BAR_HEIGHT } );

	for( size_t i = 0; i < labels.size(); i++ )
	{
		Graphics::DrawRect( { pos.x - ITEM_WIDTH / 2, pos.y - ITEM_HEIGHT / 2 }, { pos.x + ITEM_WIDTH / 2, pos.y + ITEM_HEIGHT / 2 }, bgColours[i], true );
		Graphics::DrawDebugString( pos, labels[i], textColours[i] );
		pos.x += ITEM_WIDTH;
	}
}

// Draws the mini screen using a separate render target 
void DrawMiniScreen( Point2f pos, float elapsedTime )
{
	static int spr_dead = Graphics::GetSpriteId( "agent8_dead" );
	static int spr_star = Graphics::GetSpriteId( "star" );
	static bool mini_screen = true;

	static float lifetime = 0;
	lifetime += elapsedTime;
	int frameIndex = static_cast<int>( ( lifetime * FRAMES_PER_SECOND ) / 20);

	static float rot_angle = 0.0f;
	static std::vector< Point2f > stars = { { rand(), rand() }, { rand(), rand() }, { rand(), rand() }, { rand(), rand() } };
	rot_angle += 0.01f;

	if( mini_screen )
	{
		Graphics::SetRenderTarget( &g_miniScreen ); // Subsequent graphics operations act on the mini screen
		Graphics::ClearBuffer( Pixel( 0x00, 0x00, 0x4F ) );
		for( Point2f& starPos : stars )
		{
			starPos -= 10 * Point2f( cos( rot_angle ), sin( rot_angle ) );
			starPos += { MINI_WIDTH, MINI_HEIGHT };
			starPos.x = fmodf( starPos.x, MINI_WIDTH );
			starPos.y = fmodf( starPos.y, MINI_HEIGHT );
			Graphics::Draw( spr_star, starPos, frameIndex );
		}
		Graphics::DrawRotated( spr_dead, { MINI_WIDTH / 2, MINI_HEIGHT / 2 }, frameIndex, rot_angle );
		Graphics::UpdateSprite( "MINI SCREEN", g_miniScreen ); // Regenerates the pre-multiplied alpha buffer
		Graphics::SetRenderTarget( Graphics::GetDrawingBuffer() ); // Subsequent graphics operations revert to the main drawing buffer
		Graphics::DrawTransparent(Graphics::GetSpriteId("MINI SCREEN"), pos, 0, { 0.8f, 0.8f, 0.8f, 0.8f } ); // Draw the mini screen to the main drawing buffer
		Graphics::DrawRect( pos, { pos.x + MINI_WIDTH, pos.y + MINI_HEIGHT }, PIX_WHITE );
	}

	if( Input::KeyPress( KEY_S ) )
		mini_screen = !mini_screen;
}



