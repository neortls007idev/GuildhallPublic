#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class SpriteSheet;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct BallTrailEffect
{
public:
	BallTrailEffect( Vec2 startPos , Vec2 endPos, Rgba8 startColor , Vec2 lastBlockEndPos );
	~BallTrailEffect(){};
	
	void Update( float deltaSeconds );
	
public:
	Rgba8 m_startColor;
	Rgba8 m_endColor;
	Rgba8 m_currentColor;
	float m_currentAge			= 0.f;

	Vec2  m_startPos			= Vec2::ZERO;
	Vec2  m_endPos				= Vec2::ZERO;
	Vec2  m_lastBlockEndPos		= Vec2::ZERO;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball : public Entity
{
public:
	Ball( Game* owner , int health , float physicsRadius , Vec2 position , Vec2 velocity , IntVec2 spriteCoords , eEntityType type = BALL );
	~Ball();

	void Update( float deltaSeconds ) override;
	void CreateBallTrailEffect( float deltaSeconds );

	void EmplaceBackTrailEffect( BallTrailEffect* trail );

	//--------------------------------------------------------------------------------------------------------------------------------------------


	void Render() const override;

	void Move( float deltaSeconds );
	void AddVelocityNudge();
	void UpdateCurrentTexture( Rgba8 newColor );
	void UpdateCurrentTexture( IntVec2 spriteCoords );
	void UpdateBounds( float scale );
	
public:
	float							m_physicsRadius;
	Vec2							m_pos;
	Vec2							m_velocity;
	SpriteSheet*					m_currentSpriteSheet				= nullptr;
	float							m_ballPaddleMaxCoolDownTime			= BALL_PADDLE_MAX_COLLISION_COOLDOWN_TIME;
	bool							m_isInCooldown						= false;
	AABB2							m_cosmeticRadius;
	IntVec2							m_spriteCoords;
	Vec2							m_minUVs;
	Vec2							m_maxUVs;
	Rgba8							m_color;

	bool							m_brickThrough						= false;
	float							m_currentScore						= 0.f;
	size_t							m_currentScoreMultiplier			= 0;

	std::vector< BallTrailEffect* > m_trailEffect;
	std::vector< Vertex_PCU >		m_trailEffectVerts;
	size_t							m_lastSlotIndex						= 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
