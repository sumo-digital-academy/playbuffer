#include "Play.h"
#include "Star.h"

Star::Star(GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName)
	: GameObject(objType, position, velocity, spriteName) 
{
	m_rotation = 0.1f;
	m_acceleration = { 0.0f, -0.5f };
}

void Star::Update()
{
	StandardMovementUpdate();

	if (IsOutsideDisplay())
		m_destroy = true;
}