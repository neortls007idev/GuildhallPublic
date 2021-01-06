#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Game/MapGenStep.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Tile;

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapGenStepCellularAutomata : public MapGenStep
{
public:
	MapGenStepCellularAutomata( const XMLElement& element );
	~MapGenStepCellularAutomata() {};

	virtual void RunStepOnce( Map& map ) override;
	int NumberOfMatchingNeighbor( Map& map , Tile* currentTile );
private:
	IntRange	    m_numOfNeighbors = IntRange( 1 , 8 );
	TileDefinition* m_neighborTileType = nullptr;
};
//--------------------------------------------------------------------------------------------------------------------------------------------