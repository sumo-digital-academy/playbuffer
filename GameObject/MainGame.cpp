// Play.h should be included in all cpp files, but PLAY_IMPLEMENTATION should only be defined in one 
#define PLAY_IMPLEMENTATION
#include "Play.h"

#include "MainGame.h"
#include "Laser.h"
#include "Saucer.h"
#include "Player.h"

// A global game state object accessible to everything (not ideal, but simple)
GameState gState;

// Avoid "magic numbers" in your code by using const or constexpr variables
const Point2f PLAYER_START_POS{ 640, 650 };

// MainGameEntry is the entry point for a PlayBuffer program: called once at the start of your program
// The Play Manager automatically loads all sprites in advance from "Data\Sprites"
// Backgrounds are loaded manually and audio is loaded from "Data\Audio" when requested
// Sprite origins control their positioning and centre of rotation
// GameObjects  add themselves to a managed internal list so no need to store the pointer from new (see GameObject.h)
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
    Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
    Play::LoadBackground( "Data\\Backgrounds\\background.png" );
    Play::CentreAllSpriteOrigins();
    Play::StartAudioLoop( "music" );
    new Player( PLAYER_START_POS );
}

// MainGameUpdate is called by the PlayBuffer once for each frame of the game (60 times a second!)
// This is where you update and draw your game state for a single frame: it must return every frame or your game will hang!
// elapsedTime can be used to implement a variable frame step, but it is ignored in this example
bool MainGameUpdate( float elapsedTime )
{
    gState.time += elapsedTime;

    GameObject::UpdateAll( gState );

    Play::DrawBackground();
    GameObject::DrawAll( gState );
    Play::DrawFontText( "105px", "SCORE: " + std::to_string( gState.score ), { DISPLAY_WIDTH / 2, 50 }, Play::CENTRE );
    
    Play::PresentDrawingBuffer();
    return Play::KeyDown( VK_ESCAPE );
}

// MainGameExit gets called once when the player quits the game 
// It needs to destroy everything, including the manager
int MainGameExit( void )
{
    GameObject::DestroyAll();
    Play::DestroyManager();
    return PLAY_OK;
}



