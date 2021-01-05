#include "Game/MapGenStepMutate.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------


MapGenStepMutate::MapGenStepMutate( const XMLElement& element ) : MapGenStep(element)
{
	m_chanceToMutate = ParseXmlAttribute( element , "ChanceToMutate" , m_chanceToMutate );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapGenStepMutate::RunStepOnce( Map& map )
{
	if ( m_setTile == nullptr )
	{
		return;
	}

	RandomNumberGenerator rng;
	
	float	chancePerTile = m_chancePerTile.GetRandomInRange( rng );
	IntVec2 tileCoords;

	for ( tileCoords.y = 0; tileCoords.y < map.m_dimensions.y; tileCoords.y++ )
	{
		for (  tileCoords.x = 0; tileCoords.x < map.m_dimensions.x; tileCoords.x++ )
		{	
			Tile& tile = *map.GetTileAtTileCoords( tileCoords );
			if ( tile.GetTileType() == m_ifTile )
			{
				if ( rng.RollPercentChance( chancePerTile ) )
				{
					tile.SetTileType( m_setTile );
				}
			}
		}
	}

// 	for ( int tileIndex = 0; tileIndex < map.m_tiles.size(); tileIndex++ )
// 	{
// 	}
}
