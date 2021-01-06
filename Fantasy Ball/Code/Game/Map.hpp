#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Game/Entity.hpp"
#include "Game/PowerUps.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball;
class MapDefinition;
class Game;
class TileDefinition;
class ParticleEmitter2D;
class PowerUps;

//--------------------------------------------------------------------------------------------------------------------------------------------
	typedef std::vector<Entity*> Entitylist;
//--------------------------------------------------------------------------------------------------------------------------------------------
class Map
{
public:
	Map( Game* owner );
	Map( Game* owner , MapDefinition* mapDefinition , std::string mapName );

	void		InitializeLevelBounds();

	//--------------------------------------------------------------------------------------------------------------------------------------------

	~Map();

	void		Update( float deltaSeconds );
	void		Render();

	void		UpdateSideCosmeticBounds();
	void		UpdateTopCosmeticBounds();
	void		RenderLevelBounds();
	void		RenderLevelSideBounds() const;

	//--------------------------------------------------------------------------------------------------------------------------------------------

	Entity*		SpawnNewEntity ( eEntityType type , const Vec2& position , const Vec2& velocity = Vec2::ZERO ,
					             TileDefinition* tileDef = nullptr , ePowerUpType powerUpType = PT_INVALID );
	
	void		AddEntityToMap( Entity* entity );
	void		AddEntityToList( Entitylist& entityList , Entity* entity );

	void		UpdateBallPosWhenGrabbed( float newPosX );

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			COLLISION RESOLUTION
	//--------------------------------------------------------------------------------------------------------------------------------------------
	
	void		ResolveCollisions();
	void		ResolveBallvSingleSideBoundCollision( Ball* ball , AABB2 bounds );
	void		ResolveBallvBoundsCollisions();
	void		ResolveBallvPaddleCollisions();

	void		ResolveBallvTileCollisions();

	void		ResolveBallvSingleSideBoundCollision( PowerUps* powerUps , AABB2 bounds );
	void		ResolvePowerUpvBoundsCollisions();
	void		ResolvePaddlevPowerUpCollisions();

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			SPAWN PARTICLE EFFECTS
	//--------------------------------------------------------------------------------------------------------------------------------------------
	
	void		SpawnParticlesOnBallCollisionUsingEmitter( Ball* ball , Vec2 refPoint , uint num4XParticles , ParticleEmitter2D* emitter ,
														   float minLifeTime , float maxLifeTime , Vec2 dimensions , float velocityMultiplier );

	void		SpawnRandomParticleUsingEmitter ( Ball* ball , Vec2 refPoint , float minLifeTime , float maxLifeTime ,
					                              Vec2 dimensions , float velocityMultiplier , ParticleEmitter2D* emmiter );
	//--------------------------------------------------------------------------------------------------------------------------------------------

	Entity*		GetFirstValidEntryInList( eEntityType type );
	Entitylist* GetEntityList( eEntityType type );
	void		GarbageCollection();
	void		DestroyEmitters();
public:
	Game*							m_owner;
	AABB2							m_backGround;

	AABB2							m_leftWallPhysicalBounds;
	AABB2							m_rightWallPhysicalBounds;
	AABB2							m_topWallPhysicalBounds;
	AABB2							m_pitPhysicalBounds;

	AABB2							m_leftWallCosmeticBounds;
	AABB2							m_rightWallCosmeticBounds;
	AABB2							m_topWallCosmeticBounds;
	AABB2							m_pitCosmeticBounds;
	
	IntVec2							m_dimensions;
	MapDefinition*					m_mapDefinition											= nullptr;
	std::string						m_name;
	Entitylist						m_entityListsByType[ NUM_ENTITY_TYPES ];
	ParticleEmitter2D*				m_tileEmitter;
	ParticleEmitter2D*				m_boundsEmitter;
	ParticleEmitter2D*				m_paddleEmitter;
	int								m_backgroundIndex										= 0;
	int								m_numAliveBalls											= 0;
	int								m_numAliveTiles											= 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

