#include "Play.h"
#include "Laser.h"
#include "ObjectManager.h"
#include "Agent8.h"

void Laser::Update()
{
	StandardMovementUpdate();

	if( IsOutsideDisplay() )
		m_destroy = true;
}

void Laser::OnCollision( GameObject* other )
{
	Agent8* pPlayer = (Agent8*)GetObjectManager()->GetPlayer();

	switch( other->GetObjectType() )
	{
	case TYPE_TOOL:
		other->Destroy();
		Destroy( true );
		pPlayer->AddScore( 100 );
		break;

	case TYPE_COIN:
		other->Destroy();
		Destroy( true );
		Play::PlayAudio( "snd_error" );
		pPlayer->AddScore( -300 );

		if( pPlayer->GetScore() < 0 )
			pPlayer->SetScore( 0 );

		break;
	}
}