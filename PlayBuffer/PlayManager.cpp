//********************************************************************************************************************************
// File:		PlayManager.cpp
// Description:	A manager for providing simplified access to the PlayBuffer framework
// Platform:	Independent
// Notes:		The GameObject management is "opt in" as many will want to create their own GameObject class
//********************************************************************************************************************************
#include "PlayBufferPCH.h"

namespace Play
{
	// A set of default colour definitions
	Colour cBlack{ 0.0f, 0.0f, 0.0f };
	Colour cRed{ 100.0f, 0.0f, 0.0f };
	Colour cGreen{ 0.0f, 100.0f, 0.0f };
	Colour cBlue{ 0.0f, 0.0f, 100.0f };
	Colour cMagenta{ 100.0f, 0.0f, 100.0f };
	Colour cCyan{ 0.0f, 100.0f, 100.0f };
	Colour cYellow{ 100.0f, 100.0f, 0.0f };
	Colour cOrange{ 100.0f, 50.0f, 0.0f };
	Colour cWhite{ 100.0f, 100.0f, 100.0f };
	Colour cGrey{ 50.0f, 50.0f, 50.0f };

	int frameCount = 0; // Updated in Play::Present

	// The camera
	Point2f cameraPos{ 0.0f, 0.0f };
	// Spaces and co-ordinate systems
	DrawingSpace drawSpace = DrawingSpace::WORLD;

	//**************************************************************************************************
	// Manager creation and deletion
	//**************************************************************************************************

	void CreateManager( int displayWidth, int displayHeight, int displayScale )
	{
		Play::Graphics::CreateManager( displayWidth, displayHeight, "Data\\Sprites\\" );
		Play::Window::CreateManager( Play::Graphics::GetDrawingBuffer(), displayScale );
		Play::Window::RegisterMouse( Play::Input::CreateManager() );
		Play::Audio::CreateManager( "Data\\Audio\\" );
		// Seed the game's random number generator based on the time
		srand( (int)time( NULL ) );
	}

	void DestroyManager()
	{
		Play::Audio::DestroyManager();
		Play::Graphics::DestroyManager();
		Play::Window::DestroyManager();
		Play::Input::DestroyManager();
#ifdef PLAY_USING_GAMEOBJECT_MANAGER
		Play::DestroyAllGameObjects();
#endif
	}

	//**************************************************************************************************
	// PlayGraphics functions
	//**************************************************************************************************

	void ClearDrawingBuffer( Colour c )
	{
		int r = static_cast<int>( c.red * 2.55f );
		int g = static_cast<int>( c.green * 2.55f );
		int b = static_cast<int>( c.blue * 2.55f );
		Play::Graphics::ClearBuffer( { r, g, b } );
	}

	void DrawDebugText( Point2D pos, const char* text, Colour c, bool centred )
	{
		Play::Graphics::DrawDebugString( TRANSFORM_SPACE( pos ), text, { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f }, centred );
	}

	void PresentDrawingBuffer()
	{
		static bool debugInfo = false;
		DrawingSpace originalDrawSpace = drawSpace;

		if( KeyPressed( KEY_F1 ) )
			debugInfo = !debugInfo;

		if( debugInfo )
		{
			drawSpace = DrawingSpace::SCREEN;

			int textX = 10;
			int textY = 10;
			std::string s = "PlayBuffer Version:" + std::string( PLAY_VERSION );
			Play::Graphics::DrawDebugString( { textX - 1, textY - 1 }, s, PIX_BLACK, false );
			Play::Graphics::DrawDebugString( { textX + 1, textY + 1 }, s, PIX_BLACK, false );
			Play::Graphics::DrawDebugString( { textX + 1, textY - 1 }, s, PIX_BLACK, false );
			Play::Graphics::DrawDebugString( { textX - 1, textY + 1 }, s, PIX_BLACK, false );
			Play::Graphics::DrawDebugString( { textX, textY }, s, PIX_YELLOW, false );

			drawSpace = DrawingSpace::WORLD;

#ifdef PLAY_USING_GAMEOBJECT_MANAGER	
			DrawGameObjectsDebug();
#endif
		}

		Play::Window::Present();
		frameCount++;

		drawSpace = originalDrawSpace;
	}

	//**************************************************************************************************
	// PlayAudio functions
	//**************************************************************************************************

	int PlayAudioPitch( const char* fileName, int minPercent, int maxPercent )
	{
		PLAY_ASSERT_MSG( minPercent > 1.0f && maxPercent < 10000.0f, "Invalid audio pitch range: must be between 1 and 10,000 %" );
		float freq = (minPercent + rand() % (maxPercent - minPercent)) / 100.0f;
		return Play::Audio::StartSound( fileName, false, 1.0f, freq);
	}

	//**************************************************************************************************
	// PlayGraphics functions
	//**************************************************************************************************

	void ColourSprite( const char* spriteName, Colour c )
	{
		int spriteId = Play::Graphics::GetSpriteId( spriteName );
		Play::Graphics::ColourSprite( spriteId, static_cast<int>( c.red * 2.55f ), static_cast<int>( c.green * 2.55f), static_cast<int>( c.blue * 2.55f ) );
	}

	void CentreSpriteOrigin( const char* spriteName )
	{
		int spriteId = Play::Graphics::GetSpriteId( spriteName );
		Play::Graphics::SetSpriteOrigin( spriteId, Play::Graphics::GetSpriteSize( spriteId ) / 2, false );
	}

	void CentreMatchingSpriteOrigins( const char* rootName )
	{
		int spriteId = Play::Graphics::GetSpriteId( rootName ); // Finds the first matching sprite and assumes same dimensions
		Play::Graphics::SetSpriteOrigins( rootName, Play::Graphics::GetSpriteSize( spriteId ) / 2, false );
	}

	void CentreAllSpriteOrigins()
	{
		for( int i = 0; i < Play::Graphics::GetTotalLoadedSprites(); i++ )
			Play::Graphics::SetSpriteOrigin( i, Play::Graphics::GetSpriteSize( i ) / 2, false );
	}

	void MoveSpriteOrigin( const char* spriteName, int xOffset, int yOffset )
	{
		int spriteId = Play::Graphics::GetSpriteId( spriteName );
		Play::Graphics::SetSpriteOrigin( spriteId, { xOffset, yOffset }, true ); // relative option set
	}


	void MoveAllSpriteOrigins(int xoffset, int yoffset)
	{
		for (int i = 0; i < Play::Graphics::GetTotalLoadedSprites(); i++)
			Play::Graphics::SetSpriteOrigin(i, { xoffset, yoffset }, true ); // relative option set
	}

	void SetSpriteOrigin( const char* spriteName, int xOrigin, int yOrigin )
	{
		int spriteId = Play::Graphics::GetSpriteId( spriteName );
		Play::Graphics::SetSpriteOrigin( spriteId, { xOrigin, yOrigin } ); 
	}
	
	void DrawSpriteTransparent(const char* spriteName, Point2D pos, int frameIndex, float opacity, Colour colour)
	{
		Graphics::DrawTransparent(Graphics::GetSpriteId(spriteName), TRANSFORM_SPACE(pos), frameIndex, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f });
	}

	void DrawSpriteTransparent(int spriteID, Point2D pos, int frameIndex, float opacity, Colour colour)
	{
		Graphics::DrawTransparent(spriteID, TRANSFORM_SPACE(pos), frameIndex, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f } );
	}

	void DrawSpriteRotated( const char* spriteName, Point2D pos, int frameIndex, float angle, float scale, float opacity, Colour colour)
	{
		Play::Graphics::DrawRotated( Play::Graphics::GetSpriteId( spriteName ), TRANSFORM_SPACE( pos ), frameIndex, angle, scale, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f });
	}

	void DrawSpriteRotated( int spriteID, Point2D pos, int frameIndex, float angle, float scale, float opacity, Colour colour )
	{
		Play::Graphics::DrawRotated( spriteID, TRANSFORM_SPACE( pos ), frameIndex, angle, scale, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f });
	}

	void DrawSpriteTransformed( int spriteID, const Matrix2D& transform, int frameIndex, float opacity, Colour colour )
	{
		Play::Graphics::DrawTransformed( spriteID, TRANSFORM_MATRIX_SPACE( transform ), frameIndex, { opacity, colour.red / 100.0f, colour.green / 100.0f, colour.blue / 100.0f });
	}

	void DrawLine( Point2f start, Point2f end, Colour c )
	{
		return Play::Graphics::DrawLine( TRANSFORM_SPACE( start), TRANSFORM_SPACE( end ), { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f }  );
	}

	void DrawCircle( Point2D pos, int radius, Colour c )
	{
		Play::Graphics::DrawCircle( TRANSFORM_SPACE( pos ), radius, { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f } );
	}

	void DrawRect(  Point2D bottomLeft, Point2D topRight, Colour c, bool fill )
	{
		Play::Graphics::DrawRect( TRANSFORM_SPACE( bottomLeft ), TRANSFORM_SPACE( topRight ), { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f }, fill );
	}

	void DrawSpriteLine( Point2D startPos, Point2D endPos, const char* penSprite, Colour col /*= cWhite */ )
	{
		int spriteId = Play::Graphics::GetSpriteId( penSprite );
		ColourSprite( penSprite, col );

		//Draws a line in any angle
		int x1 = static_cast<int>( startPos.x );
		int y1 = static_cast<int>( startPos.y );
		int x2 = static_cast<int>( endPos.x );
		int y2 = static_cast<int>( endPos.y );

		//Implementation of Bresenham's Line Drawing Algorithm
		int dx = abs( x2 - x1 );
		int sx = 1;
		if( x2 < x1 ) { sx = -1; }

		int dy = -abs( y2 - y1 );
		int sy = 1;
		if( y2 < y1 ) { sy = -1; }
		
		int err = dx + dy;

		if( dx == 0 && dy == 0 ) return;

		while( true )
		{
			Play::DrawSprite( spriteId, { x1, y1 }, 0 );
			
			if( x1 == x2 && y1 == y2 )
				break;

			int e2 = 2 * err;
			if( e2 >= dy )
			{
				err += dy;
				x1 += sx;
			}
			if( e2 <= dx )
			{
				err += dx;
				y1 += sy;
			}
		}
	}

	// Not exposed externally
	void DrawCircleOctants( int spriteId, int x, int y, int ox, int oy )
	{
		//displaying all 8 coordinates of(x,y) residing in 8-octants
		Play::DrawSprite( spriteId, { x + ox, y + oy }, 0 );
		Play::DrawSprite( spriteId, { x - ox, y + oy }, 0 );
		Play::DrawSprite( spriteId, { x + ox, y - oy }, 0 );
		Play::DrawSprite( spriteId, { x - ox, y - oy }, 0 );
		Play::DrawSprite( spriteId, { x + oy, y + ox }, 0 );
		Play::DrawSprite( spriteId, { x - oy, y + ox }, 0 );
		Play::DrawSprite( spriteId, { x + oy, y - ox }, 0 );
		Play::DrawSprite( spriteId, { x - oy, y - ox }, 0 );
	}

	void DrawSpriteCircle( Point2D pos, int radius, const char* penSprite, Colour col )
	{
		int spriteId = Play::Graphics::GetSpriteId( penSprite );
		ColourSprite( penSprite, col );

		pos = TRANSFORM_SPACE( pos );

		int ox = 0, oy = radius;
		int d = 3 - 2 * radius;
		DrawCircleOctants( spriteId, static_cast<int>(pos.x), static_cast<int>(pos.y), ox, oy );

		while( oy >= ox )
		{
			ox++;
			if( d > 0 )
			{
				oy--;
				d = d + 4 * ( ox - oy ) + 10;
			}
			else
			{
				d = d + 4 * ox + 6;
			}
			DrawCircleOctants( spriteId, static_cast<int>(pos.x), static_cast<int>(pos.y), ox, oy );
		}
	};

	void DrawFontText( const char* fontId, std::string text, Point2D pos, Align justify )
	{
		int font = Play::Graphics::GetSpriteId( fontId );

		int totalWidth{ 0 };

		for( char c : text )
			totalWidth += Play::Graphics::GetFontCharWidth( font, c );

		switch( justify )
		{
			case Align::CENTRE:
				pos.x -= totalWidth / 2;
				break;
			case Align::RIGHT:
				pos.x -= totalWidth;
				break;
			default:
				break;
		}

		pos.x += Play::Graphics::GetSpriteOrigin( font ).x;
		Play::Graphics::DrawString( font, TRANSFORM_SPACE( pos ), text );
	}

	void DrawPixel(Point2D pos, Colour col)
	{
		return Play::Graphics::DrawPixel(TRANSFORM_SPACE( pos ), { col.red * 2.55f, col.green * 2.55f, col.blue * 2.55f });
	}

	void BeginTimingBar( Colour c )
	{
		Play::Graphics::TimingBarBegin( Pixel( c.red*2.55f, c.green*2.55f, c.blue*2.55f ) );
	}

	int ColourTimingBar( Colour c )
	{
		return Play::Graphics::SetTimingBarColour( Pixel( c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f ) );
	}

	//**************************************************************************************************
	// Miscellaneous functions
	//**************************************************************************************************

	int RandomRollRange( int begin, int end )
	{
		int range = abs( end - begin );
		int rnd = ( rand() % ( range + 1 ) );
		if( end > begin )
			return begin + rnd;
		else
			return end + rnd;
	}
}