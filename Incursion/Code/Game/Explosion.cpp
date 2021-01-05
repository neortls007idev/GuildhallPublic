#include "Game/Explosion.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;
extern Game* g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

Explosion::Explosion( Game* theGame , Vec2 position, float orientation, float blastRadius, float animationDuration ) :Entity( theGame , position, orientation )
{
	//m_orientationDegrees = 0.f;
	m_isGarbage = false;
	m_isDead = false;
	m_faction = FACTION_NEUTRAL;
	m_entityType = EXPLOSION_ENTITY;
	m_age = 0.0f;
	m_animationDuration = animationDuration;
	m_blastRadius = blastRadius;

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Explosion::Update( float deltaSeconds )
{
	if ( m_age <= m_animationDuration )
	{
		m_age += deltaSeconds;
	}
	else
	{
		IsDead();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Explosion::Render() const
{
	if ( !m_isDead )
	{
		Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Explosion_5x5.png" );
		SpriteSheet* spriteSheet = new SpriteSheet( *texture , IntVec2( 5 , 5 ) );
		SpriteAnimDefinition anim = SpriteAnimDefinition( *spriteSheet , 0 , 24 , m_animationDuration , SpriteAnimPlaybackType::ONCE );
		const SpriteDefinition& explosion = anim.GetSpriteDefAtTime( m_age );
		Vec2 uvMins;
		Vec2 uvMaxs;
		explosion.GetUVs( uvMins , uvMaxs );

		Vertex_PCU tempExplosion[ 6 ];
		for ( int index = 0; index < 6; index++ )
		{
			tempExplosion[ index ] = m_ExplosionVerts[ index ];
		}
		tempExplosion[ 0 ].m_uvTexCoords = uvMins;
		tempExplosion[ 1 ].m_uvTexCoords = Vec2( uvMaxs.x , uvMins.y );
		tempExplosion[ 2 ].m_uvTexCoords = Vec2( uvMins.x , uvMaxs.y );

		tempExplosion[ 3 ].m_uvTexCoords = Vec2( uvMaxs.x , uvMins.y );
		tempExplosion[ 4 ].m_uvTexCoords = uvMaxs;
		tempExplosion[ 5 ].m_uvTexCoords = Vec2( uvMins.x , uvMaxs.y );

		TransformVertexArray2D( 6 , tempExplosion , m_blastRadius , m_orientationDegrees , m_position );
		g_theRenderer->BindTexture( texture );
		g_theRenderer->SetBlendMode( eBlendMode::ADDITIVE );
		g_theRenderer->DrawVertexArray( 6 , tempExplosion );
		g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
		g_theRenderer->BindTexture( nullptr );
	}
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Explosion::IsDead()
{
	m_isDead = true;
	m_isGarbage = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------


