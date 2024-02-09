#pragma once

class Smoke : public Emitter
{
public:
	Smoke(Play::Point2f pos, float lifetime);
};

class Portal : public PolygonEmitter
{
public:
	Portal(Play::Point2f pos, float lifetime, float scale = 1.f);
};