#include "Game/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;
extern Game* g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile::Tile( IntVec2 tileCoordinates, TileDefinition* tileType ) : 
																	m_tileCoords( tileCoordinates ),
																	m_tileDef( tileType )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::SetTileType( TileDefinition* tileType )
{
	m_tileDef = tileType;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition* Tile::GetTileType() const
{
	return m_tileDef;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

IntVec2 Tile::GetTileCoordinates()
{
	return m_tileCoords;
}
