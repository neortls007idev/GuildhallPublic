#pragma once
#include "Game/Entity.hpp"
#include "Tile.hpp"
#include "Engine/Math/IntVec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

typedef int TileIndex;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Map
{
	friend class Game;
	friend class World;

//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	Map( Game* theGame , IntVec2 size, int MapNumber );

	void InitialNPCSpawner();

	void InitializeTileVertices();
	void OuterBoundaryWalls();
	void SpawnWorms( int numberOfWormsToSpawn , Tiletype wormType );
	void RandomizeStoneTiles();
	void RandomizeMudTiles();
	void SafeZones();
	void SafeZonesBoundary();
	bool CheckMapValidityUsingFloodFill( IntVec2 startTile , IntVec2 endTile );

	void SpawnNewEntity( EntityType type , Faction faction , const Vec2& position, const float& orientation, const float blastRadius = 1.f, const float animationDuration = 1.f );
	void AddEntityToMap( Entity* entity );
	void AddEntityToList( Entitylist& entityList, Entity* entity );

	void ResolveEntityEntityCollision( Entity& first , Entity& second );
	void ResolveEntityWorldCollision( Entity* entity );
	void ResolveEntityBulletCollision( Entity& entity );
	void CheckCollisions();

	RayCastResult RayCast( Vec2 startPosition , Vec2 forwardDirection , float maxDistance ) const;
	bool InLineOfSight( Vec2& startPosition , Vec2& forwardDirection , float maxDistance );
	bool IsTileSolid( const Tile& tile ) const;
	Vec2 GetTileCenter( IntVec2 tileCOords ) const;

	~Map();

	void Update( float deltaSeconds );
	void CheckNoClipping();
	void Render() const;
	void GarbageCollection();

	int GetTileIndexforTileCoords( const IntVec2& tileCoords ) const;
	IntVec2 GetTileCoordsforTileIndex( int tileIndex ) const;

//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	
	Vec2 m_exitPosition;
	IntVec2						m_size			   = IntVec2( 0 , 0 );
	Game*						m_theGame		   = nullptr;
	float						m_age			   = 0.f;
	std::vector< Tile >			m_tiles;
	std::vector<Vertex_PCU>     m_tileVerts;
	Entitylist					m_entityListsByType[ NUM_ENTITY_TYPES ];
private:
	void SetUnreachedTilesToStone();
	int							m_thisMapNumber = 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------