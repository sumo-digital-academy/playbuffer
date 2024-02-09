#pragma once
#include "GameObject.h"

// The state index is the only representation of state which is visible to code externally
enum Agent8StateIndex
{
	STATE_APPEAR = 0,
	STATE_HALT,
	STATE_PLAY,
	STATE_DEAD,
	TOTAL_STATES
};

class State;

class Agent8 : public GameObject
{
public:
	// Constructor and Destructor
	Agent8( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName );
	~Agent8();

	// Virtual Update function which must be overridden by all children of GameObject
	void Update() override;
	// Optional virtual override
	void OnCollision( GameObject* other ) override;

	// Ways to get and set the score
	void AddScore( int x ) { m_score += x; };
	void SetScore( int score ) { m_score = score; };
	int GetScore() { return m_score; };

	// The state index is the only representation of state which is visible to code externally
	void SetStateIndex( Agent8StateIndex newState );
	Agent8StateIndex GetStateIndex() { return m_stateIndex; }

private:
	int m_score{ 0 };

	// The state classes and their operation are encapsulated internally
	State* m_pCurrentState;
	State* m_pAllStates[ TOTAL_STATES ];

	Agent8StateIndex m_stateIndex{ STATE_APPEAR };
};

