#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;

struct GameState
{
	int score =0;
};
GameState gameState;

enum GameObjectType
{
 TYPE_NULL = -1,
 TYPE_AGENT8,
};

void HandlePlayerControls();
// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::StartAudioLoop("music");
	Play::CreateGameObject(TYPE_AGENT8, { 115, 0 }, 50, "agent8");
}


// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	Play::DrawBackground();
	HandlePlayerControls();
	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}


void HandlePlayerControls()
{
	GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
	if (Play::KeyDown(VK_UP))
	{
		obj_agent8.velocity = { 0, -4 };
		Play::SetSprite(obj_agent8, "agent8_climb", 0.25f);
	}
	else if (Play::KeyDown(VK_DOWN))
	{
		obj_agent8.acceleration = { 0, 1 };
		Play::SetSprite(obj_agent8, "agent8_fall", 0);
	}
	else
	{
		Play::SetSprite(obj_agent8, "agent8_hang", 0.02f);
		obj_agent8.velocity *= 0.5f;
		obj_agent8.acceleration = { 0, 0 };
	}
	Play::UpdateGameObject(obj_agent8);
	if (Play::IsLeavingDisplayArea(obj_agent8))
		obj_agent8.pos = obj_agent8.oldPos;
	Play::DrawLine({ obj_agent8.pos.x, 0 }, obj_agent8.pos, Play::cWhite);
	Play::DrawObjectRotated(obj_agent8);
}