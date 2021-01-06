#include "Game/Map.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/player.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Map::Map( Game* theGame , IntVec2 size ) : m_theGame( theGame ) , m_Size( size )
{
	for ( int verticalIndex = 0; verticalIndex < m_Size.y; verticalIndex ++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < m_Size.x; horizontalIndex ++ )
		{
			m_tiles.push_back( Tile( IntVec2( horizontalIndex , verticalIndex ) , TILE_TYPE_GRASS ) );
		}
	}
		
	RandomizeObstacleTiles();
	SafeZones();
	OuterBoundaryWalls();
	SafeZonesBoundary();
	
	
	Player* m_Player = new Player( m_theGame , Vec2( 1.5f , 1.5f ) , Vec2( 0.f , 0.f ) , 45.f );
	m_entities.push_back( m_Player );
	Player* tank= new Player( m_theGame , Vec2( 7.0f , 3.0f ) , Vec2( 0.f , 0.f ) , 45.f );
	m_entities.push_back( tank );
	
}

void Map::Update( float deltaSeconds )
{
	m_entities[ 0 ]->Update( deltaSeconds );
	//m_entities[ 1 ]->Update( deltaSeconds );
	PushMobileDiscOutOfFixedDisc( m_entities[ 0 ]->m_position , m_entities[ 0 ]->m_physicsRadius , m_entities[ 1 ]->m_position , m_entities[ 1 ]->m_physicsRadius );
}

void Map::Render() const
{


	for ( int index = 0; index < m_tiles.size(); index++ )
	{

		m_tiles[ index ].Render();
	}
	m_entities[ 0 ]->Render();
	m_entities[ 1 ]->Render();

}

int Map::GetTileIndexforTileCoords( const IntVec2& tileCoords ) const
{
	return tileCoords.x + ( m_Size.x * tileCoords.y );
}

IntVec2 Map::GetTileCoordsforTileIndex( int tileIndex ) const
{
	int tileY = tileIndex / m_Size.x;
	int tileX = tileIndex - ( tileY * m_Size.x );
	return( IntVec2( tileX , tileY ) );
}

void Map::OuterBoundaryWalls()
{
	for ( int horizontalIndex = 0; horizontalIndex < m_Size.x; horizontalIndex++ )
	{
		m_tiles[ horizontalIndex ].SetTileType( TILE_TYPE_STONE );
	}
	for ( int verticalIndex = 1; verticalIndex < m_Size.y; verticalIndex++ )
	{
		int index =  verticalIndex * m_Size.x ;
		int index2 = index - 1;
		m_tiles[ index2 ].SetTileType( TILE_TYPE_STONE );
		m_tiles[ index ].SetTileType( TILE_TYPE_STONE );
	}
	
	for ( int horizontalIndex = 0; horizontalIndex < m_Size.x; horizontalIndex++ )
	{
		int index = ((m_Size.y-1)*m_Size.x) + horizontalIndex;
		m_tiles[ index ].SetTileType( TILE_TYPE_STONE );
	}
}

void Map::InitializePlayerTile()
{
	//m_tiles.push_back( Tile( 1 , 1 ) , TILE_TYPE_GRASS );
}

void Map::RandomizeObstacleTiles()
{
	
	int percentageOfObstaclesInMap = (int)(( m_Size.x * m_Size.y ) * PERCENTAGE_OF_OBSTACLES_IN_MAP);
	for ( int index = 0; index < percentageOfObstaclesInMap; index ++)
	{
		int randomTileIndexX = g_RNG->RollRandomIntInRange( 1 , (int) m_Size.x-2 );
		int randomTileIndexY = g_RNG->RollRandomIntInRange( 1 , (int) m_Size.y-2 );
		int tileIndex = ( randomTileIndexY * m_Size.x ) + randomTileIndexX;
		m_tiles[ tileIndex ].SetTileType( TILE_TYPE_STONE );
	}
}

void Map::SafeZones()
{
	//-----------------------------------------------------------------------------------------------------------------
	// SPAWN SAFEZONE
	//-----------------------------------------------------------------------------------------------------------------
	
	for (int verticalIndex = 1; verticalIndex < 6; verticalIndex++)
	{
		for ( int horizontalIndex = 1; horizontalIndex < 6; horizontalIndex++ )
		{
			int index = ( verticalIndex * m_Size.x ) + horizontalIndex;
			m_tiles[ index ].SetTileType( TILE_TYPE_GRASS );
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	// FINISH SAFEZONE
	//-----------------------------------------------------------------------------------------------------------------

	for ( int verticalIndex = m_Size.y - 1; verticalIndex > m_Size.y - 7; verticalIndex-- )
	{
		for ( int horizontalIndex = m_Size.x - 1; horizontalIndex > m_Size.x - 7; horizontalIndex-- )
		{
			int index = ( verticalIndex * m_Size.x ) + horizontalIndex;
			m_tiles[ index ].SetTileType( TILE_TYPE_GRASS );
		}
	}
}

void Map::SafeZonesBoundary()
{
	//-----------------------------------------------------------------------------------------------------------------
	// SPAWN BOUNDARY
	//-----------------------------------------------------------------------------------------------------------------
	int index = ( int ) ( ( 2 * WORLD_SIZE_X ) + 4 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( 3 * WORLD_SIZE_X ) + 4 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( 4 * WORLD_SIZE_X ) + 2 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( 4 * WORLD_SIZE_X ) + 3 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( 4 * WORLD_SIZE_X ) + 4 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	//-----------------------------------------------------------------------------------------------------------------
	// FINISH LINE BOUNDARY
	//-----------------------------------------------------------------------------------------------------------------

	index = ( int ) ( ( ( WORLD_SIZE_Y - 3 ) * WORLD_SIZE_X ) + WORLD_SIZE_X - 5 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( WORLD_SIZE_Y - 4 ) * WORLD_SIZE_X ) + WORLD_SIZE_X - 5 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( WORLD_SIZE_Y - 5 ) * WORLD_SIZE_X ) + WORLD_SIZE_X - 5 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( WORLD_SIZE_Y - 6 ) * WORLD_SIZE_X ) + WORLD_SIZE_X - 5 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( WORLD_SIZE_Y - 6 ) * WORLD_SIZE_X ) + WORLD_SIZE_X - 4 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( WORLD_SIZE_Y - 6 ) * WORLD_SIZE_X ) + WORLD_SIZE_X - 3 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

}
