#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Game/MapGenStep.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapGenStepMutate : public MapGenStep
{
public:
	MapGenStepMutate( const XMLElement& element );
	~MapGenStepMutate() {};

	virtual void RunStepOnce( Map& map ) override;

private:
	FloatRange				 m_chanceToMutate = FloatRange( 1.f );
};
//--------------------------------------------------------------------------------------------------------------------------------------------