#include "Game/Paddle.hpp"
#include "Game/Game.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;
extern InputSystem*		g_theInput;

//--------------------------------------------------------------------------------------------------------------------------------------------

Paddle::Paddle( Game* owner , int health , AABB2 paddleCollider , Vec2 startPos ) :
																	Entity( owner , health , eEntityType::PADDLE ) ,
																	m_paddleCollider( paddleCollider ) ,
																	m_position( startPos )
{
	m_paddleCollider.Translate( m_position );
	g_theGame->m_gameSS[ SS_BALLGRAB ]->RollRandomSpriteUVs( m_grabEffectUVs.m_mins , m_grabEffectUVs.m_maxs );
	m_grabEffectAnim = new SpriteAnimDefinition( *g_theGame->m_gameSS[ SS_BALLGRAB ] , 0 , 9 , m_grabEffectUpdateTimer );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Paddle::~Paddle()
{
	delete m_grabEffectAnim;
	m_grabEffectAnim = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Paddle::Update( float deltaSeconds )
{
	UpdateFromUserInput( deltaSeconds );

	m_grabEffectAnim->GetSpriteDefAtTime( m_grabEffectUpdateTimer ).GetUVs( m_grabEffectUVs.m_mins , m_grabEffectUVs.m_maxs );
	m_grabEffectUpdateTimer -= deltaSeconds;
	if ( m_grabEffectUpdateTimer <= 0.f )
	{
		m_grabEffectUpdateTimer = 0.5f;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Paddle::Render() const
{
	Vec2 paddleDim = m_paddleCollider.GetDimensions();
	AABB2 ballGrabEffect = AABB2::ZERO_TO_ONE;
	ballGrabEffect.SetDimensions( paddleDim.x * 0.75f , 175.f );
	Vec2 center = m_paddleCollider.GetCenter();
	center += ( m_paddleCollider.GetDimensions() * 0.5f + Vec2( 0.f , 37.5f ) );
	ballGrabEffect.SetCenter( m_paddleCollider.GetCenter().x , center.y );
	
	if ( g_theGame->m_isBallLaunchable )
	{
		
		Texture* ballGrabTex = const_cast< Texture* >( &g_theGame->m_gameSS[ SS_BALLGRAB ]->GetTexture() );
		g_theRenderer->SetBlendMode( ADDITIVE );
		g_theRenderer->BindTexture( ballGrabTex );
		g_theRenderer->DrawAABB2( ballGrabEffect , WHITE , m_grabEffectUVs.m_mins , m_grabEffectUVs.m_maxs );
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->SetBlendMode( ALPHA );
	}
	
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_PADDLE ] );
	g_theRenderer->DrawAABB2( m_paddleCollider , ORANGE );
	g_theRenderer->BindTexture( nullptr );

	if ( m_owner->m_isDebugDraw )
	{
		
		g_theRenderer->DrawUnfilledAABB2( ballGrabEffect	, MAGENTA	, 2.5f );
		g_theRenderer->DrawUnfilledAABB2( m_paddleCollider	, MAGENTA	, 2.5f );
		g_theRenderer->DrawUnfilledAABB2( m_paddleCollider	, CYAN		, 2.5f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Paddle::UpdateFromUserInput( float deltaSeconds )
{
	Vec2 mousePos = g_theInput->GetRelativeMovement();

	float speed = 20.f * g_theGame->m_paddleSensitivity;

	float paddleColliderLength = m_paddleCollider.GetDimensions().x;

	Vec2 translate = Vec2::ZERO;
	translate.x = mousePos.x * speed * deltaSeconds;
	
	m_position.x += translate.x;
	m_position.x = Clamp( m_position.x , m_owner->m_currentLevel->m_leftWallPhysicalBounds.m_maxs.x + paddleColliderLength * 0.5f,
	                      m_owner->m_currentLevel->m_rightWallPhysicalBounds.m_mins.x - paddleColliderLength * 0.5f );

	translate.x = m_position.x;
	
	m_owner->m_currentLevel->UpdateBallPosWhenGrabbed( translate.x );
	
	m_paddleCollider.SetCenter( m_position );

}

//--------------------------------------------------------------------------------------------------------------------------------------------
