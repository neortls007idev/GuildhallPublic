#include "Game/GameParticleEmitterSpawnJob.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Engine/ParticleSystem/ParticleEmitter3D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
extern		JobSystem* g_theJobSystem;

//--------------------------------------------------------------------------------------------------------------------------------------------

GameParticleEmitterSpawnJob::GameParticleEmitterSpawnJob( int jobID , ParticleEmitter3D* emitter , size_t numNewParticles , float particleVelocity ,
									int particleMinLifeTime , int particleMaxLifeTime , Vec2 particleSize , Rgba8 startColor , Rgba8 endColor ) :
																																	Job() ,
																																	m_numNewParticles( numNewParticles ) ,
																																	m_emitter( emitter ) ,
																																	m_particleVelocity( particleVelocity ) ,
																																	m_particleMinLifeTime( particleMinLifeTime ) ,
																																	m_particleMaxLifeTime( particleMaxLifeTime ) ,
																																	m_particleSize( particleSize ) ,
																																	m_startColor( startColor ) ,
																																	m_endColor( endColor )
{
	UNUSED( jobID );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameParticleEmitterSpawnJob::~GameParticleEmitterSpawnJob()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameParticleEmitterSpawnJob::Execute()
{
 	size_t numWorkerThreads			= std::thread::hardware_concurrency() - 1;				// -1 for the main thread;
 	size_t particlesPerJob			= m_numNewParticles / numWorkerThreads;
 	size_t leftoverParticles		= m_numNewParticles % numWorkerThreads;
 	size_t numParticleSpawnSubJobs  = 0;
 	
 	if( particlesPerJob > 0 )
 	{
 		numParticleSpawnSubJobs = m_numNewParticles / particlesPerJob;
 	}
 	
 	for ( size_t spawnSubJobCount = 0; spawnSubJobCount < numParticleSpawnSubJobs; spawnSubJobCount++ )
 	{
 		GameParticleEmitterSpawnSubJob* spawnParticleSubJob = new GameParticleEmitterSpawnSubJob( 0 , m_emitter , particlesPerJob , m_particleVelocity ,
 																	m_particleMinLifeTime , m_particleMaxLifeTime , m_particleSize , m_startColor , m_endColor );
 		g_theJobSystem->PostJob( *spawnParticleSubJob );
 	}
 	if ( leftoverParticles > 0 )
 	{
 		GameParticleEmitterSpawnSubJob* spawnParticleSubJob = new GameParticleEmitterSpawnSubJob( 0 , m_emitter , particlesPerJob , m_particleVelocity ,
 																	m_particleMinLifeTime , m_particleMaxLifeTime , m_particleSize , m_startColor , m_endColor );
 		g_theJobSystem->PostJob( *spawnParticleSubJob );
 	}

//	for ( uint particleSpawned = 0; particleSpawned < m_numNewParticles; particleSpawned++ )
//	{
//		Vec3 position = g_RNG->RollRandomInUnitSphere();
//		Vec3 deviation = g_RNG->RollRandomUnitVec3() * m_particleVelocity;
//		int maxAge = g_RNG->RollRandomIntInRange( m_particleMinLifeTime , m_particleMaxLifeTime );
//		m_emitter->SpawnNewRandomParticleFromSpriteSheet( AABB2( -m_particleSize , m_particleSize ) , position , 1.f , deviation ,
//			static_cast< uint16_t >( maxAge ) , m_startColor , m_endColor );
//	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameParticleEmitterSpawnSubJob::GameParticleEmitterSpawnSubJob( int jobID , ParticleEmitter3D* emitter , size_t numNewParticles , float particleVelocity ,
	int particleMinLifeTime , int particleMaxLifeTime , Vec2 particleSize , Rgba8 startColor , Rgba8 endColor ) :
																													Job() ,
																													m_newSpawnCount( numNewParticles ) ,
																													m_emitter( emitter ) ,
																													m_particleVelocity( particleVelocity ) ,
																													m_particleMinLifeTime( particleMinLifeTime ) ,
																													m_particleMaxLifeTime( particleMaxLifeTime ) ,
																													m_particleSize( particleSize ) ,
																													m_startColor( startColor ) ,
																													m_endColor( endColor )
{
	UNUSED( jobID );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameParticleEmitterSpawnSubJob::~GameParticleEmitterSpawnSubJob()
{
	m_emitter = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameParticleEmitterSpawnSubJob::Execute()
{
	for ( uint particleSpawned = 0; particleSpawned < m_newSpawnCount; particleSpawned++ )
	{
		Vec3 position = g_RNG->RollRandomInUnitSphere();
		Vec3 deviation = g_RNG->RollRandomUnitVec3() * m_particleVelocity;
		int maxAge = g_RNG->RollRandomIntInRange( m_particleMinLifeTime , m_particleMaxLifeTime );
		m_emitter->SpawnNewRandomParticleFromSpriteSheet( AABB2( -m_particleSize , m_particleSize ) , position , 1.f , deviation ,
															static_cast< uint16_t >( maxAge ) , m_startColor , m_endColor );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------