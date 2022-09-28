#include "Play.h"

#include "MainGame.h"
#include "Laser.h"
#include "Saucer.h"

Saucer::Saucer( Point2f pos ) : GameObject( pos )
{
    m_type = OBJ_SAUCER;
    m_spriteId = Play::GetSpriteId( "Saucer" );
}

void Saucer::Update( GameState& state )
{
    if( !m_hit )
    {
        m_velocity.x = -SAUCER_SPEED_INCREMENT * state.difficulty;
        m_velocity.y = sin( m_worldMat.row[2].x / 100 ) * 3;

        if( m_worldMat.row[2].x < -50 )
            m_worldMat.row[2] = { SAUCER_START_X, SAUCER_START_Y, 1.0f }; // remember to include the w
    }
    else
    {
        m_velocity.x = -SAUCER_SPEED_INCREMENT * state.difficulty;
        m_velocity.y += 1;
        m_rot += 0.1f * state.difficulty;
          
        if( m_worldMat.row[2].y > DISPLAY_HEIGHT )
            m_active = false;
    }

    m_worldMat.row[2] += m_velocity;
}

void Saucer::Draw( GameState& ) const
{
    Play::DrawSpriteTransformed( m_spriteId, m_worldMat, 0 );
}

void Saucer::SpawnSaucers( GameState& state )
{
    for( int n = 0; n < state.difficulty + SAUCER_WAVE_SIZE; n++ )
    {
        new Saucer( { SAUCER_START_X + ( n * 100 ), SAUCER_START_Y } );
    }
    state.difficulty++;
    Play::PlayAudio( "reward" );
}




