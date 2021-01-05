#pragma once
#include "Game/MapGenStep.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapGenStepWorm : public MapGenStep
{
public:
	MapGenStepWorm( const XMLElement& element );
	~MapGenStepWorm() {};

	virtual void RunStepOnce( Map& map ) override;

	void SpawnWorm( Map& map );

private:
	IntRange m_numberOfWorms    = IntRange( 0 );
	IntRange m_lengthOfEachWorm = IntRange( 0 );

};

//--------------------------------------------------------------------------------------------------------------------------------------------