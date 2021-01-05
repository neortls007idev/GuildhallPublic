#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

#include "Game/GameCommon.hpp"
#include "Game/TileDefinition.hpp"

#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Entity {

public:
	Entity( Game* pointerToGameInstance , Vec2 pos , float orientation );
	virtual void Update( float deltaSeconds );
	virtual void Render() const;
	virtual void IsDead();

	void MoveEntity(float deltaSeconds);
	void WrapAroundScreen();
	void TakeDamage();
	
	bool IsAlive() const;
	bool IsGarbage() const;

//--------------------------------------------------------------------------------------------------------------------------------------------

public:
	Vec2  m_velocity			   = Vec2::ZERO;

	Vec2  m_position			   = Vec2::ZERO;
	float m_orientationDegrees = 0.f;
	float m_angularVelocity    = 0.f;
	int	  m_health			   = 1;
	bool  m_isDead			   = true;
	bool  m_isGarbage		   = true;

	float m_physicsRadius	   = 0.f;
	float m_cosmeticRadius	   = 0.f;
	AABB2 m_entityBounds	   = AABB2::ZERO_TO_ONE;

	float m_age				   = 0.f;

	bool  m_canWalk = false;
	bool  m_canSwim = false;
	bool  m_canFly = false;	
		
	const Rgba8 m_cosmeticColor = MAGENTA;
	const Rgba8 m_physicsColor  = CYAN;

	Game* m_theGame;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
//				DATATYPE  ALIASES
//--------------------------------------------------------------------------------------------------------------------------------------------

typedef std::vector<Entity*>Entitylist;

//--------------------------------------------------------------------------------------------------------------------------------------------