#pragma once
#include "Game/Entity.hpp"
#include <vector>
#include "Tile.hpp"
#include "Engine/Math/IntVec2.hpp"

class Map
{
	friend class Game;
	friend class World;

public:

	Map( Game* theGame , IntVec2 size );

	void OuterBoundaryWalls();
	void InitializePlayerTile();
	void RandomizeObstacleTiles();
	void SafeZones();
	void SafeZonesBoundary();

	~Map() {};

	void Update( float deltaSeconds );
	void Render() const;

	int GetTileIndexforTileCoords( const IntVec2& tileCoords ) const;
	IntVec2 GetTileCoordsforTileIndex( int tileIndex ) const;


public:
	std::vector< Entity* >      m_entities;
	std::vector< Tile >			m_tiles;
	//std::vector< Vertex_PCU >	
	IntVec2						m_Size = IntVec2( 0 , 0 );
	Game* m_theGame = nullptr;
	float m_age = 0;
};
/*
Map( Game* theGame , World* world , const IntVec2& size ) {

}*/