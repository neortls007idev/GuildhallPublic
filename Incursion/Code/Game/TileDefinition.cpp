#include "Game/TileDefinition.hpp"
#include "Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------
//			STATIC VARIABLES DEFINITION
//--------------------------------------------------------------------------------------------------------------------------------------------

std::vector<TileDefinition> TileDefinition::s_definitions;


//extern Game* g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition::TileDefinition( SpriteDefinition spriteDefinition , Rgba8 tint , bool isSolid ) : m_spriteDfinition( spriteDefinition ) , m_tint( tint ) , m_isSolid( isSolid )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------------------------------

void TileDefinition::CreateTileDefinitions()
{
	Texture* tileTerrain8x8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	SpriteSheet* tileTerrains = new SpriteSheet( *tileTerrain8x8 , IntVec2( 8 , 8 ) );

	const SpriteDefinition& defaultTerrain = tileTerrains->GetSpriteDefinition( 1 );
	s_definitions.push_back( TileDefinition( defaultTerrain , WHITE , false ) );

	const SpriteDefinition& edgeTileTerrain = tileTerrains->GetSpriteDefinition( 53 );
	s_definitions.push_back( TileDefinition( edgeTileTerrain , Rgba8( 195 , 195 , 195 ) , false ) );
	
	const SpriteDefinition& grassTerrain = tileTerrains->GetSpriteDefinition( 1 );
	s_definitions.push_back( TileDefinition( grassTerrain , Rgba8( 0 , 80 , 0 ), false ) );

	const SpriteDefinition& stoneTerrain = tileTerrains->GetSpriteDefinition( 33 );
	s_definitions.push_back( TileDefinition( stoneTerrain , Rgba8( 150 , 150 , 150 ) , true ) );
	
	const SpriteDefinition& waterTerrain = tileTerrains->GetSpriteDefinition( 61 );
	s_definitions.push_back( TileDefinition( waterTerrain , Rgba8( 100 , 220 , 225 ) , false ) );

	const SpriteDefinition& lavaTerrain = tileTerrains->GetSpriteDefinition( 62 );
	s_definitions.push_back( TileDefinition( lavaTerrain , Rgba8( 197 , 123 , 26 ) , false ) );
		
	const SpriteDefinition & mudTerrain = tileTerrains->GetSpriteDefinition( 20 );
	s_definitions.push_back( TileDefinition( mudTerrain , Rgba8( 145 , 80 , 15 ) , false ) );

	const SpriteDefinition& exitPosition = tileTerrains->GetSpriteDefinition( 57 );
	s_definitions.push_back( TileDefinition( exitPosition , WHITE , false ) );

	const SpriteDefinition& startAreaTerrain = tileTerrains->GetSpriteDefinition( 51 );
	s_definitions.push_back( TileDefinition( startAreaTerrain , WHITE , false ) );

	const SpriteDefinition& exitAreaTerrain = tileTerrains->GetSpriteDefinition( 58 );
	s_definitions.push_back( TileDefinition( exitAreaTerrain , YELLOW , false ) );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------------------------------

void TileDefinition::GetSpriteSheetUv( Vec2 minSpriteSheetUVs , Vec2 maxSpriteSheetUvs )
{
	//m_spriteSheetMinUVs = minSpriteSheetUVs;
	//m_spriteSheetMaxUVs = maxSpriteSheetUvs;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
