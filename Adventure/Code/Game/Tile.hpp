#pragma once
#include "Engine/Math/IntVec2.hpp"

class TileDefinition;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct Tile
{

public:
	Tile( IntVec2 tileCoordinates , TileDefinition* tileType );
	~Tile() {};

	// GetTileType() const;
	void SetTileType( TileDefinition* tileType );
	TileDefinition* GetTileType() const;
	IntVec2 GetTileCoordinates();
	

protected:
	IntVec2			m_tileCoords;
	TileDefinition* m_tileDef = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------