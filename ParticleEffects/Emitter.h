#pragma once
// #include "Particle.h" // Colour, Point2f, Vector2f

enum GravityDirection
{  
	UP = 0,
	DOWN
};

class Emitter
{

protected:

	int m_id{ -1 };
	
	// Shape and Position
	ParticleShape m_shape{  };
	Play::Point2f m_pos;

	// Gravity
	bool m_useGravity{ false };
	GravityDirection m_gravityDirection{ DOWN };
	float m_gravityMultiplier{ 1.f };

	// Spawn Characteristics
	bool m_emitContinuously{ true };
	float m_spawnTimer{ 0.f };
	float m_spawnRate{ 20.0f }; // Particles per second

	// Lifetime
	float m_minParticleLifetime{ 0.75f };
	float m_maxParticleLifetime{ 1.25f };

	// Emitter Lifetime
	float m_lifetime{ 0.f };
	float m_currentTime{ 0.f };

	// Rotation
	float m_minRotSpeed{ 0.f };
	float m_maxRotSpeed{ 0.f };
	float m_rotIncrement{ 0.f };
	bool m_velocityInOrientationDirection{ false };

	// Velocity
	float m_minStartVelocity{ 1.f };
	float m_maxStartVelocity{ 10.f };

	// Colour
	Play::Colour m_startColour{ 100.f, 100.f, 100.f };
	float m_startColourVariation{ 1.f };
	float m_colorWiggle{ 1.f };

	// Scale
	float m_minScale{ 0.5f };
	float m_maxScale{ 2.f };
	float m_increment{ 0.f };
	float m_scaleWiggle{ 0.f };

	// Direction
	float m_minEmissionDirection{ 0.f };
	float m_maxEmissionDirection{ 360.f };
	float m_directionIncrement{ 0.f };
	float m_directionWiggle{ 0.f };

	// Opacity
	float m_startOpacity{ 1.f };
	float m_endOpacity{ 0.f };

	// Special
	bool m_particleLine{ false };
	Play::Vector2f m_particleLineVector{ 10.f, 10.f };
	Play::Vector2f m_initialPush{ 1.f, 1.f };
	bool m_flock{ false };

public:
	Emitter();
	Emitter(ParticleShape _shape, Play::Point2f _pos, bool _emitContinously, bool _useGravity);
	~Emitter();

	int GetID();

	void DestroyParticle();

	void Start();
	void Stop();

	// Setter methods
	void SetScale(float min, float max, float incr, float wig);
	void SetVelocity(float minStartVel, float maxStartVel);
	void SetColour(Play::Colour col, float randStartVariation, float wiggle);
	void SetSpawnRate(float sr);
	void SetDirection(float min, float max, float incr, float wiggle);
	void SetGravity(bool state, GravityDirection dir, float gravityMult);
	void SetOpacity(float start, float end);
	void SetParticleLifetime(float min, float max);
	void SetParticleLineProperties(bool state, Play::Vector2f lineVector, Play::Vector2f initPush);
	void SetRotation(float minSpeed, float maxSpeed, float increment, bool alignToVelocity);
	void SetPosition(Play::Point2f pos);
	bool LifetimeEnded();

	virtual void CreateParticle();
	virtual void Update(float elapsedTime);
	virtual Particle ChooseParticleLocationAndInitialVelocity();

	void DrawParticles();

protected:
	std::vector<Particle> v_Particles{};

};