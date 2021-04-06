//********************************************************************************************************************************
// File:		PlayManager.cpp
// Description:	A manager for providing simplified access to the PlayBuffer framework
// Platform:	Independent
// Notes:		The GameObject management is "opt in" as many will want to create their own GameObject class
//********************************************************************************************************************************
#include "PlayBufferPCH.h"

//**************************************************************************************************
// GameObject Class Definition
//**************************************************************************************************

// Define this to opt in to the PlayManager
#ifdef PLAY_USING_GAMEOBJECT_MANAGER

// Constructor for the GameObject struct - kept as simple as possible
GameObject::GameObject( int type, Point2f newPos, int collisionRadius, int spriteId = 0 )
	: type( type ), pos( newPos ), radius( collisionRadius ), spriteId( spriteId )
{
	// Member variables are assigned default values in the class header
	static int uniqueId = 0;
	m_id = uniqueId++;
}

#endif

// The PlayManager is namespace rather than a class
namespace Play
{
#ifdef PLAY_USING_GAMEOBJECT_MANAGER

	// A map is used internally to store all the GameObjects and their unique ids
	static std::map<int, GameObject&> objectMap;

	// Used instead of Null return values, PlayMangager operations performed on this GameObject should fail transparently
	static GameObject noObject{ -1,{ 0, 0 }, 0, -1 };

#endif 

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

	//**************************************************************************************************
	// Manager creation and deletion
	//**************************************************************************************************

	void CreateManager( int displayWidth, int displayHeight, int displayScale )
	{
		PlayGraphics::Instance( displayWidth, displayHeight, "Data\\Sprites\\" );
		PlayWindow::Instance( PlayGraphics::Instance().GetDrawingBuffer(), displayScale );
		PlayWindow::Instance().RegisterMouse( PlayInput::Instance().GetMouseData() );
		PlayAudio::Instance( "Data\\Audio\\" );
		// Seed the game's random number generator based on the time
		srand( (int)time( NULL ) );
	}

	void DestroyManager()
	{
		PlayAudio::Destroy();
		PlayGraphics::Destroy();
		PlayWindow::Destroy();
		PlayInput::Destroy();
#ifdef PLAY_USING_GAMEOBJECT_MANAGER
		for( std::pair<const int, GameObject&>& p : objectMap )
			delete& p.second;
		objectMap.clear();
#endif
	}

	int GetBufferWidth()
	{
		return PlayWindow::Instance().GetWidth();
	}

	int GetBufferHeight()
	{
		return PlayWindow::Instance().GetHeight();
	}

	//**************************************************************************************************
	// PlayGraphics functions
	//**************************************************************************************************

	void ClearDrawingBuffer( Colour c )
	{
		int r = static_cast<int>( c.red * 2.55f );
		int g = static_cast<int>( c.green * 2.55f );
		int b = static_cast<int>( c.blue * 2.55f );
		PlayGraphics::Instance().ClearBuffer( { r, g, b } );
	}

	int LoadBackground( const char* pngFilename )
	{
		return PlayGraphics::Instance().LoadBackground( pngFilename );
	}

	void DrawBackground( int background )
	{
		PlayGraphics::Instance().DrawBackground( background );
	}

	void DrawDebugText( Point2D pos, const char* text, Colour c, bool centred )
	{
		PlayGraphics::Instance().DrawDebugString( pos, text, { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f }, centred );
	}

	void PresentDrawingBuffer()
	{
		PlayGraphics& pblt = PlayGraphics::Instance();
		static bool debugInfo = false;

		if( KeyPressed( VK_F1 ) )
			debugInfo = !debugInfo;

		if( debugInfo )
		{
			int textX = 10;
			int textY = 10;
			std::string s = "PlayBuffer Version:" + std::string( PLAY_VERSION );
			pblt.DrawDebugString( { textX - 1, textY - 1 }, s, PIX_BLACK, false );
			pblt.DrawDebugString( { textX + 1, textY + 1 }, s, PIX_BLACK, false );
			pblt.DrawDebugString( { textX + 1, textY - 1 }, s, PIX_BLACK, false );
			pblt.DrawDebugString( { textX - 1, textY + 1 }, s, PIX_BLACK, false );
			pblt.DrawDebugString( { textX, textY }, s, PIX_YELLOW, false );

#ifdef PLAY_USING_GAMEOBJECT_MANAGER
			
			for( std::pair<const int, GameObject&>& i : objectMap )
			{
				GameObject& obj = i.second;
				int id = obj.spriteId;
				Vector2D size = pblt.GetSpriteSize( obj.spriteId );
				Vector2D origin = pblt.GetSpriteOrigin( id );

				// Corners of sprite drawing area
				Point2D p0 = obj.pos - origin;
				Point2D p2 = { obj.pos.x + size.width - origin.x, obj.pos.y + size.height - origin.y };
				Point2D p1 = { p2.x, p0.y };
				Point2D p3 = { p0.x, p2.y };

				DrawLine( p0, p1, cRed );
				DrawLine( p1, p2, cRed );
				DrawLine( p2, p3, cRed );
				DrawLine( p3, p0, cRed );

				DrawCircle( obj.pos, obj.radius, cBlue );

				DrawLine( { obj.pos.x - 20,  obj.pos.y - 20 }, { obj.pos.x + 20, obj.pos.y + 20 }, cWhite );
				DrawLine( { obj.pos.x + 20, obj.pos.y - 20 }, { obj.pos.x - 20, obj.pos.y + 20 }, cWhite );

				s = pblt.GetSpriteName( obj.spriteId ) + " f[" + std::to_string( obj.frame ) + "]";
				pblt.DrawDebugString( { ( p0.x + p1.x ) / 2.0f, p0.y - 20 }, s, PIX_WHITE, true );
			}
#endif
		}

		PlayWindow::Instance().Present();
	}

	Point2D GetMousePos()
	{
		PlayInput& input = PlayInput::Instance();
		return input.GetMousePos();
	}

	//**************************************************************************************************
	// PlaySpeaker functions
	//**************************************************************************************************

	void PlayAudio( const char* fileName )
	{
		PlayAudio::Instance().StartAudio( fileName, false );
	}

	void StartAudioLoop( const char* fileName )
	{
		PlayAudio::Instance().StartAudio( fileName, true );
	}

	void StopAudioLoop( const char* fileName )
	{
		PlayAudio::Instance().StopAudio( fileName );
	}

	//**************************************************************************************************
	// PlayBuffer functions
	//**************************************************************************************************

	int GetSpriteId( const char* spriteName )
	{
		return PlayGraphics::Instance().GetSpriteId( spriteName );
	}

	int GetSpriteHeight( const char* spriteName )
	{
		return static_cast<int>(PlayGraphics::Instance().GetSpriteSize( GetSpriteId( spriteName ) ).height);
	}

	int GetSpriteWidth( const char* spriteName )
	{
		return static_cast<int>( PlayGraphics::Instance().GetSpriteSize( GetSpriteId( spriteName ) ).width );
	}

	int GetSpriteHeight( int spriteId )
	{
		return static_cast<int>( PlayGraphics::Instance().GetSpriteSize( spriteId ).height );
	}

	int GetSpriteWidth( int spriteId )
	{
		return static_cast<int>( PlayGraphics::Instance().GetSpriteSize( spriteId ).width );
	}

	Point2D GetSpriteOrigin( const char* spriteName )
	{
		return PlayGraphics::Instance().GetSpriteOrigin( GetSpriteId( spriteName ) );
	}

	Point2D GetSpriteOrigin( int spriteId )
	{
		return PlayGraphics::Instance().GetSpriteOrigin( spriteId );
	}

	const char* GetSpriteName( int spriteId )
	{
		return PlayGraphics::Instance().GetSpriteName( spriteId ).c_str();
	}

	void ColourSprite( const char* spriteName, Colour c )
	{
		int spriteId = PlayGraphics::Instance().GetSpriteId( spriteName );
		PlayGraphics::Instance().ColourSprite( spriteId, static_cast<int>( c.red * 2.55f ), static_cast<int>( c.green * 2.55f), static_cast<int>( c.blue * 2.55f ) );
	}

	void CentreSpriteOrigin( const char* spriteName )
	{
		PlayGraphics& pblt = PlayGraphics::Instance();
		int spriteId = pblt.GetSpriteId( spriteName );
		pblt.SetSpriteOrigin( spriteId, pblt.GetSpriteSize( spriteId ) / 2, false );
	}

	void CentreMatchingSpriteOrigins( const char* rootName )
	{
		PlayGraphics& pblt = PlayGraphics::Instance();
		int spriteId = pblt.GetSpriteId( rootName ); // Finds the first matching sprite and assumes same dimensions
		pblt.SetSpriteOrigins( rootName, pblt.GetSpriteSize( spriteId ) / 2, false );
	}

	void CentreAllSpriteOrigins()
	{
		PlayGraphics& pblt = PlayGraphics::Instance();

		for( int i = 0; i < pblt.GetTotalLoadedSprites(); i++ )
			pblt.SetSpriteOrigin( i, pblt.GetSpriteSize( i ) / 2, false );
	}

	void MoveSpriteOrigin( const char* spriteName, int xOffset, int yOffset )
	{
		PlayGraphics& pblt = PlayGraphics::Instance();
		int spriteId = pblt.GetSpriteId( spriteName );
		pblt.SetSpriteOrigin( spriteId, { xOffset, yOffset }, true ); // relative option set
	}

	void MoveMatchingSpriteOrigins( const char* rootName, int xoffset, int yoffset )
	{
		PlayGraphics& pblt = PlayGraphics::Instance();
		pblt.SetSpriteOrigins( rootName, { xoffset, yoffset }, true ); // relative option set
	}

	void MoveAllSpriteOrigins()
	{
		PlayGraphics& pblt = PlayGraphics::Instance();

		for( int i = 0; i < pblt.GetTotalLoadedSprites(); i++ )
			pblt.SetSpriteOrigin( i, pblt.GetSpriteSize( i ) / 2, true );
	}

	void SetSpriteOrigin( int spriteId, int xOrigin, int yOrigin )
	{
		PlayGraphics& pblt = PlayGraphics::Instance();
		pblt.SetSpriteOrigin( spriteId, { xOrigin, yOrigin } );
	}

	void SetSpriteOrigin( const char* spriteName, int xOrigin, int yOrigin )
	{
		PlayGraphics& pblt = PlayGraphics::Instance();
		int spriteId = pblt.GetSpriteId( spriteName );
		pblt.SetSpriteOrigin( spriteId, { xOrigin, yOrigin } ); 
	}

	void DrawSprite( const char* spriteName, Point2D pos, int frameIndex )
	{
		PlayGraphics::Instance().Draw( PlayGraphics::Instance().GetSpriteId( spriteName ), pos, frameIndex );
	}

	void DrawSprite( int spriteID, Point2D pos, int frameIndex )
	{
		PlayGraphics::Instance().Draw( spriteID, pos, frameIndex );
	}

	void DrawSpriteTransparent( const char* spriteName, Point2D pos, int frameIndex, float opacity )
	{
		PlayGraphics::Instance().DrawTransparent( PlayGraphics::Instance().GetSpriteId( spriteName ), pos, frameIndex, opacity );
	}

	void DrawSpriteTransparent( int spriteID, Point2D pos, int frameIndex, float opacity )
	{
		PlayGraphics::Instance().DrawTransparent( spriteID, pos, frameIndex, opacity );
	}

	void DrawSpriteRotated( const char* spriteName, Point2D pos, int frameIndex, float angle, float scale, float opacity )
	{
		PlayGraphics::Instance().DrawRotated( PlayGraphics::Instance().GetSpriteId( spriteName ), pos, frameIndex, angle, scale, opacity );
	}

	void DrawSpriteRotated( int spriteID, Point2D pos, int frameIndex, float angle, float scale, float opacity )
	{
		PlayGraphics::Instance().DrawRotated( spriteID, pos, frameIndex, angle, scale, opacity );
	}

	void DrawLine( Point2f start, Point2f end, Colour c )
	{
		return PlayGraphics::Instance().DrawLine( start, end, { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f }  );
	}

	void DrawCircle( Point2D pos, int radius, Colour c )
	{
		PlayGraphics::Instance().DrawCircle( pos, radius, { c.red * 2.55f, c.green * 2.55f, c.blue * 2.55f } );
	}

	void DrawSpriteLine( Point2f startPos, Point2f endPos, const char* penSprite, Colour c )
	{
		int spriteId = PlayGraphics::Instance().GetSpriteId( penSprite );
		ColourSprite( penSprite, c );

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

		if( err == 0 ) return;

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

	void DrawSpriteCircle( int x, int y, int radius, const char* penSprite, Colour c )
	{
		int spriteId = PlayGraphics::Instance().GetSpriteId( penSprite );
		ColourSprite( penSprite, c );

		int ox = 0, oy = radius;
		int d = 3 - 2 * radius;
		DrawCircleOctants( spriteId, x, y, ox, oy );

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
			DrawCircleOctants( spriteId, x, y, ox, oy );
		}
	};

	void DrawFontText( const char* fontId, std::string text, Point2D pos, Align justify )
	{
		int font = PlayGraphics::Instance().GetSpriteId( fontId );

		int totalWidth{ 0 };
		for( char c : text )
			totalWidth += PlayGraphics::Instance().GetFontCharWidth( font, c );

		switch( justify )
		{
			case CENTRE:
				pos.x -= totalWidth / 2;
				break;
			case RIGHT:
				pos.x -= totalWidth;
				break;
			default:
				break;
		}

		PlayGraphics::Instance().DrawString( font, pos, text );
	}

	//**************************************************************************************************
	// GameObject functions
	//**************************************************************************************************

#ifdef PLAY_USING_GAMEOBJECT_MANAGER

	int CreateGameObject( int type, Point2f newPos, int collisionRadius, const char* spriteName )
	{
		int spriteId = PlayGraphics::Instance().GetSpriteId( spriteName );
		// Deletion is handled in DestroyGameObject()
		GameObject* pObj = new GameObject( type, newPos, collisionRadius, spriteId );
		int id = pObj->GetId();
		objectMap.insert( std::map<int, GameObject&>::value_type( id, *pObj ) );
		return id;
	}

	GameObject& GetGameObject( int ID )
	{
		if( objectMap.find( ID ) == objectMap.end() )
			return noObject;

		return objectMap.find( ID )->second;
	}

	GameObject& GetGameObjectByType( int type )
	{
		for( std::pair<const int, GameObject&>& i : objectMap )
		{
			if( i.second.type == type )
				return i.second;
		}

		return noObject;
	}

	std::vector<int> CollectGameObjectIDsByType( int type )
	{
		std::vector<int> vec;
		for( std::pair<const int, GameObject&>& i : objectMap )
		{
			if( i.second.type == type )
				vec.push_back( i.first );
		}
		return vec; // Returning a copy of the vector
	}

	std::vector<int> CollectAllGameObjectIDs()
	{
		std::vector<int> vec;

		for( std::pair<const int, GameObject&>& i : objectMap )
			vec.push_back( i.first );

		return vec; // Returning a copy of the vector
	}

	void UpdateGameObject( GameObject& obj )
	{
		if( obj.type == -1 ) return; // Don't update noObject

		// Save the current position in case we need to go back
		obj.oldPos = obj.pos;
		obj.oldRot = obj.rotation;

		// Move the object according to a very simple physical model
		obj.velocity += obj.acceleration;
		obj.pos += obj.velocity;
		obj.rotation += obj.rotSpeed;

		// Handle the animation frame update
		obj.framePos += obj.animSpeed;
		if( obj.framePos > 1.0f )
		{
			obj.frame++;
			obj.framePos -= 1.0f;
		}
	}

	void DestroyGameObject( int ID )
	{
		if( objectMap.find( ID ) == objectMap.end() )
		{
			PLAY_ASSERT_MSG( false, "Unable to find object with given ID" );
		}
		else
		{
			GameObject* go = &objectMap.find( ID )->second;
			delete go;
			objectMap.erase( ID );
		}
	}

	void DestroyGameObjectsByType( int objType )
	{
		std::vector<int> typeVec = CollectGameObjectIDsByType( objType );
		for( size_t i = 1; i < typeVec.size(); i++ )
			DestroyGameObject( typeVec[i] );
	}

	bool IsColliding( GameObject& object1, GameObject& object2 )
	{
		//Don't collide with noObject
		if( object1.type == -1 || object2.type == -1 )
			return false;

		int xDiff = int( object1.pos.x ) - int( object2.pos.x );
		int yDiff = int( object1.pos.y ) - int( object2.pos.y );
		int radii = object2.radius + object1.radius;

		// Game progammers don't do square root!
		return( ( xDiff * xDiff ) + ( yDiff * yDiff ) < radii * radii );
	}

	bool IsVisible( GameObject& obj )
	{
		if( obj.type == -1 ) return false; // Not for noObject

		PlayGraphics& pblt = PlayGraphics::Instance();
		PlayWindow& pbuf = PlayWindow::Instance();

		int spriteID = obj.spriteId;
		Vector2f spriteSize = pblt.GetSpriteSize( obj.spriteId );
		Vector2f spriteOrigin = pblt.GetSpriteOrigin( spriteID );

		return( obj.pos.x + spriteSize.width - spriteOrigin.x > 0 && obj.pos.x - spriteOrigin.x < pbuf.GetWidth() &&
			obj.pos.y + spriteSize.height - spriteOrigin.y > 0 && obj.pos.y - spriteOrigin.y < pbuf.GetHeight() );
	}

	bool IsLeavingDisplayArea( GameObject& obj, Direction dirn )
	{
		if( obj.type == -1 ) return false; // Not for noObject

		PlayWindow& pbuf = PlayWindow::Instance();
		PlayGraphics& pblt = PlayGraphics::Instance();

		int spriteID = obj.spriteId;
		Vector2f spriteSize = pblt.GetSpriteSize( obj.spriteId );
		Vector2f spriteOrigin = pblt.GetSpriteOrigin( spriteID );

		if( dirn != VERTICAL )
		{
			if( obj.pos.x - spriteOrigin.x < 0 && obj.velocity.x < 0 )
				return true;

			if( obj.pos.x + spriteSize.width - spriteOrigin.x > pbuf.GetWidth() && obj.velocity.x > 0 )
				return true;
		}

		if( dirn != HORIZONTAL )
		{
			if( obj.pos.y - spriteOrigin.y < 0 && obj.velocity.y < 0 )
				return true;

			if( obj.pos.y + spriteSize.height - spriteOrigin.y > pbuf.GetHeight() && obj.velocity.y > 0 )
				return true;
		}

		return false;
	}

	bool IsAnimationComplete( GameObject& obj )
	{
		if( obj.type == -1 ) return false; // Not for noObject
		return obj.frame >= PlayGraphics::Instance().GetSpriteFrames( obj.spriteId ) - 1;
	}

	void SetGameObjectDirection( GameObject& obj, int speed, float angle )
	{
		if( obj.type == -1 ) return; // Not for noObject

		obj.velocity.x = speed * cos( angle );
		obj.velocity.y = speed * sin( angle );
	}

	void PointGameObject( GameObject& obj, int speed, int targetX, int targetY )
	{
		if( obj.type == -1 ) return; // Not for noObject

		float xdiff = obj.pos.x - targetX;
		float ydiff = obj.pos.y - targetY;

		float angle = atan2( ydiff, xdiff ) + 3.14f;

		obj.velocity.x = speed * cos( angle );
		obj.velocity.y = speed * sin( angle );
	}

	void SetSprite( GameObject& obj, const char* spriteName, float animSpeed )
	{
		int newSprite = PlayGraphics::Instance().GetSpriteId( spriteName );
		// Only reset the animation back to the start when it is new
		if( newSprite != obj.spriteId )
			obj.frame = 0;
		obj.spriteId = newSprite;
		obj.animSpeed = animSpeed;
	}

	void DrawObject( GameObject& obj )
	{
		if( obj.type == -1 ) return; // Don't draw noObject
		PlayGraphics::Instance().Draw( obj.spriteId, obj.pos, obj.frame );
	}

	void DrawObjectTransparent( GameObject& obj, float opacity )
	{
		if( obj.type == -1 ) return; // Don't draw noObject
		PlayGraphics::Instance().DrawTransparent( obj.spriteId, obj.pos, obj.frame, opacity );
	}

	void DrawObjectRotated( GameObject& obj, float opacity )
	{
		if( obj.type == -1 ) return; // Don't draw noObject
		PlayGraphics::Instance().DrawRotated( obj.spriteId, obj.pos, obj.frame, obj.rotation, obj.scale, opacity );
	}

#endif

	//**************************************************************************************************
	// Miscellaneous functions
	//**************************************************************************************************

	bool KeyPressed( int vKey )
	{
		return PlayInput::Instance().KeyPressed( vKey );
	}

	bool KeyDown( int vKey )
	{
		return PlayInput::Instance().KeyDown( vKey );
	}

	int RandomRoll( int sides )
	{
		return ( rand() % sides ) + 1;
	}

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