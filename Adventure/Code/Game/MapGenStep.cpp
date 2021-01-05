#include "Game/MapGenStep.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Game/MapGenStepMutate.hpp"
#include "Game/MapGenStepWorm.hpp"
#include "Game/MapGenStepCellularAutomata.hpp"
#include "Game/MapGenStepFromImage.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

MapGenStep::MapGenStep( const XMLElement& generatorXMLElement )
{
	m_name = generatorXMLElement.Name();
	m_ifTile		= ParseXmlAttribute( generatorXMLElement , "ifTile" , m_ifTile );
	m_setTile		= ParseXmlAttribute( generatorXMLElement , "setTile" , m_ifTile );
	m_chanceToRun   = ParseXmlAttribute( generatorXMLElement , "chanceToRun" , m_chanceToRun );
	m_chancePerTile = ParseXmlAttribute( generatorXMLElement , "chancePerTile" , m_chancePerTile );
	m_numIterations = ParseXmlAttribute( generatorXMLElement , "iterations" , m_numIterations );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapGenStep::RunStep( Map& map )
{
	RandomNumberGenerator rng = RandomNumberGenerator();
	if ( !m_chanceToRun.GetRandomInRange( rng ) )
	{
		return;
	}

	int numIterations = m_numIterations.GetRandomInRange( rng );
	for ( int iterationCount = 0; iterationCount < numIterations; iterationCount++ )
	{
		RunStepOnce( map );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

// void MapGenStep::RunStepOnce( Map& map )
// {
// 	
// }

//--------------------------------------------------------------------------------------------------------------------------------------------

MapGenStep* MapGenStep::CreateMapGenStep( const XMLElement& generatorXMLElement )
{
	std::string generatorName = generatorXMLElement.Name();
	if ( generatorName.compare( "Mutate" )			 == 0 )			 return new MapGenStepMutate( generatorXMLElement );
	if ( generatorName.compare( "Worm" )			 == 0 )			 return new MapGenStepWorm( generatorXMLElement );
	if ( generatorName.compare( "FromImage" )		 == 0 )			 return new MapGenStepFromImage( generatorXMLElement );
	if ( generatorName.compare( "CellularAutomata" ) == 0 )			 return new MapGenStepCellularAutomata( generatorXMLElement );
// 	if( generatorName == "PerlinNoise")			 return new
// 	if( generatorName == "RoomsAndPaths")		 return new

	ERROR_AND_DIE( Stringf( "XML ERROR: Invalid Map Generation type\"%s\"!" , generatorName.c_str() ) );
}
