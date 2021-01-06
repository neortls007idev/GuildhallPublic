#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/TileDefinition.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum EntityType
{
	INVALID_ENTITY = -1,
	PLAYERTANK_ENTITY,
	NPCTANK_ENTITY	 ,
	NPCTURRET_ENTITY ,
	NPCBOULDER_ENTITY,
	GOOD_BULLET_ENTITY,
	EVIL_BULLET_ENTITY,
	EXPLOSION_ENTITY,

	NUM_ENTITY_TYPES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum Faction
{
	INVALID_FACTION = -1 ,
	FACTION_ALLY,
	FACTION_ENEMY,
	FACTION_NEUTRAL,
	
	NUM_FACTIONS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Entity {
//	friend class Map;

public:

	Entity( Game* pointerToGameInstance , Vec2 pos , Vec2 velocity , float orientation , EntityType type , Faction faction );
	Entity( Game* pointerToGameInstance , Vec2 pos , float orientation );
	virtual void Update(float deltaSeconds);

	void Movement(float deltaSeconds);
	void WrapAroundScreen();
	void TakeDamage();

	virtual void Render() const;
	virtual void IsDead();

	bool IsAlive() const;
	bool IsGarbage() const;

//--------------------------------------------------------------------------------------------------------------------------------------------

public:
	int m_entityID			   = 0;
	EntityType m_entityType	   = INVALID_ENTITY;
	Faction m_faction		   = INVALID_FACTION;

	Vec2 m_velocity			   = Vec2::ZERO;

	Vec2 m_position			   = Vec2::ZERO;
	float m_orientationDegrees = 0.f;
	float m_angularVelocity    = 0.f;
	int	  m_health			   = 1;
	bool  m_isDead			   = true;
	bool  m_isGarbage		   = true;

	float m_physicsRadius	   = 0.f;
	float m_cosmeticRadius	   = 0.f;

	float m_age				   = 0.f;

	bool  m_pushesEntiites	   = false;
	bool  m_isPushedByWalls	   = false;
	bool  m_isPushedByEntities = false;
	bool  m_isHitByBullets	   = false;

	const Rgba8 m_cosmeticRing = MAGENTA;
	const Rgba8 m_physicsRing  = CYAN;

	Game* m_theGame;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct RayCastResult
{

public:

	bool		m_didImpact = false;
	Vec2		m_impactPosition = Vec2::ZERO;
	float		m_impactDistance = 0.f;
	Vec2		m_impactNormal = Vec2::ZERO;
	EntityType	m_impactEntityType = INVALID_ENTITY;
	float		m_impactFraction = 0.f;

};


//--------------------------------------------------------------------------------------------------------------------------------------------
//				DATATYPE  ALIASES
//--------------------------------------------------------------------------------------------------------------------------------------------

typedef std::vector<Entity*>Entitylist;

//--------------------------------------------------------------------------------------------------------------------------------------------