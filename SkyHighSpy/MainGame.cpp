#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;
constexpr int WRAP_BORDER = 200;

constexpr int ORIGIN_OFFSET_ASTEROID = 10;
constexpr int ORIGIN_OFFSET_METEOR = 34;
constexpr int ORIGIN_OFFSET_AGENT = -60;

constexpr int TEXT_SUBMIDDLE_X = 623;
constexpr int TEXT_SUBMIDDLE_Y = 260;
constexpr int TEXT_BOTTOM_Y = 50;

constexpr int GEM_RADIUS = 20;
constexpr int ROCK_RADIUS = 60;

//Sprite Names
const char* spr_agentLeft = "agent8_left_7";
const char* spr_agentRight = "agent8_right_7";
const char* spr_fly = "agent8_fly";
const char* spr_dead = "agent8_dead_2";

const char* spr_asteroid = "asteroid_2";
const char* spr_pieces = "asteroid_pieces_3";
const char* spr_meteor = "meteor_2";
const char* spr_gem = "silver_gem";
const char* spr_ring = "blue_ring";
const char* spr_particle = "white_particle";

const char* font_large = "72px";
const char* font_small = "32px";

enum Types
{
	TYPE_NULL = -1,
	TYPE_AGENT8,
	TYPE_ASTEROID,
	TYPE_GEM,
	TYPE_PIECES,
	TYPE_METEOR,
	TYPE_PARTICLES,
	TYPE_WAITING,
};


enum Agent8States
{
	STATE_FLYING = 0,
	STATE_ATTACHED,
	STATE_DEAD,
	STATE_START,
	STATE_COMPLETE,
};

struct GameState
{
	Agent8States agentStates = STATE_START;
	int score = 0;
	int level = 1;
	int gemNumber = level;
	int gemsSpawned = 0;
	int attached_id = -1;
};

GameState gameState;

void UpdateAgent();
void StateAttached(GameObject& obj_agent);
void StateFlying(GameObject& obj_agent);
void StateDead(GameObject& obj_agent);
void StateStart(GameObject& obj_agent);
void StateWin();
void Collision(GameObject& obj_agent);

void UpdateUI();

void SpawnAsteroids();
void SpawnMeteors();
void SpawnPieces(GameObject& object);
void SpawnGems(GameObject& object);
void SpawnParticles();

void UpdateRock();
void UpdatePieces();
void UpdateGems();
void UpdateParticles();

void Restart();

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::StartAudioLoop("music");

	//Spawn Player and set origin for crawling sprites
	Play::CreateGameObject(TYPE_AGENT8, { 0,0 }, 50, spr_agentLeft);
	Play::MoveSpriteOrigin(spr_agentLeft, ORIGIN_OFFSET_AGENT, 0);
	Play::MoveSpriteOrigin(spr_agentRight, ORIGIN_OFFSET_AGENT, 0);
	Play::MoveSpriteOrigin(spr_asteroid, ORIGIN_OFFSET_ASTEROID, 0);
	Play::MoveSpriteOrigin(spr_meteor, ORIGIN_OFFSET_METEOR, 0);

	//Platforms and hazards
	Restart();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	Play::DrawBackground();
	UpdateRock();
	UpdatePieces();
	UpdateGems();
	UpdateParticles();
	UpdateAgent();
	UpdateUI();

	Play::PresentDrawingBuffer();
	return Play::KeyDown( Play::KEY_ESCAPE );
}

void SpawnAsteroids()
{
	std::vector<int> vAsteroids;

	//Asteroids equal to the level number
	for (int i = 0; i < gameState.level; i++)
	{
		int pos_x, pos_y;

		if( i == 0 ) // Make sure the player's starting asteroid is visible
		{
			pos_x = DISPLAY_WIDTH / 2;
			pos_y = DISPLAY_HEIGHT / 2;
		}
		else
		{
			//Randomly set position and rotation
			pos_x = Play::RandomRoll(DISPLAY_WIDTH);
			pos_y = Play::RandomRoll(DISPLAY_HEIGHT);
		}

		int id_rock = Play::CreateGameObject(TYPE_ASTEROID, { pos_x, pos_y }, ROCK_RADIUS, spr_asteroid);
		vAsteroids.push_back(id_rock);

		//Random rotation between 0-2pi radians (360 degrees)
		float rotation = (((float)rand() / RAND_MAX) * (PLAY_PI * 2));
		GameObject& obj_rock = Play::GetGameObject(id_rock);
		obj_rock.rotation = rotation;
		obj_rock.animSpeed = 0.05f;
	}

	//Choose the first asteroid for the player to stand on
	int index_attached = 0;
	gameState.attached_id = vAsteroids.at(index_attached);
}

void SpawnMeteors()
{
	//Half as many meteors, starts on level 2
	for (int i = 1; i <= gameState.level / 2; i++)
	{
		//Random position
		int pos_x = Play::RandomRoll(DISPLAY_WIDTH);
		int pos_y = Play::RandomRoll(DISPLAY_HEIGHT);
		int id_meteor = Play::CreateGameObject(TYPE_METEOR, { pos_x, pos_y }, ROCK_RADIUS, spr_meteor);
		GameObject& obj_meteor = Play::GetGameObject(id_meteor);

		float rotation = (((float)rand() / RAND_MAX) * (PLAY_PI * 2));
		obj_meteor.rotation = rotation;
		obj_meteor.animSpeed = 0.05f;

		std::vector<int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);
		for (int id : vAsteroids)
		{
			GameObject& obj_rock = Play::GetGameObject(id);
			if (Play::IsColliding(obj_meteor, obj_rock))
			{
				obj_meteor.pos.x += ROCK_RADIUS * sin(obj_meteor.rotation);
				obj_meteor.pos.y += ROCK_RADIUS * cos(obj_meteor.rotation);
			}
		}
	}
}

void UpdateRock()
{
	std::vector<std::vector<int>> vRocks =
	{
		{Play::CollectGameObjectIDsByType(TYPE_METEOR)},
		{Play::CollectGameObjectIDsByType(TYPE_ASTEROID)}
	};
	for (std::vector<int> vec : vRocks)
	{
		for (int id : vec)
		{
			GameObject& obj_rock = Play::GetGameObject(id);
			Play::SetGameObjectDirection(obj_rock, 4, obj_rock.rotation);
			Play::UpdateGameObject(obj_rock, true, WRAP_BORDER);
			Play::DrawObjectRotated(obj_rock);
		}
	}
}

void UpdateAgent()
{
	GameObject& obj_agent = Play::GetGameObjectByType(TYPE_AGENT8);

	switch (gameState.agentStates)
	{
	case STATE_FLYING:
		StateFlying(obj_agent);
		Collision(obj_agent);
		break;
	case STATE_ATTACHED:
		StateAttached(obj_agent);
		break;
	case STATE_DEAD:
		StateDead(obj_agent);
		break;
	case STATE_START:
		StateAttached( obj_agent ); // Start state is the same as Attached, but with instructions
		break;
	case STATE_COMPLETE:
		StateFlying(obj_agent);
		StateWin();
		break;
	}

	Play::UpdateGameObject(obj_agent, true, WRAP_BORDER);

	// Overflow checks
	if (obj_agent.rotation < 0)
		obj_agent.rotation += 2 * PLAY_PI;
	if (obj_agent.rotation > 2 * PLAY_PI)
		obj_agent.rotation -= 2 * PLAY_PI;

	Play::DrawObjectRotated(obj_agent);
}

void StateFlying(GameObject& obj_agent)
{
	Play::SetSprite(obj_agent, spr_fly, 1);
	Play::SetGameObjectDirection(obj_agent, 7, obj_agent.rotation);
	SpawnParticles();
	if (Play::KeyDown(Play::KEY_RIGHT))
	{
		obj_agent.rotSpeed = -0.05f;
	}
	else if (Play::KeyDown(Play::KEY_LEFT))
	{
		obj_agent.rotSpeed = 0.05f;
	}
	else
	{
		obj_agent.rotSpeed = 0;
	}
}

void Collision(GameObject& obj_agent)
{
	std::vector<int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);
	std::vector<int> vMeteors = Play::CollectGameObjectIDsByType(TYPE_METEOR);
	std::vector<int> vGems = Play::CollectGameObjectIDsByType(TYPE_GEM);

	//Landing
	for (int id : vAsteroids)
	{
		GameObject& obj_rock = Play::GetGameObject(id);
		if (Play::IsColliding(obj_agent, obj_rock))
		{
			gameState.agentStates = STATE_ATTACHED;
			Play::SetSprite(obj_agent, spr_agentLeft, 0);
			gameState.attached_id = id;
			obj_agent.pos = obj_rock.pos;
			Play::PointGameObject(obj_agent, (int)obj_agent.oldPos.x, (int)obj_agent.oldPos.y);
		}
	}

	//Hitting meteor
	for (int id : vMeteors)
	{
		GameObject& obj_meteor = Play::GetGameObject(id);
		if (Play::IsColliding(obj_agent, obj_meteor))
		{
			Play::PlayAudio("combust");
			gameState.agentStates = STATE_DEAD;
		}
	}

	//Collecting gems
	for (int id : vGems)
	{
		GameObject& obj_gem = Play::GetGameObject(id);
		if (Play::IsColliding(obj_agent, obj_gem))
		{
			gameState.score++;
			Play::PlayAudio("collect");
			int id_ring = Play::CreateGameObject(TYPE_PARTICLES, obj_gem.pos, 0, spr_ring);
			GameObject& obj_ring = Play::GetGameObject(id_ring);
			obj_ring.scale = 0.25;
			Play::DestroyGameObject(id);

			if (gameState.score == gameState.gemNumber)
			{
				gameState.agentStates = STATE_COMPLETE;
				Play::PlayAudio("reward");
			}
		}
	}
}

void StateAttached(GameObject& obj_agent)
{
	GameObject& obj_attached = Play::GetGameObject(gameState.attached_id);

	obj_agent.pos = obj_attached.pos;
	obj_agent.velocity = obj_attached.velocity;
	
	if (Play::KeyDown(Play::KEY_RIGHT))
	{
		obj_agent.rotSpeed = -0.05f;
		Play::SetSprite(obj_agent, spr_agentRight, 0.5);
	}
	else if (Play::KeyDown(Play::KEY_LEFT))
	{
		obj_agent.rotSpeed = 0.05f;
		Play::SetSprite(obj_agent, spr_agentLeft, 0.5);
	}
	else
	{
		obj_agent.rotSpeed = 0;
		obj_agent.animSpeed = 0;
	}

	//Launch
	if (Play::KeyPressed(Play::KEY_SPACE))
	{
		gameState.agentStates = STATE_FLYING;
		Play::PlayAudio("explode");
		obj_agent.pos.x += 60 * cos(obj_agent.rotation);
		obj_agent.pos.y += 60 * sin(obj_agent.rotation);
		obj_agent.animSpeed = 0.1f;
		SpawnPieces(obj_attached);
		SpawnGems(obj_attached);
		Play::DestroyGameObject(gameState.attached_id);
		gameState.attached_id = -1;
		Play::SetSprite( obj_agent, spr_fly, 1 );
	}
}

void StateDead(GameObject& obj_agent)
{
	Play::SetSprite(obj_agent, spr_dead, 0.1f );
	obj_agent.rotSpeed = 0;
	Play::SetGameObjectDirection(obj_agent, 10, obj_agent.rotation);

	//Restart current level
	if (Play::KeyPressed(Play::KEY_SPACE))
	{
		Restart();
		Play::SetSprite(obj_agent, spr_agentLeft, 0);
	}
}

void StateWin()
{
	if (Play::KeyPressed(Play::KEY_SPACE))
	{
		gameState.level++;
		gameState.gemNumber = gameState.level;
		Restart();
	}
}

void UpdateUI()
{
	switch (gameState.agentStates)
	{
	case STATE_ATTACHED:
		//Fallthrough
	case STATE_FLYING:
		Play::DrawFontText(font_small, "Gems = " + std::to_string(gameState.score), { 50,670 }, Play::LEFT);
		break;
	case STATE_DEAD:
		Play::DrawFontText(font_large, "Burned Out", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT/ 2 }, Play::CENTRE);
		Play::DrawFontText(font_small, "Collected " + std::to_string(gameState.score) + " Needed " + std::to_string(gameState.gemNumber), { TEXT_SUBMIDDLE_X, TEXT_SUBMIDDLE_Y }, Play::CENTRE);
		Play::DrawFontText(font_small, "Spacebar to Restart", { DISPLAY_WIDTH / 2, TEXT_BOTTOM_Y}, Play::CENTRE);
		break;
	case STATE_START:
		//Instructions
		Play::DrawFontText(font_large, "Level " + std::to_string(gameState.level), { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
		Play::DrawFontText(font_small, "Collect " + std::to_string(gameState.gemNumber) + " gem(s)", { TEXT_SUBMIDDLE_X, TEXT_SUBMIDDLE_Y }, Play::CENTRE);
		Play::DrawFontText(font_small, "Left and right keys to move, spacebar to jump", { DISPLAY_WIDTH / 2, TEXT_BOTTOM_Y }, Play::CENTRE);
		break;
	case STATE_COMPLETE:
		Play::DrawFontText(font_large, "Level " + std::to_string(gameState.level) + " Complete!", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
		Play::DrawFontText(font_small, "Spacebar to play next level", { TEXT_SUBMIDDLE_X, TEXT_SUBMIDDLE_Y }, Play::CENTRE);
	}
}

void SpawnPieces(GameObject& object)
{
	float rad = 0.25f;
	//Three pieces in opposite directions
	for (int i = 1; i <= 3; i++)
	{
		int id_piece = Play::CreateGameObject(TYPE_PIECES, object.pos, 0, spr_pieces);
		GameObject& obj_piece = Play::GetGameObject(id_piece);
		Play::SetGameObjectDirection(obj_piece, 10, rad);
		obj_piece.frame = i;
		rad += 0.5f;
	}

	//Particles
	for (int x = 0; x <= 5; x++)
	{
		SpawnParticles();
	}

}

void UpdatePieces()
{
	std::vector<int> vPieces = Play::CollectGameObjectIDsByType(TYPE_PIECES);
	for (int id : vPieces)
	{
		GameObject& obj_piece = Play::GetGameObject(id);
		Play::UpdateGameObject(obj_piece);
		Play::DrawObjectRotated(obj_piece);
		if (!Play::IsVisible(obj_piece))
		{
			Play::DestroyGameObject(id);
		}
	}
}

void SpawnGems(GameObject& object)
{
	//Only if total number of gems for level isn't reached yet
	if (gameState.gemsSpawned < gameState.gemNumber)
	{
		int id_gem = Play::CreateGameObject(TYPE_WAITING, object.pos, GEM_RADIUS, spr_gem);
		GameObject& obj_gem = Play::GetGameObject(id_gem);

		//Check in display area
		if (obj_gem.pos.y >= DISPLAY_HEIGHT)
		{
			obj_gem.pos.y = DISPLAY_HEIGHT - GEM_RADIUS;
		}
		if (obj_gem.pos.x >= DISPLAY_WIDTH)
		{
			obj_gem.pos.x = DISPLAY_WIDTH - GEM_RADIUS;
		}
		if (obj_gem.pos.y <= 0)
		{
			obj_gem.pos.y = GEM_RADIUS;
		}
		if (obj_gem.pos.x <= 0)
		{
			obj_gem.pos.x = GEM_RADIUS;
		}
		//Used for timer until visible
		obj_gem.animSpeed = 0.5;
		gameState.gemsSpawned++;
	}
}

void UpdateGems()
{
	//Update gems-in-waiting
	std::vector<int> vWait = Play::CollectGameObjectIDsByType(TYPE_WAITING);
	for (int id : vWait)
	{
		GameObject& obj_waiting = Play::GetGameObject(id);
		Play::UpdateGameObject(obj_waiting);
		//Delay before allowing agent collision
		if (obj_waiting.frame == 10)
		{
			obj_waiting.type = TYPE_GEM;
			obj_waiting.animSpeed = 0.0f;
			obj_waiting.rotSpeed = 0.05f;
		}
	}

	//Update gems
	std::vector<int> vGems = Play::CollectGameObjectIDsByType(TYPE_GEM);
	for (int id : vGems)
	{
		GameObject& obj_gem = Play::GetGameObject(id);
		Play::UpdateGameObject(obj_gem);
		Play::DrawObjectRotated(obj_gem);

		//Animation
		if (obj_gem.rotation >= 1 || obj_gem.rotation <= -1)
		{
			obj_gem.rotSpeed *= -1;
		}
	}
}

void SpawnParticles()
{
	GameObject& obj_agent = Play::GetGameObjectByType(TYPE_AGENT8);
	int posOffset_x = Play::RandomRoll(5);
	int posOffset_y = Play::RandomRoll(5);
	Play::CreateGameObject(TYPE_PARTICLES, { obj_agent.oldPos.x + posOffset_x, obj_agent.oldPos.y + posOffset_y }, 0, spr_particle);
}

void UpdateParticles()
{
	std::vector<int> vParticles = Play::CollectGameObjectIDsByType(TYPE_PARTICLES);
	for (int id : vParticles)
	{
		GameObject& obj_particles = Play::GetGameObject(id);
		//Update both gem rings and Agent8 particles
		if (Play::GetSpriteName(obj_particles.spriteId) == spr_ring)
		{
			obj_particles.scale += 0.1f;
		}
		else
		{
			obj_particles.scale -= 0.02f;
		}
		Play::UpdateGameObject(obj_particles);
		Play::DrawObjectRotated(obj_particles, obj_particles.scale);
		if (obj_particles.scale <= 0.05f || obj_particles.scale >= 1.5f )
		{
			Play::DestroyGameObject(id);
		}
	}
}

void Restart()
{
	GameObject& obj_agent = Play::GetGameObjectByType(TYPE_AGENT8);

	//Destroy objects
	std::vector<std::vector<int>> vDestroy =
	{
		{ Play::CollectGameObjectIDsByType(TYPE_ASTEROID) },
		{ Play::CollectGameObjectIDsByType(TYPE_METEOR) },
		{ Play::CollectGameObjectIDsByType(TYPE_GEM) },
		{ Play::CollectGameObjectIDsByType(TYPE_PIECES) },
	};
	for (std::vector<int> v : vDestroy)
	{
		for (int id : v)
		{
			Play::DestroyGameObject(id);
		}
	}

	//Spawn new asteroids an meteors
	SpawnAsteroids();
	SpawnMeteors();

	//Reset variables
	gameState.score = 0;
	gameState.gemsSpawned = 0;
	gameState.agentStates = STATE_START;
	Play::SetSprite(obj_agent, spr_agentLeft, 0);
	obj_agent.rotation = PLAY_PI / 2.0f;

	//Attach to asteroid and reset rotation
	GameObject& obj_attached = Play::GetGameObject( gameState.attached_id );
	obj_agent.pos = obj_attached.pos;
	obj_agent.velocity = obj_attached.velocity;
	obj_agent.rotSpeed = 0;
}

// Gets called once when the player quits the game
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}