// This is purely for code examples.
// The extra comments are because doxygen doesn't include the snippets if it's not at least two lines.

#include "PlayBufferPCH.h"
using namespace Play;
#include "PlayObject.h"

#pragma warning(disable:4189)

void Function()
{
	//!	[CreateManager]
	constexpr int DISPLAY_WIDTH = 1280;
	constexpr int DISPLAY_HEIGHT = 720;
	constexpr int DISPLAY_SCALE = 1;

	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	//!	[CreateManager]
	//!	[DestroyManager]
	//PH
	Play::DestroyManager();
	//!	[DestroyManager]

	//!	[PresentDrawingBuffer]
	//PH
	Play::PresentDrawingBuffer();
	//!	[PresentDrawingBuffer]
	
	//!	[GetBufferWidth]
	//PH
	int BufferWidth = Play::GetBufferWidth();
	//!	[GetBufferWidth]
	
	//!	[GetBufferHeight]
	//PH
	int BufferHeight = Play::GetBufferHeight();
	//!	[GetBufferHeight]
	
	//!	[GetMousePos]
	//PH
	Point2D mousePos = Play::GetMousePos();
	//!	[GetMousePos]
	
	//! [GetMouseButton]
	//PH
	bool LeftButtonClicked = Play::GetMouseButton(LEFT);
	//! [GetMouseButton]

	//! [PlayAudio]
	//PH
	int soundID = Play::PlayAudio( "blast" );
	//! [PlayAudio]

	//! [PlayAudioPitch]
	//PH
	int pitchSoundID = Play::PlayAudioPitch( "blast", 45, 65 );
	//! [PlayAudioPitch]
	//! [StartAudioLoop]
	//PH
	int loopedSoundID = Play::StartAudioLoop( "music" );
	//! [StartAudioLoop]
	
	//! [StopAudioLoop]
	//PH
	Play::StopAudioLoop( loopedSoundID );
	//! [StopAudioLoop]
	
	//! [StopAudio]
	Play::StopAudio( "blast" );
	Play::StopAudio( "music" );
	//! [StopAudio]

	//! [SetCameraPosition]
	//PH
	Play::SetCameraPosition( Point2f( 100.0f, 200.0f ) );
	//! [SetCameraPosition]
	
	//! [SetDrawingSpace]
	// Go into screen space, so we can position things on the screen 
	// without having the camera position affect them.
	Play::SetDrawingSpace(SCREEN);

	// Draw some user interface elements...
	//! [DrawFontText]
	Play::DrawFontText( "font64px_10x10", "Level 1: Get ready!", 
						{ DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 }, 
						Play::CENTRE );
	//! [DrawFontText]

	// Go back into world space ready for the next frame.
	Play::SetDrawingSpace(WORLD);
	//! [SetDrawingSpace]

	//! [GetCameraPosition]
	//PH
	Point2f currentCameraPosition = Play::GetCameraPosition();
	//! [GetCameraPosition]
	
	//! [GetDrawingSpace]
	//PH
	DrawingSpace myDrawingSpace = Play::GetDrawingSpace();
	//! [GetDrawingSpace]
	
	//! [ClearDrawingBuffer]
	//PH
	Play::ClearDrawingBuffer( Play::cOrange );
	//! [ClearDrawingBuffer]
	
	//! [LoadBackground]
	//PH
	int backgroundIndex = Play::LoadBackground( "Data\\Backgrounds\\spr_background.png" );
	//! [LoadBackground]

	//! [DrawBackground]
	//PH
	Play::DrawBackground( backgroundIndex );
	//! [DrawBackground]
	
	//! [DrawDebugText]
	//PH
	Play::DrawDebugText( { 50, 50 }, "Hello World!", Play::cBlue, true );
	//! [DrawDebugText]
	
	//! [GetSpriteId]
	//PH
	int agent8_sprId = Play::GetSpriteId( "agent8" );
	//! [GetSpriteId]
	
	//! [GetSpriteHeight]
	//PH
	int fan_height = Play::GetSpriteHeight( "fan" );
	//! [GetSpriteHeight]
	
	//! [GetSpriteWidth]
	//PH
	int fan_width = Play::GetSpriteWidth( "fan" );
	//! [GetSpriteWidth]
	
	//! [GetSpriteHeightID]
	//PH
	int agent8_height = Play::GetSpriteHeight( agent8_sprId );
	//! [GetSpriteHeightID]
	
	//! [GetSpriteWidthID]
	//PH
	int agent8_width = Play::GetSpriteWidth( agent8_sprId );
	//! [GetSpriteWidthID]

	//! [GetSpriteName]
	//PH
	const char* agent8_name = Play::GetSpriteName( agent8_sprId );
	//! [GetSpriteName]
	
	//! [GetSpriteFrames]
	//PH
	int agent8_numFrames = Play::GetSpriteFrames( agent8_sprId );
	//! [GetSpriteFrames]
	
	//! [ColourSprite]
	//PH
	Play::ColourSprite( "agent8" , Play::cGreen );
	//! [ColourSprite]
	
	//! [CentreSpriteOrigin]
	//PH
	Play::CentreSpriteOrigin( "agent8_fall" );
	//! [CentreSpriteOrigin]

	//! [CentreMatchingSpriteOrigins]
	//PH
	Play::CentreMatchingSpriteOrigins( "agent8" );
	//! [CentreMatchingSpriteOrigins]	
	
	//! [CentreAllSpriteOrigins]
	//PH
	Play::CentreAllSpriteOrigins();
	//! [CentreAllSpriteOrigins]
		
	//! [MoveSpriteOrigin]
	//PH
	Play::MoveSpriteOrigin( "agent8_fall", 10, 20);
	//! [MoveSpriteOrigin]
			
	//! [MoveMatchingSpriteOrigins]
	//PH
	Play::MoveMatchingSpriteOrigins( "agent8", 10, 20 );
	//! [MoveMatchingSpriteOrigins]
			
	//! [MoveAllSpriteOrigins]
	//PH
	Play::MoveAllSpriteOrigins( 10, 20 );
	//! [MoveAllSpriteOrigins]
		
	//! [SetSpriteOrigin]
	//PH
	Play::SetSpriteOrigin( "agent8_fall", 10, 20);
	//! [SetSpriteOrigin]
		
	//! [SetSpriteOriginID]
	//PH
	Play::SetSpriteOrigin( agent8_sprId, 10, 20);
	//! [SetSpriteOriginID]
			
	//! [GetSpriteOrigin]
	//PH
	Point2D agent8_fall_sprOrigin = Play::GetSpriteOrigin( "agent8_fall" );
	//! [GetSpriteOrigin]
				
	//! [GetSpriteOriginID]
	//PH
	Point2D agent8_sprOrigin = Play::GetSpriteOrigin( agent8_sprId );
	//! [GetSpriteOriginID]

	//! [GetSpritePixelData]
	//PH
	const PixelData *agent8_pixelData = Play::GetSpritePixelData( agent8_sprId );
	//! [GetSpritePixelData]
	
	//! [SetDrawingBlendMode]
	//PH
	Play::SetDrawingBlendMode(BLEND_ADD);
	//! [SetDrawingBlendMode]
		
	//! [DrawSprite]
	//PH
	Play::DrawSprite( "agent8", { 100, 100 }, 0 );
	//! [DrawSprite]
			
	//! [DrawSpriteID]
	//PH
	Play::DrawSprite( agent8_sprId, { 100, 100 }, 0 );
	//! [DrawSpriteID]
			
	//! [DrawSpriteTransparent]
	//PH
	Play:: DrawSpriteTransparent( "agent8", { 100, 100 }, 0, 0.5f );
	//! [DrawSpriteTransparent]
			
	//! [DrawSpriteTransparentID]
	//PH
	Play:: DrawSpriteTransparent( agent8_sprId, { 100, 100 }, 0, 0.5f );
	//! [DrawSpriteTransparentID]
			
	//! [DrawSpriteRotated]
	//PH
	Play::DrawSpriteRotated( "agent8", { 100, 100 }, 0, DegToRad(120.0f) );
	//! [DrawSpriteRotated]
			
	//! [DrawSpriteRotatedID]
	//PH
	Play::DrawSpriteRotated( agent8_sprId, { 100, 100 }, 0, DegToRad(120.0f) );
	//! [DrawSpriteRotatedID]
				
	//! [DrawSpriteTransformed]
	Matrix2D transformMatrix =  MatrixScale(0.5f, 2.0f) * 
								MatrixRotation( DegToRad(45.0f) ) * 
								MatrixTranslation(100.0f, 150.0f);
	Play::DrawSpriteTransformed( agent8_sprId, transformMatrix, 0);
	//! [DrawSpriteTransformed]
			
	//! [DrawLine]
	//PH
	Play::DrawLine( {10, 10 }, { 40, 40 }, Play::cWhite );
	//! [DrawLine]
				
	//! [DrawCircle]
	//PH
	Play::DrawCircle( { 150, 150 }, 30, Play::cWhite );
	//! [DrawCircle]
					
	//! [DrawRect]
	//PH
	Play::DrawRect( { 100, 100 }, { 300, 200 }, Play::cWhite );
	//! [DrawRect]
				
	//! [DrawSpriteLine]
	//PH
	Play::DrawSpriteLine( { 10, 10 }, { 40, 40 }, "pen_8px", Play::cBlue );
	//! [DrawSpriteLine]
					
	//! [DrawSpriteCircle]
	//PH
	Play::DrawSpriteCircle( { 150, 150 }, 40, "pen_8px", Play::cBlue );
	//! [DrawSpriteCircle]
						
	//! [DrawPixel]
	//PH
	Play::DrawPixel( { 150, 150 }, Play::cBlue );
	//! [DrawPixel]
						
	//! [BeginTimingBar]
	//PH
	Play::BeginTimingBar(cWhite);
	//! [BeginTimingBar]
							
	//! [ColourTimingBar]
	//PH
	Play::ColourTimingBar(cRed);
	//! [ColourTimingBar]
							
	//! [DrawTimingBar]
	//PH
	Play::DrawTimingBar({ 0,0 }, { 20, 10 });
	//! [DrawTimingBar]
								
	//! [KeyPressed]
	//PH
	bool spacePressed = Play::KeyPressed( Play::KEY_SPACE );
	//! [KeyPressed]
									
	//! [KeyDown]
	//PH
	bool spaceDown = Play::KeyDown( Play::KEY_SPACE );
	//! [KeyDown]
										
	//! [RandomRoll]
	//PH
	int hitRoll = Play::RandomRoll( 20 );
	//! [RandomRoll]
										
	//! [RandomRollRange]
	//PH
	int damageRoll = Play::RandomRollRange( 4, 12 );
	//! [RandomRollRange]
	
#define TYPE_PLAYER 1
#define TYPE_FAN 2
#define TYPE_TOOL 3
#define TYPE_LASER 4
#define TYPE_COIN 4

	int fan_sprite_id = 3;

	//! [GameObject]
	int agent8_id = Play::CreateGameObject(TYPE_PLAYER, {512, 128}, 64, "agent8");
	GameObject& agent8 = Play::GetGameObject( agent8_id );
	agent8.velocity = { 10, 0 };
	agent8.pos.y = 75;
	//! [GameObject]

	//! [CreateGameObject]
	//PH
	int coin_id = Play::CreateGameObject(TYPE_COIN, {320, 64}, 32, "coin");
	//! [CreateGameObject]
	
	//! [GetGameObject]
	//PH
	GameObject& coin = Play::GetGameObject( coin_id );
	//! [GetGameObject]
	
	//! [GetGameObjectByType]
	//PH
	GameObject& laser = Play::GetGameObjectByType(TYPE_LASER);
	//! [GetGameObjectByType]

	//! [CollectGameObjectIDsByType]
	//PH
	std::vector<int> tool_IDs = Play::CollectGameObjectIDsByType(TYPE_TOOL);
	//! [CollectGameObjectIDsByType]

	//! [CollectAllGameObjectIDs]
	//PH
	std::vector<int> gameObject_IDs = Play::CollectAllGameObjectIDs();
	//! [CollectAllGameObjectIDs]

	//! [UpdateGameObject]
	//PH
	Play::UpdateGameObject( agent8 );
	//! [UpdateGameObject]
	
	//! [DestroyGameObject]
	//PH
	Play::DestroyGameObject( agent8_id );
	//! [DestroyGameObject]

	//! [DestroyGameObjectsByType]
	//PH
	Play::DestroyGameObjectsByType(TYPE_TOOL);
	//! [DestroyGameObjectsByType]
	
	//! [DestroyAllGameObjects]
	//PH
	Play::DestroyAllGameObjects();
	//! [DestroyAllGameObjects]
		
	//! [IsColliding]
	//PH
	bool shotByLaser = Play::IsColliding( agent8, laser );
	//! [IsColliding]

	//! [IsVisible]
	//PH
	bool agent8Visible = Play::IsVisible( agent8 );
	//! [IsVisible]
	
	//! [IsLeavingDisplayArea]
	//PH
	bool leaving = Play::IsLeavingDisplayArea( agent8, Play::VERTICAL );
	//! [IsLeavingDisplayArea]
	
	//! [IsAnimationComplete]
	//PH
	bool agent8AnimationComplete = Play::IsAnimationComplete( agent8 );
	//! [IsAnimationComplete]
		
	//! [SetGameObjectDirection]
	//PH
	Play::SetGameObjectDirection( agent8, 10, PLAY_PI);
	//! [SetGameObjectDirection]
		
	//! [PointGameObject]
	//PH
	Play::PointGameObject( agent8, 400, 400);
	//! [PointGameObject]
			
	//! [SetSprite]
	//PH
	Play::SetSprite( agent8, "agent8", 1 );
	//! [SetSprite]
				
	//! [DrawObject]
	//PH
	Play::DrawObject( agent8 );
	//! [DrawObject]
					
	//! [DrawObjectTransparent]
	//PH
	Play::DrawObjectTransparent( agent8, 0.7f );
	//! [DrawObjectTransparent]
	
	//! [DrawObjectRotated]
	//PH
	Play::DrawObjectRotated( agent8, DegToRad(45.0f) );
	//! [DrawObjectRotated]
		
	//! [DrawGameObjectsDebug]
	//PH
	Play::DrawGameObjectsDebug();
	//! [DrawGameObjectsDebug]

}