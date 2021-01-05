#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Primitives/AABB2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

enum Tiletype /*: unsigned char*/
{
	TILE_TYPE_INVALID = -1 ,
	TILE_TYPE_DEFAULT,
	TILE_TYPE_EDGETILE,
	TILE_TYPE_GRASS,
	TILE_TYPE_STONE,
	TILE_TYPE_WATER,
	TILE_TYPE_LAVA,
	TILE_TYPE_MUD,
	TILE_TYPE_EXIT,
	TILE_TYPE_STARTAREA_TILE,
	TILE_TYPE_EXITAREA_TILE,
	NUM_TILE_TYPES  
};

//--------------------------------------------------------------------------------------------------------------------------------------------

Rgba8 GetColorForTileType( Tiletype type );
bool  IsTileTypeSolid( Tiletype type );

//--------------------------------------------------------------------------------------------------------------------------------------------

struct Tile
{

public:
	Tile( IntVec2 tileCoordinates , Tiletype type ); // or alternate Tile( int tileX, int tileY, TIleType type)
	~Tile() {};
	void Update( float deltaSeconds );
	void Render() const;

	Tiletype GetTileType( ) const;
	void SetTileType( const Tiletype type );
	void SetTileID( const Tiletype type );

public:
	IntVec2 m_tileCoords;
	Tiletype m_type = TILE_TYPE_INVALID;
	//--------------------------------------------------------------------------------------------------------------------------------------------
	// VARIABLES PARKED FOR CHECKING FLOODFILL
	//--------------------------------------------------------------------------------------------------------------------------------------------
	bool m_hasBeenSeen = false;
	bool m_hasBeenProcessed = false;

};

//--------------------------------------------------------------------------------------------------------------------------------------------