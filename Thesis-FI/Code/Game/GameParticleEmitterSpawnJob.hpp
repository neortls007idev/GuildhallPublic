#pragma once
#include "Engine/Memory/Job.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class ParticleEmitter3D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameParticleEmitterSpawnJob : public Job
{
public:
	GameParticleEmitterSpawnJob( int jobID , ParticleEmitter3D* emitter , size_t numNewParticles ,
					  float particleVelocity , int particleMinLifeTime, int particleMaxLifeTime ,
					  Vec2 particleSize , Rgba8 startColor, Rgba8 endColor );
	
	~GameParticleEmitterSpawnJob();
	void Execute() override;

private:
	size_t				m_numNewParticles			= 0;
	ParticleEmitter3D*	m_emitter					= nullptr;
	float				m_particleVelocity			= 0.f;
	int					m_particleMinLifeTime		= 0;
	int					m_particleMaxLifeTime		= 0;
	Vec2				m_particleSize				= Vec2::ZERO;
	Rgba8				m_startColor;
	Rgba8				m_endColor;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameParticleEmitterSpawnSubJob : public Job
{
public:
	GameParticleEmitterSpawnSubJob( int jobID , ParticleEmitter3D* emitter , size_t numNewParticles ,
						 float particleVelocity , int particleMinLifeTime , int particleMaxLifeTime ,
						 Vec2 particleSize , Rgba8 startColor , Rgba8 endColor );
	
	~GameParticleEmitterSpawnSubJob();

	void Execute() override;

private:
	size_t				m_newSpawnCount			= 0;
	ParticleEmitter3D*	m_emitter				= nullptr;
	float				m_particleVelocity		= 0.f;
	int					m_particleMinLifeTime	= 0;
	int					m_particleMaxLifeTime	= 0;
	Vec2				m_particleSize			= Vec2::ZERO;
	Rgba8				m_startColor;
	Rgba8				m_endColor;
};

//--------------------------------------------------------------------------------------------------------------------------------------------