#pragma once
#include "GameObject.h"

class Star : public GameObject
{
public:
	Star( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName );
	~Star() {};

	// Virtual Update function which must be overridden by all children of GameObject
	void Update() override;
};