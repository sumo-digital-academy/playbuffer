#include "Play.h"
#include "Fan.h"
#include "ObjectManager.h"

Fan::Fan( GameObjectType objType, Play::Point2f pos, Play::Vector2f velocity, std::string spriteName )
	: GameObject( objType, pos, velocity, spriteName )
{
	m_animSpeed = 1.0f;
}

void Fan::Update()
{
	if( Play::RandomRoll( 50 ) == 50 )
	{
		GetObjectManager()->CreateObject( TYPE_TOOL, m_pos, { -4, 0 } );
		Play::PlayAudio( "snd_tool" );
	}

	if( Play::RandomRoll( 150 ) == 1 )
	{
		GetObjectManager()->CreateObject( TYPE_COIN, m_pos, { -3, 0 } );
	}

	StandardMovementUpdate(); 
	UpdateAnimation(); 

	if( IsLeavingDisplay() )
	{
		m_pos = m_oldPos;
		m_velocity.y *= -1;
	}
}