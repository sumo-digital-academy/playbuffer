#pragma once

enum class EmitterType
{
	SMOKE = 0,
	PORTAL
};

class Emitter;

class ParticleManager
{
public:
	void UpdateEmitters(float elapsedTime);
	int CreateEmitter(EmitterType emitter, Play::Point2f pos, float lifetime, float scale = 1.f);
	void DrawParticles();
	void DestroyEmitter(std::vector<Emitter*>::iterator& it);

	Emitter* GetEmitter(int id);

	// Singleton
	static ParticleManager& Instance(int maxEmitters); // For initialisation
	static ParticleManager& Instance(); // General access
	static void Destroy();

private:
	// Singleton
	ParticleManager();  // Private constructor and destructor
	~ParticleManager();
	static ParticleManager* s_pInstance; // Static pointer to the instance of the Particle Manager

	ParticleManager& operator=(const ParticleManager&) = delete; // No assignment allowed
	ParticleManager(const ParticleManager&) = delete; // No copy constructor allowed;

	std::vector<Emitter*> m_v_pEmitters{};
	int m_maxEmitters{ 100 };
};