#include "Play.h"
#include "MainGame.h"
#include "Common.h"
#include "Particle.h"
#include "Emitter.h"
#include "PolygonEmitter.h"

PolygonEmitter::PolygonEmitter(ParticleShape shape, std::vector<Play::Point2f> corners, bool deleteParticles)
{
	m_shape = shape;
	m_v_corners = corners;
	m_deleteOutOfBoundsParticles = deleteParticles;
}

PolygonEmitter::PolygonEmitter()
{
	// TODO: Is it OK that this is empty?
}

PolygonEmitter::~PolygonEmitter()
{
	v_Particles.clear();
}

Particle PolygonEmitter::ChooseParticleLocationAndInitialVelocity()
{
	// Choose a corner to create a vector from
	int randIndex = Play::RandomRoll(m_v_corners.size());
	randIndex--; // Because RandomRoll doesn't return 0

	int originIndex;

	Play::Vector2f sideVector;

	// Choose a side
	int size = m_v_corners_world.size();
	sideVector = { m_v_corners_world[randIndex == m_v_corners_world.size() - 1 ? 0 : randIndex + 1] - m_v_corners_world[randIndex] };
	originIndex = randIndex;
	if (DEBUG) Play::DrawLine(m_v_corners_world[randIndex == m_v_corners_world.size() - 1 ? 0 : randIndex + 1], m_v_corners_world[randIndex], Play::cMagenta);

	// Get a point along the chosen side vector
	float randMultValue = Play::RandomRoll(100) / 100.f;
	Play::Point2f spawnPoint = sideVector * randMultValue + m_v_corners_world[originIndex];
	if (DEBUG) Play::DrawDebugText(spawnPoint, "@");

	Play::Vector2f velocity{ sideVector.y, -sideVector.x }; // velocity perpendicular to chosen side (y, -x)
	Play::Vector2f normVel = NormaliseVector(velocity); // to normalise a vector, divide it by its length

	float angle = RandomFloat(-Play::PLAY_PI / 2.f, Play::PLAY_PI / 2.f); // Random angle in radians

	normVel = { normVel.x * cos(angle) - normVel.y * sin(angle), normVel.x * sin(angle) + normVel.y * cos(angle) };	// Rotate velocity by the angle

	float magnitude = RandomFloat(m_minStartVelocity, m_maxStartVelocity); // Random magnitude within limits

	return Particle { m_shape, spawnPoint, normVel * magnitude, {0.f, 0.f}, 0.f, m_startColour };
}

void PolygonEmitter::Update(float elapsedTime)
{
	// Update Emitter location
	for (Play::Point2f& c : m_v_corners)
	{
		m_v_corners_world.push_back(c + m_pos);
	}

	if (DEBUG)
	{
		for (int i = 0; i < m_v_corners_world.size(); i++)
		{
			int mod = (i + 1) % m_v_corners_world.size(); // This is very cool and useful - using modulo to loop round array
			Play::DrawLine(m_v_corners_world[i], m_v_corners_world[mod], Play::cGreen);
		}
	}

	Emitter::Update(elapsedTime);

	// Check for out of bounds particles
	if (v_Particles.empty())
		return;

	if (m_deleteOutOfBoundsParticles)
	{
		std::vector<Particle>::iterator it = v_Particles.begin();
		while (it != v_Particles.end())
		{
			for (int i = 0; i < m_v_corners_world.size(); i++)
			{
				int mod = (i + 1) % m_v_corners_world.size();
				Play::Vector2f sideVector = m_v_corners_world[mod] - m_v_corners_world[i]; // this is correct
				Play::Vector2f perpVec = { sideVector.y, -sideVector.x };

				Play::Point2f halfwayPoint = { (m_v_corners_world[mod].x + m_v_corners_world[i].x) / 2, (m_v_corners_world[mod].y + m_v_corners_world[i].y) / 2 }; // Start Point - halfway between corners
				Play::Vector2f normPerpVec = perpVec / sqrtf(powf(perpVec.x, 2) + powf(perpVec.y, 2)); // This scary bit is normalizing the perp Vector by dividing by its length then multiplying for visual purposes

				if (DEBUG)
				{
					// Draw Perp Vec
					Play::DrawLine
					(
						halfwayPoint, // Start Point - halfway between corners
						normPerpVec * 50.f + halfwayPoint,
						Play::cRed
					);

					// Draw point to particle vec
					Play::DrawLine(halfwayPoint, it->pos, Play::cMagenta);
				}

				float dp = dot(perpVec, it->pos - halfwayPoint);

				if (dp < 0)
				{
					// Commented out to make it look cooler for now
					// it = v_Particles.erase(it);
					break;
				}
			}

			if (it != v_Particles.end())
			{
				++it;
			}
		}
	}

	m_v_corners_world.clear();
}