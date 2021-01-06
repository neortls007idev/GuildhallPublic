#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball;
class TileDefinition;
class Map;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eTileMove
{
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_CENTER,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Tile : public Entity
{
public:
	Tile( Map* owner , IntVec2 tileCoordinates , TileDefinition* tileType );
		
	void					Update( float deltaSeconds );
	void					Render() const;
	
	int						GetTileIndex();
	IntVec2					GetTileCoordinates()														{ return m_tileCoords; }
	TileDefinition*			GetTileType() const															{ return m_tileDef; }
	void					SetTileType( TileDefinition* tileType )										{ m_tileDef = tileType; }
	bool					TileCollisionResponse( Ball* ball );

	void					SpawnParticlesOnBallCollision( Ball* ball , Vec2 refPoint , uint num4XParticles );

	//--------------------------------------------------------------------------------------------------------------------------------------------

	AABB2					CreateColliderFromCoords() const;
	AABB2					GetCollider() const															{ return m_bounds;  }
	void					UpdateCollider();

public:
	bool							m_isSolid										= false;
	bool							m_isVisible										= false;
	
protected:
	float							m_animTime;
	Map*							m_ownerMap										= nullptr;
	IntVec2							m_tileCoords;
	AABB2							m_bounds;
	TileDefinition*					m_tileDef										= nullptr;
	int								m_solidAtHealth									= 0;
	int								m_visibleAtHealth								= 0;
	Rgba8							m_tileColor;
	eTileMove						m_tileAnim;
private:
};

//--------------------------------------------------------------------------------------------------------------------------------------------