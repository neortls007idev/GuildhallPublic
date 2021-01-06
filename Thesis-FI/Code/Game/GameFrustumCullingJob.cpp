#include "Game/GameFrustumCullingJob.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Memory/JobSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern		JobSystem* g_theJobSystem;

//--------------------------------------------------------------------------------------------------------------------------------------------

GameFrustumCullingJob::GameFrustumCullingJob( int jobID , Frustum cameraFustum , OBJInstances& modelCulledInstances ,
											  OBJInstances& modelAllDrawableInstances , float currentModelRadius , size_t numIntanceCount ) : 
																													Job() ,
																													m_cameraFustum( cameraFustum ) ,
																													m_modelCulledInstances( modelCulledInstances ) , 
																													m_modelAllDrawableInstances( modelAllDrawableInstances ) ,
																													m_modelRadius( currentModelRadius ) ,
																													m_numInstanceCount( numIntanceCount )
{
	UNUSED( jobID );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameFrustumCullingJob::~GameFrustumCullingJob()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameFrustumCullingJob::Execute()
{
	size_t numWorkerThreads = std::thread::hardware_concurrency() - 1;				// -1 for the main thread;
	size_t instancesPerJob = static_cast< size_t >( m_numInstanceCount / numWorkerThreads );
	size_t leftoverinstances = static_cast< size_t >( m_numInstanceCount % numWorkerThreads );
	//size_t numInstancesSpawnSubJobs = m_numInstanceCount / instancesPerJob;

	size_t instancesEndIndex = instancesPerJob - 1;

	for ( size_t IntanceStartIndex = 0; instancesEndIndex < ( m_numInstanceCount - leftoverinstances ); )
	{
		GameFrustumCullingSubJob* ViewFrustumCullingJob = new GameFrustumCullingSubJob( 0 , m_cameraFustum , m_modelCulledInstances , m_modelAllDrawableInstances ,
																						m_modelRadius , m_numInstanceCount , IntanceStartIndex , instancesEndIndex );
		g_theJobSystem->PostJob( *ViewFrustumCullingJob );
		IntanceStartIndex = instancesEndIndex + 1;
		instancesEndIndex += instancesPerJob;
	}
	if ( leftoverinstances > 0 )
	{
		instancesEndIndex -= instancesPerJob;
		GameFrustumCullingSubJob* ViewFrustumCullingJob = new GameFrustumCullingSubJob( 0 , m_cameraFustum , m_modelCulledInstances , m_modelAllDrawableInstances ,
																						m_modelRadius , m_numInstanceCount , instancesEndIndex + 1 , 
																						instancesEndIndex + leftoverinstances );
		g_theJobSystem->PostJob( *ViewFrustumCullingJob );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameFrustumCullingSubJob::GameFrustumCullingSubJob( int jobID , Frustum cameraFustum , OBJInstances& modelCulledInstances , OBJInstances& modelAllDrawableInstances ,
													float currentModelRadius , size_t numInstanceCount , size_t startIndex , size_t endIndex ) :
	Job() ,
	m_viewFrustum( cameraFustum ) ,
	m_modelCulledInstances( modelCulledInstances ) ,
	m_modelAllDrawableInstances( modelAllDrawableInstances ) ,
	m_modelRadius( currentModelRadius ) ,
	m_numInstanceCount( numInstanceCount ) , 
	m_startIndex( startIndex ),
	m_endIndex( endIndex )
{
	UNUSED( jobID );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameFrustumCullingSubJob::~GameFrustumCullingSubJob()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameFrustumCullingSubJob::Execute()
{
	m_modelCulledInstances.clear();
	
	for ( size_t instanceIndex = m_startIndex; instanceIndex < m_modelAllDrawableInstances.size(); instanceIndex++ )
	{
		if ( nullptr == m_modelAllDrawableInstances[ instanceIndex ] )
		{
			continue;
		}

		Vec3& pos = m_modelAllDrawableInstances[ instanceIndex ]->m_position;
	
		if ( m_viewFrustum.IsSphereInsideFrustum( pos , m_modelRadius ) )
		{
			m_modelCulledInstances.emplace_back( m_modelAllDrawableInstances[ instanceIndex ] );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	