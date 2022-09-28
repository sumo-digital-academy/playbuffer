#pragma once
#include "GameObject.h"

const float SAUCER_SPEED_INCREMENT = 5.0f;
const int SAUCER_WAVE_SIZE = 5;
const float SAUCER_START_X = 1500.0f;
const float SAUCER_START_Y = 125.0f;

// A Saucer class derived from GameObject
class Saucer : public GameObject
{
public:
    // Constructs a new Saucer
    Saucer( Point2f pos );
    // Virtual function to update the Saucer
    void Update( GameState& state ) override;
    // Virtual function to draw the Saucer
    void Draw( GameState& gState ) const override;
    // Make the Saucer fall out of the sky when it is hit
    void SetHit( bool hit ) { m_hit = hit; }
    // Gets whether the Saucer has been hit
    bool GetHit() const { return m_hit; }

    // Spawn a new wave of saucers when none are left
    static void SpawnSaucers( GameState& state );

private:

    // The rotation angle of the Saucer
    float m_rot{ 0 };
    // Makes the Saucer fall out of the sky
    bool m_hit{ false };
};
