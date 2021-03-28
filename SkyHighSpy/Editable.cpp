#include "Play.h"
#include "Editable.h"

// Return true for test environment, false for normal game play
bool IsTestEnvironment() 
{
	return false;
}

// Controls the number of asteroids (in level 1)
int DefaultNumAsteroids()
{
	return 2;
}

// Controls the number of fiery meteors (in level 1)
int DefaultNumMeteors()
{
	return 1;
}

// Controls the initial movement speed of the player when flying
float DefaultPlayerSpeed()
{
	return 10.0f;
}

// Controls the movement speed of asteroids
float DefaultAsteroidSpeed()
{
	return 4.0f;
}

// Controls the movement speed of fiery meteors
float DefaultMeteorSpeed()
{
	return 10.0f;
}

// Controls the movement of the rocket ship in the test environment
void UpdatePlayerMovement( float& x, float& y, float& angle, float& speed )
{
	
}

// Controls the movement of lasers in the test environment
void UpdateLaserMovement( float& x, float& y, float& angle, float& speed )
{

}

// Tests to see if a laser has collided with an asteroid
bool IsLaserCollidingWithAsteroid( float laserX, float laserY, float asteroidX, float asteroidY )
{
	return false;
}