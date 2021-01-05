#pragma once
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Game/Actor.hpp"
#include "Engine/Math/IntVec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapDefinition;

//--------------------------------------------------------------------------------------------------------------------------------------------

typedef int TileIndex;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Map
{
	friend class Game;
	friend class World;

//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	Map( Game* game, MapDefinition* mapDefinition, std::string mapName );
	~Map();

	void Update( float deltaSeconds );
	void Render() const;
	void GarbageCollection();

	void OuterBoundaryWalls();
	void InitializeTileVertices();
	void SpawnWorms( int numberOfWormsToSpawn , TileDefinition* wormType );


	void CheckCollisions();
	void ResolveEntityWorldCollision( Entity* entity );

	int GetTileIndexforTileCoords( const IntVec2& tileCoords ) const;
	IntVec2 GetTileCoordsforTileIndex( int tileIndex ) const;
	Tile* GetTileAtTileCoords( const IntVec2& tileCoords );
	Tile* GetNorthTileForTileCoords( const IntVec2& tileCoords );
	Tile* GetSouthTileForTileCoords( const IntVec2& tileCoords );
	Tile* GetEastTileForTileCoords( const IntVec2& tileCoords );
	Tile* GetWestTileForTileCoords( const IntVec2& tileCoords );
	Tile* GetNorthEastTileForTileCoords( const IntVec2& tileCoords );
	Tile* GetNorthWestTileForTileCoords( const IntVec2& tileCoords );
	Tile* GetSouthEastTileForTileCoords( const IntVec2& tileCoords );
	Tile* GetSouthWestTileForTileCoords( const IntVec2& tileCoords );

	bool CanEntityMoveInTile( const Entity* entity, const Tile* tile ) const;

	std::string	GetTileInfoForTileCoords( IntVec2 tileCoords );
	std::string	GetMapInfo();
	bool IsMousePointingActor( Vec2 mousePositionInCameraView , Actor* actor );

	//--------------------------------------------------------------------------------------------------------------------------------------------
public:
	IntVec2					m_dimensions;
	std::vector<Tile>		m_tiles;

private:
	MapDefinition*			m_mapDefinition;
	Game*					m_theGame;
	Actor*					m_player;
	std::string				m_name;
	std::vector<Vertex_PCU> m_tileVerts;
};

//--------------------------------------------------------------------------------------------------------------------------------------------