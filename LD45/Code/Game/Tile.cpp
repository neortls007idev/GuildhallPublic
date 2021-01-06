#include "Game/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/TheApp.hpp"

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Tile::Tile( IntVec2 tileCoordinates, Tiletype type ) : m_tileCoords(tileCoordinates), m_type(type)
{

}

void Tile::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Tile::Render() const
{
	static RandomNumberGenerator RNG;
	Rgba8 color;
	color.RollRandomColor( RNG );
	RNG.manuallyIncrementPosition();
	g_theRenderer->DrawAABB2( AABB2( ( float ) m_tileCoords.x , ( float ) m_tileCoords.y , ( float ) m_tileCoords.x + 40 , ( float ) m_tileCoords.y + 40 ) , color );
}

int Tile::GetTileID( const Tile* tile ) const
{
	return tile->m_type;
}


