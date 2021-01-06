#pragma once
#include "Engine/Memory/Job.hpp"
#include "Engine/Primitives/Frustum.hpp"
#include "Engine/Math/Transform.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
typedef std::vector< Transform* > OBJInstances;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class GameFrustumCullingJob : public Job
{
public:
	GameFrustumCullingJob( int jobID , Frustum cameraFustum , OBJInstances& modelCulledInstances ,
						  OBJInstances& modelAllDrawableInstances , float currentModelRadius , size_t numInstanceCount );
	~GameFrustumCullingJob();
	
	void Execute() override;

 private:
	 Frustum		m_cameraFustum;
	 OBJInstances	m_modelCulledInstances;
	 OBJInstances	m_modelAllDrawableInstances;
	 float			m_modelRadius				= 0.f;
	 size_t			m_numInstanceCount			= 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameFrustumCullingSubJob : public Job
{
public:
	GameFrustumCullingSubJob( int jobID , Frustum cameraFustum , OBJInstances& modelCulledInstances , OBJInstances& modelAllDrawableInstances ,
							  float currentModelRadius , size_t numInstanceCount , size_t startIndex , size_t endIndex );
	~GameFrustumCullingSubJob();

	void Execute() override;

private:
	Frustum			m_viewFrustum;
	OBJInstances	m_modelCulledInstances;
	OBJInstances	m_modelAllDrawableInstances;
	float			m_modelRadius				= 0.f;
	size_t			m_numInstanceCount			= 0;
	size_t			m_startIndex				= 0;
	size_t			m_endIndex					= 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
	