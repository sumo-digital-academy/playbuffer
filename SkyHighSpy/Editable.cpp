#include "Play.h"
#include "Editable.h"

// Return true to swap the spider for the rocket, false for normal game play
bool IsRocketTutorial() 
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

// Controls the initial movement speed of the spider when flying
float DefaultPlayerSpeed()
{
	return 10.0;
}

// Controls the movement speed of asteroids
float DefaultAsteroidSpeed()
{
	return 4.0;
}

// Controls the movement speed of fiery meteors
float DefaultMeteorSpeed()
{
	return 10.0;
}

// Controls the movement of the rocket ship in the tutorial mode
void UpdatePlayerMovement( float& x, float& y, float& angle, float& speed )
{

}

// Controls the movement of lasers in the tutorial mode
void UpdateLaserMovement( float& x, float& y, float& angle, float& speed )
{

}

// Tests to see if a laser has collided with an asteroid
bool IsLaserCollidingWithAsteroid( float laserX, float laserY, float asteroidX, float asteroidY )
{
	float xDiff = asteroidX - laserX;
	float yDiff = asteroidY - laserY;

	// Can you declare a new float variable called dist and us it to calculate the distance?

	// Then you need to return true if dist is small enough to be considered to be colliding.

	return false;
	// Don't put any code after the return - it won't do anything!
}
