#include "Play.h"

#include "MainGame.h"
#include "Player.h"
#include "Laser.h"
#include "Saucer.h"

Player::Player( Point2f pos ) : GameObject( pos )
{
	m_type = OBJ_PLAYER;
	m_drawOrder = PLAYER_DEPTH;
	m_spriteId = Play::GetSpriteId( "Rocket" );
}

void Player::Update( GameState& state )
{
	if( Play::KeyDown( 'A' ) )
		m_worldMat.row[2] -= m_worldMat.row[0] * PLAYER_SPEED;

	if( Play::KeyDown( 'D' ) )
		m_worldMat.row[2] += m_worldMat.row[0] * PLAYER_SPEED;

	if( Play::KeyDown( 'W' ) )
		m_worldMat.row[2] -= m_worldMat.row[1] * PLAYER_SPEED; // up the screen is -ve

	if( Play::KeyDown( 'S' ) )
		m_worldMat.row[2] += m_worldMat.row[1] * PLAYER_SPEED; // down the screen is +ve

	if( Play::KeyDown( 'Q' ) )
		m_worldMat = m_worldMat * MatrixRotation( -0.1f ); // anti-clockwise

	if( Play::KeyDown( 'E' ) )
		m_worldMat = m_worldMat * MatrixRotation( 0.1f ); // clockwise

	if( Play::KeyPressed( VK_SPACE ) )
	{
		new Laser( m_worldMat );
		Play::PlayAudio( "laser" );
	}

	if( GameObject::GetObjectCount( GameObject::OBJ_SAUCER ) == 0 )
		Saucer::SpawnSaucers( state );
}

void Player::Draw( GameState& state ) const
{
	Matrix2D wobbleMat = m_worldMat;
	wobbleMat.row[2].y += sin( state.time * PLAY_PI ) * 3;

	Play::DrawSpriteTransformed( m_spriteId, wobbleMat, (int)(2.0f * state.time) );
}

