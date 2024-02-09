#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Common.h"
#include "MainGame.h"
#include "Particle.h"
#include "Emitter.h"
#include "ParticleManager.h"

//****************************************************************************************
//
//                            Particle Effects PlayBuffer Demo
//                        by Duncan Gallagher, Sumo Digital Academy
// 
//****************************************************************************************

int DISPLAY_WIDTH = 720;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;
Point2f CENTRE_POINT{ DISPLAY_WIDTH * 0.5f, DISPLAY_HEIGHT * 0.5f };

const Vector2f GRAVITY{ 0.f, 0.5f };

bool DEBUG = false;

int g_portalEmitterID{ -1 };

void ManageInput();

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::LoadBackground( "Data\\Backgrounds\\background_square.png" );
	Play::CentreAllSpriteOrigins();
	
	ParticleManager::Instance(100);

	g_portalEmitterID = ParticleManager::Instance().CreateEmitter(EmitterType::PORTAL, Play::GetMousePos(), 0.f, 150.f);
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	static float t = 0.0f;
	t += elapsedTime;

	Play::DrawBackground();

	Play::SetDrawingBlendMode( Play::BLEND_ADD );

	ManageInput();

	ParticleManager::Instance().UpdateEmitters(elapsedTime);
	ParticleManager::Instance().DrawParticles();

	Play::PresentDrawingBuffer();

	return Play::KeyDown( KEY_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

void ManageInput()
{
	static bool prevMouseState;
	
	Emitter* emitter = ParticleManager::Instance().GetEmitter(g_portalEmitterID);
	if (emitter)
		emitter->SetPosition(Play::GetMousePos());

	// Specific reference to portal

	prevMouseState = Play::GetMouseButton(Play::LEFT);

	if (Play::KeyPressed(Play::KEY_F1))
	{
		DEBUG = !DEBUG;
	}
}