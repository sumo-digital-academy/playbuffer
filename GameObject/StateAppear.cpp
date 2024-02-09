#include "Play.h"
#include "Agent8.h"
#include "StateAppear.h"

void StateAppear::Update( Agent8* player )
{
	player->SetVelocity( { 0, -12 } );
	player->SetAcceleration( { 0, -0.5f } );
	player->SetSprite( "agent8_fall", 0.0f );
	player->SetRotation( 0 );

	if( player->GetPosition().y <= (Play::Window::GetHeight() * 2) / 3 )
		player->SetStateIndex( STATE_PLAY );
}