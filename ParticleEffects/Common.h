#pragma once
// --- //

struct Play::Vector2f; // Forward Declaration

Play::Vector2f RotateVector(Play::Vector2f, float);
Play::Vector2f NormaliseVector(Play::Vector2f vector);

float Magnitude(Play::Vector2f);

float AngleBetweenTwoVectors(Play::Vector2f a, Play::Vector2f b);

float RandomFloat(float a, float b);

// --- //