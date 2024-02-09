#include "Play.h"
#include "Agent8.h"
#include "ObjectManager.h"

// This compilation unit needs to know about all the states in order to create them
#include "State.h"
#include "StateAppear.h"
#include "StateHalt.h"
#include "StatePlay.h"
#include "StateDead.h"

static constexpr int STAR_SPEED = 16;

Agent8::Agent8( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName )
	: GameObject( objType, position, velocity, spriteName )
{
	GetObjectManager()->SetPlayer( this );

	// Allocation of state objects
	m_pAllStates[ STATE_APPEAR ] = new StateAppear();
	m_pAllStates[ STATE_HALT ] = new StateHalt();
	m_pAllStates[ STATE_PLAY ] = new StatePlay();
	m_pAllStates[ STATE_DEAD ] = new StateDead();

	SetStateIndex( STATE_APPEAR );
}

Agent8::~Agent8()
{
	// Freeing of state objects
	for( int s = 0; s < TOTAL_STATES; s++ )
		delete m_pAllStates[ s ];
}

void Agent8::SetStateIndex( Agent8StateIndex newState )
{
	PLAY_ASSERT_MSG( newState >= 0 && newState < TOTAL_STATES, "Error: Invalid state!" );
	m_stateIndex = newState;
	m_pCurrentState = m_pAllStates[ newState ];
}

void Agent8::Update()
{
	// Delegates the Update function to the current state
	m_pCurrentState->Update( this );

	StandardMovementUpdate(); // Common update from GameObject base class

	if( GetStateIndex() != STATE_APPEAR && GetStateIndex() != STATE_DEAD )
	{
		if( IsLeavingDisplay() )
			m_pos = m_oldPos;
	}

	UpdateAnimation();

	Play::DrawLine( { m_pos.x, Play::Window::GetHeight() }, m_pos, Play::cWhite );
}

void Agent8::OnCollision( GameObject* other )
{
	switch( other->GetObjectType() )
	{
	case TYPE_TOOL:
		Play::StopAudio( "snd_music" );
		Play::PlayAudio( "snd_die" );
		SetStateIndex( STATE_DEAD );
		other->Destroy( true );
		break;

	case TYPE_COIN:
		if( m_stateIndex != STATE_DEAD )
		{
			for( float rad{ 0.25f }; rad < 2.0f; rad += 0.5f )
			{
				GameObject* star = GetObjectManager()->CreateObject( TYPE_STAR, other->GetPosition(), { 0, 0 } );

				float angle = rad * Play::PLAY_PI;
				star->SetVelocity( { STAR_SPEED * sin( angle ), STAR_SPEED * -cos( angle ) } );
			}

			AddScore( 500 );
			Play::PlayAudio( "snd_collect" );
			other->Destroy( true );
		}
		break;
	}
}