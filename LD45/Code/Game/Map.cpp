#include "Game/Map.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/player.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Map::Map( Game* theGame , IntVec2 size ) : m_game( theGame ) , m_Size( size )
{
	for (int verticalIndex = 0; verticalIndex < m_Size.y; verticalIndex+=40)
	{
		for ( int horizontalIndex = 0; horizontalIndex < m_Size.x; horizontalIndex+=40)
		{
			m_tiles.push_back(Tile(IntVec2(horizontalIndex,verticalIndex),TILE_TYPE_GRASS));
		}
	}
	//Player* m_Player = new Player();
	//m_entities.push_back( new Player());
	m_age = 2.f;
}

void Map::Update( float deltaSeconds )
{
	m_age -= deltaSeconds;
	if ( m_age <= 0 )
	{
		m_age = 2.f;
	}
}

void Map::Render() const
{
		for ( int index = 0; index < m_tiles.size(); index++ )
		{
			m_tiles[ index ].Render();
		}

		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->BindTexture( m_game->m_Waldo );
		
		static int waldoTile;
		static RandomNumberGenerator RNG;

		if ( m_age >= 1.5f )
		{
			waldoTile = RNG.RollRandomIntInRange( 0 , ( int ) m_tiles.size() - 1 );
			RNG.manuallyIncrementPosition();
		}
		m_tiles[ waldoTile ].Render();
		g_theRenderer->BindTexture( nullptr );
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

