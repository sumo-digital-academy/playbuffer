#pragma once
#include "GameObject.h"

class Coin : public GameObject
{
public:
	Coin( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName );
	~Coin() {};

	// Virtual Update function which must be overridden by all children of GameObject
	void Update() override;
};