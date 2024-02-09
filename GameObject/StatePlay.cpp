#include "Play.h"
#include "Agent8.h"
#include "StatePlay.h"
#include "ObjectManager.h"

void StatePlay::Update( Agent8* player )
{
	if( Play::KeyDown( Play::KEY_UP ) )
	{
		player->SetVelocity( { 0, 4 } );
		player->SetSprite( "agent8_climb", 0.25f );
	}
	else if( Play::KeyDown( Play::KEY_DOWN ) )
	{
		player->SetAcceleration( { 0, -1 } );
		player->SetSprite( "agent8_fall", 0.0f );
	}
	else
	{
		if( player->GetVelocity().y > 5 )
		{
			player->SetStateIndex( STATE_HALT );
			player->SetSprite( "agent8_halt", 0.333f );
			player->SetAcceleration( { 0, 0 } );
		}
		else
		{
			player->SetSprite( "agent8_hang", 0.02f );
			player->SetVelocity( player->GetVelocity() * 0.5f );
			player->SetAcceleration( { 0, 0 } );
		}
	}

	if( Play::KeyPressed( Play::KEY_SPACE ) )
	{
		Play::Vector2D firePos = player->GetPosition() + Play::Vector2D( 155, 75 );
		GetObjectManager()->CreateObject( TYPE_LASER, firePos, { 32, 0 } );
		Play::PlayAudio( "snd_shoot" );
	}
}