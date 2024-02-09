#pragma once
#include "GameObject.h"

class Laser : public GameObject
{
public:
	Laser( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName ) : GameObject( objType, position, velocity, spriteName ) {};
	~Laser() {};

	// Virtual Update function which must be overridden by all children of GameObject
	void Update() override;
	void OnCollision( GameObject* other ) override;
};