#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Primitives/AABB2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class TileDefinition;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct Tile
{

public:
	Tile( IntVec2 tileCoordinates , TileDefinition* tileType ); // or alternate Tile( int tileX, int tileY, TIleType type)
	~Tile() {};
	void Update( float deltaSeconds );
	void Render() const;
	bool IsSolid() const;
	
public:
	IntVec2				m_tileCoords;
	TileDefinition*		m_tileDef			= nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------