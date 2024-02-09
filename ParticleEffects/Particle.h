#pragma once

enum ParticleShape
{
	CIRCLE = 0,
	CLOUD,
	DISC,
	EXPLOSION,
	FLARE,
	FLUID,
	LINE,
	PIXEL,
	RING,
	SMOKE,
	SNOW,
	SPARK,
	SPHERE,
	SQUARE,
	STAR
};

extern const char* ShapeSprites[15];

struct Particle
{
	ParticleShape shape;
	const char* spriteName{ "" };
	Play::Point2f pos{ 0.f, 0.f };
	Play::Point2f velocity{ 0.f, 0.f };
	Play::Point2f acceleration{ 0.f, 0.f };
	float rotation{ 0.f };
	float rotSpeed{ 0.f };
	float scale{ 1.f };
	float lifetime = 2.f;
	float currentTime = 0.f;
	bool isActive = true;
	float directionWiggle = 0.0f;
	Play::Colour colour{ 100.f, 100.f, 100.f };

	Particle(ParticleShape _shape, Play::Point2f _pos, Play::Point2f _velocity, Play::Point2f _acceleration, float _rotSpeed, Play::Colour _colour);
};