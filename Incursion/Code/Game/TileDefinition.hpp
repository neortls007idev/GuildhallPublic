#pragma once
#include "Game/Tile.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class TileDefinition
{
public:

//--------------------------------------------------------------------------------------------------------------------------------------------

	explicit TileDefinition( SpriteDefinition spriteDefinition , Rgba8 tint , bool isSolid );
	//Rgba8 GetColorForTileType( Tiletype type ) const;
	//bool  IsTileTypeSolid( Tiletype type ) const;
	static void  CreateTileDefinitions();
	void  GetSpriteSheetUv( Vec2 minSpriteSheetUVs, Vec2 maxSpriteSheetUvs ) ;

//--------------------------------------------------------------------------------------------------------------------------------------------

private:

//--------------------------------------------------------------------------------------------------------------------------------------------

public:
	
	static std::vector<TileDefinition>  s_definitions;
	bool							    m_isSolid = false;
	SpriteDefinition					m_spriteDfinition;
	Vec2								m_minUVs;
	Vec2								m_maxUVs;
	Rgba8							    m_tint;

private:

};
