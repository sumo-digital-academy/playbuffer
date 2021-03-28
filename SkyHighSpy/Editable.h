#pragma once

// Function declarations from Editable.cpp
bool IsRocketTutorial();

int DefaultNumAsteroids();
int DefaultNumMeteors();

float DefaultAsteroidSpeed();
float DefaultPlayerSpeed();
float DefaultMeteorSpeed();

void UpdatePlayerMovement( float& x, float& y, float& angle, float& speed );
void UpdateLaserMovement( float& x, float& y, float& angle, float& speed );
bool IsLaserCollidingWithAsteroid( float laserX, float laserY, float asteroidX, float asteroidY );
extern void CreateLaser( float x, float y, float angle, float speed );

