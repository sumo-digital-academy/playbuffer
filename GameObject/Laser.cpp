#include "Play.h"

#include "MainGame.h"
#include "Laser.h"
#include "Saucer.h"

Laser::Laser( Matrix2D& mat ) : GameObject( mat )
{
	m_velocity = { 0, -LASER_SPEED };
	m_type = OBJ_LASER;
	m_spriteId = Play::GetSpriteId( "Laser" );
	m_drawOrder = 1;
}

void Laser::Update( GameState& state )
{
	std::vector< GameObject* > vSaucerList;
	GameObject::GetObjectList( GameObject::OBJ_SAUCER, vSaucerList );

	for( GameObject* p : vSaucerList )
	{
		Saucer* s = static_cast<Saucer*>( p );
		
		float  dist = ( m_worldMat.row[2].As2D() - s->GetPosition() ).Length();

		if( s->GetHit() == false &&  dist < LASER_COLLISION_DISTANCE )
		{
			s->SetHit( true );
			state.score += SAUCER_SCORE;
			Play::PlayAudio( "clang" );
			m_active = false;
		}
	}

	m_worldMat.row[2] -= m_worldMat.row[1] * LASER_SPEED; // -ve is up the screen

	if( m_active && m_worldMat.row[2].y < 0 )
	{
		m_active = false;

		if( state.score >= LASER_COST )
			state.score -= LASER_COST;
	}
}

void Laser::Draw( GameState& ) const
{
	Play::DrawSpriteTransformed( m_spriteId, m_worldMat, 0 );
}


