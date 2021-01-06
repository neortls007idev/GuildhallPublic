#include "Game/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/TheApp.hpp"

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;


Tile::Tile( IntVec2 tileCoordinates , Tiletype type ) : m_tileCoords( tileCoordinates ) , m_type( type )
{

}

void Tile::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Tile::Render() const
{
	Rgba8 color = GetColorForTileType( m_type );
	g_theRenderer->DrawAABB2( AABB2( ( float ) m_tileCoords.x , ( float ) m_tileCoords.y , ( float ) m_tileCoords.x + 1 , ( float ) m_tileCoords.y + 1 ) , color );
}

Tiletype Tile::GetTileType( const Tile* tile ) const
{
	return tile->m_type;
}


void Tile::SetTileType( const Tiletype type )
{
	m_type = type;
}

void Tile::SetTileID( const Tiletype type )
{
	m_type = type;
}

Rgba8 GetColorForTileType( Tiletype type )
{
	switch ( type )
	{
	case TILE_TYPE_INVALID:
		return Rgba8( 0 , 0 , 0 );
		break;
	case TILE_TYPE_GRASS:
		return Rgba8( 0 , 80 , 0 );
		break;
	case TILE_TYPE_STONE:
		return Rgba8( 150 , 150 , 150 );
		break;
	case TILE_TYPE_WATER:
		return Rgba8( 100 , 220 , 225 );
		break;
	case TILE_TYPE_LAVA:
		return Rgba8( 197 , 123 , 26 );
		break;
	default:
		return Rgba8( 0 , 0 , 0 );
		break;
	}
}

bool IsTileTypeSolid( Tiletype type )
{
	switch ( type )
	{
	case TILE_TYPE_INVALID:
		return false;
		break;
	case TILE_TYPE_GRASS:
		return false;
		break;
	case TILE_TYPE_STONE:
		return true;
	case TILE_TYPE_WATER:
		return false;
		break;
	case TILE_TYPE_LAVA:
		return false;
		break;
	case NUM_TILE_TYPES:
		return false;
		break;
	default:
		return false;
		break;
	}
}
