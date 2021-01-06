#include "Game/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem*		g_theInput;
extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;
extern TheApp*			g_theApp;
extern Game*			g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile::Tile( IntVec2 tileCoordinates , TileDefinition* tileType ) : 
																	m_tileDef( tileType ) ,
																	m_tileCoords( tileCoordinates )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::Render() const
{
	Rgba8 color = m_tileDef->m_tint;
	g_theRenderer->DrawAABB2( AABB2( ( float ) m_tileCoords.x , ( float ) m_tileCoords.y , ( float ) m_tileCoords.x + 1 , ( float ) m_tileCoords.y + 1 ) , color );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Tile::IsSolid() const
{
	return m_tileDef->m_isSolid;
}

//--------------------------------------------------------------------------------------------------------------------------------------------