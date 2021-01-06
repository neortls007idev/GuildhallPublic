#pragma once
#include "GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class PowerUpDefinition;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum ePowerUpType : uchar
{
	PT_INVALID				= 0,

	PT_BALLX2				,
	PT_BALLX3				,
	PT_BALLX8				,
	PT_BRICK_THROUGH		,
	PT_BRICK_ALL_VISIBLE	,
	PT_BRICK_ALL_SOFT		,
	PT_EXTRA_LIFE			,
	PT_DEATH				,
//	PT_PADDLE_SHOOT			= 7,

	NUM_POWERUP_TYPES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class PowerUps : public Entity
{
public:
	PowerUps( Game* owner , Vec2 position , Vec2 velocity , ePowerUpType type = PT_INVALID );
	~PowerUps();
	
			void		Update( float deltaSeconds ) override;
			void		Move( float deltaSeconds );
	virtual void		Render() const override;
	
			void		PowerUpEffects();
			void		PowerUpEffectMultipleBalls( uint numballs );
			void		PowerUpEffectBrickThrough();
			void		PowerUpEffectScaleBall( float scale );
			void		PowerEffectDeath();
			void		PowerEffectExtraLife();
			void		PowerEffectAllBricksSoft();
			void		PowerEffectAllBricksVisible();
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
public:
	AABB2				m_cosmeticBounds		= AABB2( -POWERUP_HALF_DIMENSIONS , POWERUP_HALF_DIMENSIONS );

	float				m_physicsRadius			= POWERUP_PHYSICS_RADIUS;
	Vec2				m_pos					= Vec2::ZERO;
	Vec2				m_velocity				= Vec2::ZERO;

	IntVec2				m_spriteCoords			= IntVec2::ZERO;
	Vec2				m_minUVs				= Vec2::ZERO;
	Vec2				m_maxUVs				= Vec2::ONE;
		
	ePowerUpType		m_type					= PT_INVALID;
	PowerUpDefinition*	m_definition			= nullptr;
	
};

