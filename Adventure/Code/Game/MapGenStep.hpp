#pragma once
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/Map.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapGenStep
{
public:
	MapGenStep( const XMLElement& generatorXMLElement );
	virtual ~MapGenStep() {};
	void RunStep( Map& map );
	virtual void RunStepOnce( Map& map ) = 0;

public:
	static MapGenStep* CreateMapGenStep( const XMLElement& generatorXMLElement );

public:
	std::string				 m_name;
	FloatRange				 m_chanceToRun			 = FloatRange( 1.f );
	FloatRange				 m_chancePerTile		 = FloatRange( 1.f );
	IntRange				 m_numIterations		 = IntRange( 1 );
	int						 m_currentGenerationStep = 0;
	TileDefinition*			 m_ifTile				 = nullptr;
	TileDefinition*			 m_setTile				 = nullptr;
};
//--------------------------------------------------------------------------------------------------------------------------------------------