#pragma once
#include "GameObject.h"

class Tools : public GameObject
{
public:
	Tools( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName );
	~Tools() {};

	// Virtual Update function which must be overridden by all children of GameObject
	void Update() override;
};

