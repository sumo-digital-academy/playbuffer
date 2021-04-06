#include "PlayBufferPCH.h"

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int MINI_WIDTH = 240;
constexpr int MINI_HEIGHT = 144;
constexpr int DISPLAY_SCALE = 1;

// Function declarations
void DrawSprite( int spriteId, Point2f pos, int frame, float opacity, float rot, bool transparent, bool rotated );
void DrawTimingData( float yPos, float elapsedTime );
void DrawMiniScreen( Point2f pos, float elapsedTime );
void DrawSpiders( float elapsedTime );

PixelData g_miniScreen;

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	// The graphics object creates a drawing buffer and loads any sprites in the directory provided
	PlayGraphics& graphics = PlayGraphics::Instance( DISPLAY_WIDTH, DISPLAY_HEIGHT, "Data\\Sprites\\" );
	graphics.LoadBackground( "Data\\Backgrounds\\background.png" ); 
	graphics.CentreAllSpriteOrigins();
	graphics.SetSpriteOrigin( graphics.GetSpriteId( "dead" ), { 0, -50 }, true );

	// The window object manages the window for us using the drawing buffer from the graphics object
	PlayWindow::Instance( PlayGraphics::Instance().GetDrawingBuffer(), DISPLAY_SCALE );

	// The audio object manages mp3 sounds and music
	PlayAudio& audio = PlayAudio::Instance( "Data\\Audio\\" ); 
	audio.StartAudio( "walk", false ); 

	// We allocate our own memory for a separate mini drawing buffer
	g_miniScreen = { MINI_WIDTH, MINI_HEIGHT, new Pixel[MINI_WIDTH * MINI_HEIGHT] };
}

// Called by the PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	constexpr float TIMING_YPOS = 20; // Vertical position of the timing bar
	const Point2f MINI_POS( 1008, 48 ); // Position of the mini screen 

	PlayGraphics& graphics = PlayGraphics::Instance();

	static int spr_font = graphics.GetSpriteId( "font36px" );

	// Reset the timing bar data and use green for the background image draw
	graphics.TimingBarBegin( PIX_GREEN );
	graphics.DrawBackground();

	// Switch the timing bar colour to red for drawing the spider sprites
	graphics.SetTimingBarColour( PIX_RED );
	DrawSpiders( elapsedTime );

	// Switch the timing bar colour to blue for drawing the mini screen
	graphics.SetTimingBarColour( PIX_BLUE );
	DrawMiniScreen( MINI_POS, elapsedTime );

	// Switch the timing bar colour to orange for drawing the user interface
	graphics.SetTimingBarColour( PIX_ORANGE );
	graphics.DrawStringCentred( spr_font, { 640, 670 }, "Press 'T', 'R' or 'M' to toggle transparency, rotation or mini-screen" );
	DrawTimingData( TIMING_YPOS, elapsedTime );

	// Switch the timing bar colour to white for copying the drawing buffer to the window
	graphics.SetTimingBarColour( PIX_WHITE );
	PlayWindow::Instance().Present();

	// Switch the timing bar colour to black for any waiting time until the next frame
	graphics.SetTimingBarColour( PIX_BLACK );
	
	// Return true to quit when escape key is pressed
	return PlayInput::Instance().KeyPressed( VK_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	// It's important to free up any memory we allocated
	delete g_miniScreen.pPixels;

	PlayGraphics::Destroy();
	PlayAudio::Destroy();
	PlayWindow::Destroy();
	PlayInput::Destroy();

	return PLAY_OK;
}

// Draws the squads of spiders
// > Supports toggling for transparancy and rotation to demonstrate the speed difference
void DrawSpiders( float elapsedTime )
{
	constexpr int ROWS = 5; // Number of rows of spiders in each squad
	constexpr int COLUMNS = 10; // Number of columns of spiders in each squad
	constexpr int ROW_DIST = 110; // Vertical separation between spiders
	constexpr int COLUMN_DIST = 128; // Horizontal separation between spiders

	static Point2f squad1_pos = { -1400, 100 };
	static Point2f squad2_pos = { 1400, 155 };
	static float rot_angle = 0.0f;

	static bool transparent = false;
	static bool rotated = false;

	static float lifetime = 0;
	lifetime += elapsedTime;
	int frameIndex = static_cast<int>(( lifetime * FRAMES_PER_SECOND ) / 4);

	PlayGraphics& graphics = PlayGraphics::Instance();

	// We use the sprite names to pull the ids out for use later
	static int spr_walk_left = graphics.GetSpriteId( "agent8_left" );
	static int spr_walk_right = graphics.GetSpriteId( "agent8_right" );

	// Let the player control the vertical position of squad1 using the keyboard
	if( PlayInput::Instance().KeyDown( VK_UP ) )
		squad1_pos.y -= 3;

	if( PlayInput::Instance().KeyDown( VK_DOWN ) )
		squad1_pos.y += 3;

	// Toggle the transparent and rotated drawing modes
	if( PlayInput::Instance().KeyPressed( 'T' ) )
		transparent = !transparent;

	if( PlayInput::Instance().KeyPressed( 'R' ) )
		rotated = !rotated;

	// Update the positions of the squads
	squad1_pos += { 3, 0 };
	squad2_pos -= { 3, 0 };
	rot_angle += 0.01f;

	// Wrap the positions around the edge of the screen
	if( squad1_pos.x > 1400 ) 
	{
		squad1_pos.x = -1400; 
		squad2_pos.x = 1400; 
		PlayAudio::Instance().StartAudio( "walk", false );
	}

	// Draw the many rows and columns of spiders
	for( int row = 0; row < ROWS; row++ )
	{
		for( int column = 0; column < COLUMNS; column++ )
			DrawSprite( spr_walk_right, squad1_pos + Vector2f( column * COLUMN_DIST, row * ROW_DIST ), frameIndex, 0.1f * column, rot_angle, transparent, rotated );

		for( int column = 0; column < COLUMNS; column++ )
			DrawSprite( spr_walk_left, squad2_pos + Vector2f( column * COLUMN_DIST, row * ROW_DIST ), frameIndex, 0.1f * column, rot_angle, transparent, rotated );
	}
}


// Draws a sprite using the specified method
void DrawSprite( int spriteId, Point2f pos, int frame, float opacity, float rot, bool transparent, bool rotated )
{
	PlayGraphics& graphics = PlayGraphics::Instance();

	if( !rotated )
	{
		if( !transparent )
			graphics.Draw( spriteId, pos, frame ); // Fastest method
		else
			graphics.DrawTransparent( spriteId, pos, frame, opacity ); // Slightly slower
	}
	else
	{
		if( !transparent )
			graphics.DrawRotated( spriteId, pos, frame, rot, 1.0f, 1.0f ); // A lot slower
		else
			graphics.DrawRotated( spriteId, pos, frame, rot, 1.0f, opacity ); // Not much slower
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

	PlayGraphics& graphics = PlayGraphics::Instance();

	int fps = static_cast<int>( round( ( 1.0f / elapsedTime ) ) );
	graphics.DrawDebugString( { ITEM_WIDTH / 2, pos.y + 1 }, std::to_string( fps ).c_str() + std::string( " FPS" ), PIX_WHITE, true );
	graphics.DrawTimingBar( { ITEM_WIDTH, pos.y - 5 }, { TIME_BAR_WIDTH, TIME_BAR_HEIGHT } );

	for( size_t i = 0; i < labels.size(); i++ )
	{
		graphics.DrawRect( { pos.x - ITEM_WIDTH / 2, pos.y - ITEM_HEIGHT / 2 }, { pos.x + ITEM_WIDTH / 2, pos.y + ITEM_HEIGHT / 2 }, bgColours[i], true );
		graphics.DrawDebugString( pos, labels[i], textColours[i] );
		pos.x += ITEM_WIDTH;
	}
}

// Draws the mini screen using a separate render target 
void DrawMiniScreen( Point2f pos, float elapsedTime )
{
	PlayGraphics& graphics = PlayGraphics::Instance();

	static int spr_dead = graphics.GetSpriteId( "agent8_dead" );
	static int spr_star = graphics.GetSpriteId( "star" );
	static bool mini_screen = true;

	static float lifetime = 0;
	lifetime += elapsedTime;
	int frameIndex = static_cast<int>( ( lifetime * FRAMES_PER_SECOND ) / 20);

	static float rot_angle = 0.0f;
	static std::vector< Point2f > stars = { { rand(), rand() }, { rand(), rand() }, { rand(), rand() }, { rand(), rand() } };
	rot_angle += 0.01f;

	if( mini_screen )
	{
		graphics.SetRenderTarget( &g_miniScreen ); // Subsequent graphics operations act on the mini screen
		graphics.ClearBuffer( Pixel( 0x00, 0x00, 0x4F ) );
		for( Point2f& starPos : stars )
		{
			starPos += 10 * Point2f( -sin( rot_angle ), cos( rot_angle ) );
			starPos += { MINI_WIDTH, MINI_HEIGHT };
			starPos.x = fmodf( starPos.x, MINI_WIDTH );
			starPos.y = fmodf( starPos.y, MINI_HEIGHT );
			graphics.Draw( spr_star, starPos, frameIndex );
		}
		graphics.DrawRotated( spr_dead, { MINI_WIDTH / 2, MINI_HEIGHT / 2 }, frameIndex, rot_angle );
		graphics.SetRenderTarget( graphics.GetDrawingBuffer() ); // Subsequent graphics operations revert to the main drawing buffer
		graphics.DrawPixelData( &g_miniScreen, pos, 0.8f ); // Draw the mini screen to the main drawing buffer
		graphics.DrawRect( pos, { pos.x + MINI_WIDTH, pos.y + MINI_HEIGHT }, PIX_WHITE );
	}

	if( PlayInput::Instance().KeyPressed( 'M' ) )
		mini_screen = !mini_screen;
}