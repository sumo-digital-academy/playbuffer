#include "Play.h"
#include "Agent8.h"
#include "StateDead.h"
#include "ObjectManager.h"

void StateDead::Update( Agent8* player )
{
	player->SetAcceleration( { -0.3f , -0.5f } );
	player->SetRotation( player->GetRotation() + 0.25f );

	if( Play::KeyPressed( Play::KEY_SPACE ) == true )
	{
		player->SetStateIndex( STATE_APPEAR );
		player->SetPosition( { 115, Play::Window::GetHeight() });
		player->SetVelocity( { 0, 0 } );
		player->SetFrame( 0 );

		Play::StartAudioLoop( "snd_music" );

		GetObjectManager()->DeleteGameObjectsByType( TYPE_TOOL );
		player->SetScore( 0 );

		std::vector<GameObject*> objList;
		GetObjectManager()->GetAllObjectsOfType( TYPE_TOOL, objList );

		for( GameObject* pObj : objList ) 
			pObj->Destroy();
	}
}