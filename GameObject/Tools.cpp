#include "Play.h"
#include "Tools.h"

Tools::Tools( GameObjectType objTypes, Play::Point2f position, Play::Vector2f velocity, std::string sprtieName )
	: GameObject( objTypes, position, velocity, sprtieName )
{
	if( Play::RandomRoll( 2 ) == 1 )
	{
		m_spriteID = Play::GetSpriteId( "spanner" );
		m_velocity = { -4, 0 };
		m_rotSpeed = 0.1f;
	}
	else
	{
		m_spriteID = Play::GetSpriteId( "driver" );
		m_velocity = { -8, Play::RandomRollRange( -1, 1 ) * 6 };
	}
}

void Tools::Update()
{
	StandardMovementUpdate();
	UpdateAnimation();

	if( IsLeavingDisplay( true, false ) ) 
	{
		m_pos = m_oldPos;
		m_velocity.y *= -1;
	}

	if( IsOutsideDisplay() )
		m_destroy = true;
}