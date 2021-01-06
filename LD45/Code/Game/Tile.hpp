#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Primitives/AABB2.hpp"

enum Tiletype
{
	TILE_TYPE_INVALID	= -1,
	TILE_TYPE_GRASS		=  0,
	TILE_TYPE_WATER		=  2,
	TILE_TYPE_LAVA		=  3,
	NUM_TILE_TYPES		= 4
};

struct Tile
{

public:
	Tile( IntVec2 tileCoordinates, Tiletype type );
	~Tile() {};
	void Update( float deltaSeconds );
	void Render() const;

	int GetTileID( const Tile* tile ) const;
	//int SetTileID( const Tile* tile );

public:
	IntVec2 m_tileCoords;
	Tiletype m_type = TILE_TYPE_INVALID;
	

};