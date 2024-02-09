#pragma once

class PolygonEmitter : public Emitter
{
protected:
	std::vector<Play::Point2f> m_v_corners;
	std::vector<Play::Point2f> m_v_corners_world;
	Play::Vector2f m_perpUnitVect;
	bool m_deleteOutOfBoundsParticles = true;

public:
	PolygonEmitter();
	PolygonEmitter(ParticleShape shape, std::vector<Play::Point2f> corners /* anticlockwise */, bool deleteParticles);
	~PolygonEmitter();
	void Update(float elapsedTime) override;
	Particle ChooseParticleLocationAndInitialVelocity() override;
};