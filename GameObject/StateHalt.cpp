#include "Play.h"
#include "Agent8.h"
#include "StateHalt.h"

void StateHalt::Update( Agent8* player )
{
	player->SetVelocity( player->GetVelocity() * 0.9f );

	int maxFrames = Play::GetSpriteFrames( player->GetSpriteID() ) - 1;

	if( player->GetFrame() >= maxFrames )
		player->SetStateIndex( STATE_PLAY );
}