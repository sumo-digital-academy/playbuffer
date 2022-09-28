#pragma once
#include "GameObject.h"

const int SAUCER_SCORE = 1000;
const int LASER_COST = 100;
const int LASER_SPEED = 20;
const int LASER_COLLISION_DISTANCE = 50;

// Very simple Laser class derived from GameObject
class Laser : public GameObject
{
public:
    // Constructs a new Laser
    Laser( Matrix2D& mat );
    // Virtual function to update the Laser
    void Update( GameState& gState ) override;
    // Virtual function to draw the Laser
    void Draw( GameState& gState ) const override;

private:

};

