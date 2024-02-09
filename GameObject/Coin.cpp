#include "Play.h"
#include "Coin.h"

Coin::Coin( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName )
	: GameObject( objType, position, velocity, spriteName )
{
	m_rotSpeed = 0.1f;
}

void Coin::Update()
{
	StandardMovementUpdate();

	if( IsOutsideDisplay() )
		m_destroy = true;
}