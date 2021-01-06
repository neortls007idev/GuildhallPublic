#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Ball.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include <map>

#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Core/VertexUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Ball::Ball( Game* owner , int health , float physicsRadius , Vec2 position , Vec2 velocity , IntVec2 spriteCoords , eEntityType type /*= BALL */ ) :
																								Entity( owner , health , type ),
																								m_physicsRadius( physicsRadius ) ,
																								m_pos( position ),
																								m_velocity( velocity ),
																								m_spriteCoords( spriteCoords )
{
	m_currentSpriteSheet = m_owner->m_gameSS[ SS_BALL ];
	m_cosmeticRadius.SetDimensions( Vec2 ( physicsRadius , physicsRadius ) * 3.f );
	m_cosmeticRadius.SetCenter( m_pos.x , m_pos.y );
	UpdateCurrentTexture( spriteCoords );
	m_color = CYAN;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Ball::~Ball()
{
	m_currentSpriteSheet = nullptr;
	m_trailEffect.clear();
	m_trailEffectVerts.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Update( float deltaSeconds )
{
	m_trailEffectVerts.clear();
	
	if ( !m_owner->m_isBallLaunchable )
	{
		Move( deltaSeconds );
		m_cosmeticRadius.SetCenter( m_pos );
		CreateBallTrailEffect( deltaSeconds );
	}
	if ( m_owner->m_isBallLaunchable )
	{
		m_trailEffect.clear();
	}
	
	if ( m_isInCooldown )
	{
		m_ballPaddleMaxCoolDownTime -= deltaSeconds;
	}

	if ( m_ballPaddleMaxCoolDownTime <= 0.f )
	{
		m_isInCooldown = false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::CreateBallTrailEffect( float deltaSeconds )
{
	BallTrailEffect* trail = nullptr;
	
	if ( m_trailEffect.size() == 0  )
	{
		trail = new BallTrailEffect( m_pos , m_pos + m_velocity.GetNormalized() * 3.f, m_color , m_pos );
	}
	else
	{
		trail = new BallTrailEffect( m_pos , m_pos + m_velocity.GetNormalized() * 3.f , m_color , m_trailEffect[ m_lastSlotIndex ]->m_endPos );
	}
	EmplaceBackTrailEffect( trail );
		
	for( size_t index = 0; index < m_trailEffect.size() ; index++ )
	{
		BallTrailEffect* currTrailBlock = m_trailEffect[ index ];
		//BallTrailEffect* lastTrailBlock = m_trailEffect[ index - 1 ];
		
		if( currTrailBlock->m_currentAge > BALL_TRAIL_EFFECT_MAX_AGE )
		{
			delete m_trailEffect[ index ];
			m_trailEffect[ index ] = nullptr;
		}

		if( currTrailBlock != nullptr && index == m_lastSlotIndex )
		{
			currTrailBlock->Update( deltaSeconds );
			AppendExactLine2( m_trailEffectVerts , currTrailBlock->m_startPos , currTrailBlock->m_endPos ,
							  currTrailBlock->m_currentColor , m_physicsRadius * 2.f );
			if( g_theGame->m_isDebugDraw )
			{
				AppendExactLine2( m_trailEffectVerts , currTrailBlock->m_startPos , currTrailBlock->m_endPos ,
								  MAGENTA , 2.f );
			}
		}
		else if( currTrailBlock != nullptr && index != m_lastSlotIndex )
		{
			currTrailBlock->Update( deltaSeconds );

			AppendExactLine2( m_trailEffectVerts , currTrailBlock->m_lastBlockEndPos , currTrailBlock->m_startPos ,
							  currTrailBlock->m_currentColor , m_physicsRadius * 2.f );

			AppendExactLine2( m_trailEffectVerts , currTrailBlock->m_startPos , currTrailBlock->m_endPos ,
								currTrailBlock->m_currentColor , m_physicsRadius * 2.f );

			if ( g_theGame->m_isDebugDraw )
			{
				AppendExactLine2( m_trailEffectVerts , currTrailBlock->m_lastBlockEndPos , currTrailBlock->m_startPos ,
								  MAGENTA , 2.f );
				
				AppendExactLine2( m_trailEffectVerts , currTrailBlock->m_startPos , currTrailBlock->m_endPos ,
								  MAGENTA , 2.f );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::EmplaceBackTrailEffect( BallTrailEffect* trail )
{
	size_t index = 0;
	
	for( index = 0 ; index < m_trailEffect.size() ; index++ )
	{
		if( nullptr == m_trailEffect[ index ] )
		{
			m_trailEffect[ index ]  = trail;
//			m_secondLastSlotIndex	= m_lastSlotIndex;
			m_lastSlotIndex			= index;
			return;
		}
	}
	m_trailEffect.emplace_back( trail );
//	m_secondLastSlotIndex = m_lastSlotIndex;
	m_lastSlotIndex = index;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Render() const
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawVertexArray( m_trailEffectVerts );
	
	g_theRenderer->BindTexture( const_cast< Texture* >( &m_currentSpriteSheet->GetTexture() ) );
	g_theRenderer->DrawAABB2( m_cosmeticRadius , WHITE , m_minUVs , m_maxUVs );
	g_theRenderer->BindTexture( nullptr );

	if ( m_owner->m_isDebugDraw )
	{
		g_theRenderer->DrawUnfilledAABB2( m_cosmeticRadius	, MAGENTA	, 2.5f );
		g_theRenderer->DrawRing( m_pos , m_physicsRadius	, CYAN		, 2.5f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Move( float deltaSeconds )
{
	m_pos += m_velocity * deltaSeconds * 100.f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::AddVelocityNudge()
{
	Vec2 nudgeVector = g_RNG->RollRandomDirection2D();

	m_velocity += nudgeVector;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::UpdateCurrentTexture( IntVec2 spriteCoords )
{
	const SpriteDefinition& currentBallSprite = g_theGame->m_gameSS[ SS_BALL ]->GetSpriteDefinition( spriteCoords );
	currentBallSprite.GetUVs( m_minUVs , m_maxUVs );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::UpdateBounds( float scale )
{
	m_physicsRadius *= scale;
	m_cosmeticRadius.SetDimensions( m_cosmeticRadius.GetDimensions() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

/*
void Ball::UpdateCurrentTexture( Rgba8 newColor )
{
	eGameTextures newTex = TEX_BALL_CYAN;
	
	for( int index = 0 ; index < NUM_GAME_TEX ; index++ )
	{
		if ( g_theBallTexTable[ index ].color == newColor )
		{
			newTex = g_theBallTexTable[ index ].texture;
			break;
		}
	}
	
	m_currentTexture = m_owner->m_gameTex[ newTex ];
}*/


//--------------------------------------------------------------------------------------------------------------------------------------------

BallTrailEffect::BallTrailEffect( Vec2 startPos , Vec2 endPos , Rgba8 startColor , Vec2 lastBlockEndPos )
{
	m_startColor		= startColor;
	m_startColor.a		= 192;
	m_endColor			= startColor;
	m_endColor.a		= 0;
	m_currentColor		= startColor;

	m_startPos			= startPos;
	m_endPos			= endPos;
	m_lastBlockEndPos	= lastBlockEndPos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void BallTrailEffect::Update( float deltaSeconds )
{
	if ( m_currentAge <= BALL_TRAIL_EFFECT_BLOCK_MAX_AGE )
	{
		m_currentColor.LerpColorOverTime( m_startColor , m_endColor , BALL_TRAIL_EFFECT_BLOCK_MAX_AGE , m_currentAge );
	}

	m_currentAge += deltaSeconds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
