#include "Game/MapGenStepWorm.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

MapGenStepWorm::MapGenStepWorm( const XMLElement& element ) :
														MapGenStep( element )
{
	m_lengthOfEachWorm = ParseXmlAttribute( element , "wormLength" , m_lengthOfEachWorm );
	m_numberOfWorms    = ParseXmlAttribute( element , "numWorms" , m_numberOfWorms );

	if ( m_lengthOfEachWorm.IsInRange( 0 ) )
	{
		ERROR_AND_DIE( "You forgot to specify the Length of each worm correctly in the XML" );
	}

	if ( m_numberOfWorms.IsInRange( 0 ) )
	{
		ERROR_AND_DIE( "You forgot to specify the Number of each worm correctly in the XML" );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapGenStepWorm::RunStepOnce( Map& map )
{
	if ( m_setTile == nullptr )
	{
		return;
	}

	SpawnWorm( map );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapGenStepWorm::SpawnWorm( Map& map )
{
	RandomNumberGenerator rng;
	int numberOfWorms = m_numberOfWorms.GetRandomInRange( rng );

	for ( int currentWormNum = 0; currentWormNum < numberOfWorms; currentWormNum++ )
	{
		IntVec2 tileCoords;
		tileCoords.x   = rng.RollRandomIntInRange( 1 , map.m_dimensions.x - 2 );
		tileCoords.y   = rng.RollRandomIntInRange( 1 , map.m_dimensions.y - 2 );
		int wormLength = m_lengthOfEachWorm.GetRandomInRange( rng );
		//int wormCurrentLength = 0;
		int tileIndex = ( tileCoords.y * map.m_dimensions.x ) + tileCoords.x;
		for ( int wormCurrentLength = 0; wormCurrentLength < wormLength; wormCurrentLength++ )
		{

			IntVec2 nextTile = tileCoords;
			int nextTileMovement  = rng.RollRandomIntInRange( 0 , 1 );
			int nextTileVariation = rng.RollRandomIntInRange( -1 , 1 );

			if ( nextTileMovement == 0 )
			{
				tileCoords.x += nextTileVariation;
			}
			else
			{
				tileCoords.y += nextTileVariation;
			}

			tileIndex = ( tileCoords.y * map.m_dimensions.x ) + tileCoords.x;

			int currentTileIndex = map.GetTileIndexforTileCoords( tileCoords );
			map.m_tiles[ currentTileIndex ].SetTileType( m_setTile );
		}
	}
}
