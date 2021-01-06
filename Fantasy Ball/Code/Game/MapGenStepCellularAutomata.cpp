#include "Game/MapGenStepCellularAutomata.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

MapGenStepCellularAutomata::MapGenStepCellularAutomata( const XMLElement& element ) : 
																					  MapGenStep( element )
{
	m_numOfNeighbors   = ParseXmlAttribute( element , "numberOfNeighbors" , m_numOfNeighbors );
	m_neighborTileType = ParseXmlAttribute( element , "ifNeighborTileType" , m_neighborTileType );

	if ( !m_neighborTileType )
	{
		ERROR_AND_DIE( "You forgot to specify the neighbor tile type in the XML" );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapGenStepCellularAutomata::RunStepOnce( Map& map )
{
	if ( m_setTile == nullptr )
	{
		return;
	}
	RandomNumberGenerator rng;
	
	int numberOfneighborsToCheckFor = m_numOfNeighbors.GetRandomInRange( rng );
	float	chancePerTile = m_chancePerTile.GetRandomInRange( rng );
	IntVec2 tileCoords;

	std::vector<bool> hasTileChanged;
	hasTileChanged.resize( map.m_tiles.size() );

	for ( tileCoords.y = 1; tileCoords.y < map.m_dimensions.y-1; tileCoords.y++ )
	{
		for ( tileCoords.x = 1; tileCoords.x < map.m_dimensions.x-1; tileCoords.x++ )
		{
			Tile& tile = *map.GetTileAtTileCoords( tileCoords );
			int numMatchingNeighborTiles = NumberOfMatchingNeighbor( map , &tile );
			int tileIndex = map.GetTileIndexforTileCoords( tileCoords );
			if ( numMatchingNeighborTiles >= numberOfneighborsToCheckFor )
			{
				if ( rng.RollPercentChance( chancePerTile ) )
				{
					hasTileChanged[ tileIndex ] = true;
				}
			}
		}
	}

	for ( int index = 0; index < hasTileChanged.size(); index++ )
	{
		if ( hasTileChanged[index] )
		{
			//map.m_tiles[ index ].SetTileType( m_setTile );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int MapGenStepCellularAutomata::NumberOfMatchingNeighbor( Map& map , Tile* currentTile )
{
	int numMatchingNeighbors = 0;
	IntVec2 currentTileCoords = currentTile->GetTileIndex();
	Tile tempTile = *map.GetNorthTileForTileCoords( currentTileCoords );
	if (tempTile.GetTileType() == m_neighborTileType )
	{
		numMatchingNeighbors++;
	}

	tempTile = *map.GetSouthTileForTileCoords( currentTileCoords );
	if ( tempTile.GetTileType() == m_neighborTileType )
	{
		numMatchingNeighbors++;
	}

	tempTile = *map.GetEastTileForTileCoords( currentTileCoords );
	if ( tempTile.GetTileType() == m_neighborTileType )
	{
		numMatchingNeighbors++;
	}

	tempTile = *map.GetWestTileForTileCoords( currentTileCoords );
	if ( tempTile.GetTileType() == m_neighborTileType )
	{
		numMatchingNeighbors++;
	}

	tempTile = *map.GetNorthEastTileForTileCoords( currentTileCoords );
	if ( tempTile.GetTileType() == m_neighborTileType )
	{
		numMatchingNeighbors++;
	}

	tempTile = *map.GetNorthWestTileForTileCoords( currentTileCoords );
	if ( tempTile.GetTileType() == m_neighborTileType )
	{
		numMatchingNeighbors++;
	}

	tempTile = *map.GetSouthEastTileForTileCoords( currentTileCoords );
	if ( tempTile.GetTileType() == m_neighborTileType )
	{
		numMatchingNeighbors++;
	}

	tempTile = *map.GetSouthWestTileForTileCoords( currentTileCoords );
	if ( tempTile.GetTileType() == m_neighborTileType )
	{
		numMatchingNeighbors++;
	}

	return  numMatchingNeighbors;
}

