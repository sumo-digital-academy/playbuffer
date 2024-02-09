#pragma once
#include "GameObject.h"

class Fan : public GameObject
{
public:
	Fan( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName );
	~Fan() {};

	// Virtual Update function which must be overridden by all children of GameObject
	void Update() override;
};