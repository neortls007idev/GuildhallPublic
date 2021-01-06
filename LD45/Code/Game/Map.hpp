#pragma once
#include "Game/Entity.hpp"
#include <vector>
#include "Tile.hpp"
#include "Engine/Math/IntVec2.hpp"

class Map
{

public:

	Map( Game* theGame, IntVec2 size);
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
	Game* m_game = nullptr;
	float m_age = 0;
};