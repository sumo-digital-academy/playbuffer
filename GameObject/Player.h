#pragma once
#include "GameObject.h"

const int PLAYER_SPEED = 5;
const int PLAYER_DEPTH = -100;

// A Player class derived from GameObject
class Player : public GameObject
{
public:
    // Constructs a new Player
    Player( Point2f pos );
    // Virtual function to update the Player
    void Update( GameState& gState ) override;
    // Virtual function to draw the Player
    void Draw( GameState& gState ) const override;

private:

};

