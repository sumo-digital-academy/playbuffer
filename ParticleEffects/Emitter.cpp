#include "Play.h" // All the ones below will know about Play stuff
#include "MainGame.h"
#include "Particle.h"
#include "Emitter.h" 
#include "Common.h"

Emitter::Emitter()
{
	static int uniqueID = 0;
	m_id = uniqueID++;
}

Emitter::Emitter(ParticleShape _shape, Play::Point2f _pos, bool _emitContinously = true, bool _useGravity = false)
{
	m_shape = _shape;
	m_pos = _pos;
	m_emitContinuously = _emitContinously;
	m_useGravity = _useGravity;
}

Emitter::~Emitter()
{
	v_Particles.clear();
}

int Emitter::GetID()
{
	return m_id;
}

Particle Emitter::ChooseParticleLocationAndInitialVelocity()
{
	// Rotation
	float randRot = Play::RandomRollRange(static_cast<int>(m_minRotSpeed) * 100, static_cast<int>(m_maxRotSpeed) * 100) / 100.f;
	Play::RandomRoll(2) - 1 ? randRot *= -1 : randRot *= 1; // Just a coin flip to determine whether randRot is negative or positive

	// Magnitude
	float magnitude = RandomFloat(m_minStartVelocity, m_maxStartVelocity);  // (i.e.the radius or hypotenuse of the triange)

	// Determine the direction
	Play::Vector2f dir;

	if (!m_particleLine)
	{
		// Together the angle and the radius made a set of polar coordinates
		float randAngle = RandomFloat(Play::DegToRad(m_minEmissionDirection), Play::DegToRad(m_maxEmissionDirection)); // (i.e. theta in a right angled triangle)

		// Create cartesian coordinates (x,y) from polar coordinares (angle and direction)
		dir = { magnitude * cos(randAngle), magnitude * sin(randAngle) };
	}
	else
	{
		dir = m_particleLineVector + (m_initialPush * magnitude); // Push to the side a little too
		dir *= magnitude;
	}

	return Particle(m_shape, m_pos, dir, { 0.0f,0.0f }, randRot / 20.f, Play::cWhite);
}

void Emitter::CreateParticle()
{
	// Choose start location and initial Velocity
	Particle particle = ChooseParticleLocationAndInitialVelocity();

	// Nudge colour value by Colour start variation value
	// The same randVal could be used to make colour appear slightly brighter/darker
	int randRVal = Play::RandomRollRange(static_cast<int>(-m_startColourVariation), static_cast<int>(m_startColourVariation));
	int randGVal = Play::RandomRollRange(static_cast<int>(-m_startColourVariation), static_cast<int>(m_startColourVariation));
	int randBVal = Play::RandomRollRange(static_cast<int>(-m_startColourVariation), static_cast<int>(m_startColourVariation));

	Play::Colour nudgedStartColour // TODO: Possibly expensive std function?
	{
		std::clamp(static_cast<int>(m_startColour.red + randRVal), 0, 100),
		std::clamp(static_cast<int>(m_startColour.green + randGVal), 0, 100),
		std::clamp(static_cast<int>(m_startColour.blue + randBVal), 0, 100)
	};

	particle.colour = nudgedStartColour;
	
	if (m_velocityInOrientationDirection)
	{
		Play::Vector2f a = { 1.f, 0.f };
		Play::Vector2f b = particle.velocity;

		float rotation;

		rotation = AngleBetweenTwoVectors(a, b);

		particle.rotation = rotation;
	}

	particle.rotSpeed = RandomFloat(m_minRotSpeed, m_maxRotSpeed);

	// Scale and Lifetime
	particle.scale = (Play::RandomRollRange(static_cast<int>(m_minScale * 100), static_cast<int>(m_maxScale * 100))) / 100.f;
	particle.lifetime = RandomFloat(m_minParticleLifetime, m_maxParticleLifetime);

	// Push back to the Emitter's own vector
	v_Particles.push_back(particle);
}

void Emitter::SetScale(float min, float max, float incr, float wig)
{
	m_minScale = min;
	m_maxScale = max;
	m_increment = incr;
	m_scaleWiggle = wig;
}

void Emitter::SetVelocity(float minStartVel, float maxStartVel)
{
	m_minStartVelocity = minStartVel;
	m_maxStartVelocity = maxStartVel;
}

void Emitter::SetColour(Play::Colour col, float randStartVariation, float wiggle)
{
	m_startColour = col;
	m_startColourVariation = randStartVariation;
	m_colorWiggle = wiggle;
}

void Emitter::SetSpawnRate(float sr)
{
	m_spawnRate = sr;
}

void Emitter::SetDirection(float min, float max, float incr, float wiggle)
{
	m_minEmissionDirection = min;
	m_maxEmissionDirection = max;
	m_directionWiggle = wiggle;
}

void Emitter::SetGravity(bool state, GravityDirection dir = DOWN, float gravityMult = 1.f)
{
	m_useGravity = state;
	m_gravityDirection = dir;
	m_gravityMultiplier = gravityMult;
}

void Emitter::SetOpacity(float start, float end)
{
	m_startOpacity = start;
	m_endOpacity = end;
}

void Emitter::SetParticleLifetime(float min, float max)
{
	m_minParticleLifetime = min;
	m_maxParticleLifetime = max;
}

void Emitter::SetParticleLineProperties(bool state, Play::Vector2f lineVector, Play::Vector2f initPush)
{
	m_particleLine = state;
	m_particleLineVector = lineVector;
	m_initialPush = initPush;
}

void Emitter::SetRotation(float minSpeed, float maxSpeed, float increment, bool alignToVelocity)
{
	m_minRotSpeed = minSpeed;
	m_maxRotSpeed = maxSpeed;
	m_rotIncrement = increment;
	m_velocityInOrientationDirection = alignToVelocity;
}

void Emitter::Update(float elapsedTime)
{
	if (v_Particles.empty() && !m_emitContinuously)
	{
		return;
	}

	// Update Emitter lifetime timer
	if (!m_lifetime == 0.0f)
	{
		m_currentTime += elapsedTime;
		if (m_currentTime > m_lifetime)
		{
			Stop();
		}
	}

	// Update particle spawn timer
	m_spawnTimer += elapsedTime;

	// TODO: Could do with a settable delay, which could also be offset randomly
	if (m_emitContinuously)
	{
		float spawnDelay = 1.f / m_spawnRate;

		if (m_spawnTimer > spawnDelay)
		{
			int iterations = static_cast<int>(m_spawnTimer / spawnDelay);

			for (int i = 0; i < iterations; i++)
			{
				CreateParticle();
			}
			
			m_spawnTimer = 0.f;
		}
	}

	for (Particle& p : v_Particles)
	{
		// Update currentTime of all particles and delete if they are beyond their lifetime
		p.currentTime += elapsedTime;

		if (p.currentTime > p.lifetime)
		{
			p.isActive = false;
		}

		// For particleLine emitters, manage the velocity towards the start vector using the distance test (dot product)
		if (m_particleLine)
		{
			Play::Point2f startPoint{ m_pos };
			Play::Point2f endPoint{ m_pos + m_particleLineVector };

			Play::Vector2f lineVec = endPoint - startPoint;

			// Get a perpendicular vector to the start vector (velocity)
			Play::Vector2f unitPerpVec = { lineVec.y, -lineVec.x } ; // This is how you get a perpendicular vector apparently
			unitPerpVec.Normalize();

			// Remember this is a test VECTOR not a point, so has to be from the start point
			Play::Vector2f testVec = p.pos - startPoint;

			if (DEBUG)
			{
				Play::DrawLine(startPoint, endPoint, Play::cRed);
				Play::DrawLine(m_pos, m_pos + unitPerpVec * 100.f, Play::cGreen); // * 100.f for visualisation purposes
				Play::DrawLine(startPoint, p.pos, Play::cMagenta);
			}

			// Get dot product between the test point (particle) and the perpendicular unit vector
			float dp = dot(testVec, unitPerpVec); // p.pos is like the test vec in the example

			p.velocity -= (unitPerpVec * dp) * 0.01f;

			if (DEBUG)
			{
				Play::DrawDebugText(p.pos, std::to_string(dp).c_str(), Play::cWhite);
			}
		}

		// Update position - still need to do this if a particle line.
		
		p.rotation += p.rotSpeed;
		p.pos += p.velocity;
		p.rotSpeed += m_rotIncrement;

		if (m_velocityInOrientationDirection && !m_directionWiggle)
		{
			Play::Vector2f facingUnitVect = RotateVector({ 1.f, 0.f }, p.rotation);
			p.velocity = facingUnitVect * Magnitude(p.velocity);

			if (DEBUG)
			{
				Play::DrawLine(p.pos, p.pos + p.velocity * 10.f, Play::cGreen);
				Play::DrawLine(p.pos, p.pos + RotateVector({ 5.f, 0.f }, p.rotation) * 10.f, Play::cRed);
			}
		}

		if (m_directionWiggle != 0.0f)
		{
			float angle = RandomFloat(-m_directionWiggle, m_directionWiggle);
			p.velocity = RotateVector(p.velocity, angle);

			if (m_velocityInOrientationDirection)
			{
				p.rotation = AngleBetweenTwoVectors({ 1.f, 0.f }, p.velocity);
			}
		}

		p.velocity += p.acceleration;

		// Scale increment
		p.scale < 0.f ? p.scale = 0 : p.scale += m_increment / 10.f;

		// Scale wiggle
		p.scale += Play::RandomRollRange(static_cast<int>(-m_scaleWiggle * 100.f), static_cast<int>(m_scaleWiggle * 100.f)) / 1000.f;

		if (m_useGravity)
		{
			switch (m_gravityDirection)
			{
			case UP:
				p.velocity += GRAVITY * m_gravityMultiplier;
				break;

			case DOWN:
				p.velocity -= GRAVITY * m_gravityMultiplier;
				break;
			}
		}
		
	}

	// Check for particles whose lifetime has ended and remove them
	std::vector<Particle>::iterator it = v_Particles.begin();
	while (it != v_Particles.end())
	{
		if (!it->isActive) // Is an iterator is it a pointer precious????
			it = v_Particles.erase(it); // erase returns an iterator immediately after the one that was erased
		else
			++it;
	}
}

void Emitter::DrawParticles()
{
	if (v_Particles.empty() && !m_emitContinuously)
	{
		return;
	}

	// If emitter has a random particle scale (i.e. the scale isn't 1.f), use the more expensive Draw method
	if (m_minScale != 1.f || m_maxScale != 1.f)
	{
		for (Particle& p : v_Particles)
		{
			// Wiggle colour

			float opacity = std::lerp(m_startOpacity, m_endOpacity, p.currentTime / p.lifetime);

			Play::DrawSpriteRotated(p.spriteName, p.pos, 0, p.rotation, p.scale, opacity, p.colour);
		}

	}
	// Otherwise use the cheaper Draw method
	else
	{
		for (Particle& p : v_Particles)
		{
			Play::DrawSprite(p.spriteName, p.pos, 0);
		}
	}
}

void Emitter::Start()
{
	m_emitContinuously = true;
	m_spawnTimer = 0.f;
}

void Emitter::Stop()
{
	m_emitContinuously = false;
	m_spawnTimer = 0.f;
}

bool Emitter::LifetimeEnded()
{
	if (m_lifetime != 0.0f && m_currentTime > m_lifetime)
	{
		if (v_Particles.empty())
		{
			return true;
		}
		else return false;
	}
	else
	{
		return false;
	}
}

void Emitter::SetPosition(Play::Point2f pos)
{
	m_pos = pos;
}