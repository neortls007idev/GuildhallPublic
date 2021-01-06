#include "Game/Bullet.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;
extern Game* g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

Bullet::Bullet( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction ) :
	Entity( CurrentGameInstance ,
		position ,
		velocity ,
		orientation ,
		type ,
		faction
	)
{
	InitializeBulletMemberVariables();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Bullet::Update( float deltaSeconds )
{
	Entity::Movement( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Bullet::Render() const
{
	g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_BULLET ] );
	Vertex_PCU tempTank[ 6 ];
	for ( int index = 0; index < 6; index++ )
	{
		tempTank[ index ] = m_bulletVerts[ index ];
	}
	TransformVertexArray2D( 6 , tempTank , 1.f , m_orientationDegrees , m_position );
	g_theRenderer->DrawVertexArray( 6 , tempTank );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_PLAYER_TURRET ] );
	g_theRenderer->BindTexture( nullptr );
	
//--------------------------------------------------------------------------------------------------------------------------------------------

	//if ( g_theGame->m_debugDraw )
	//{
	//	g_theRenderer->DrawRing( m_position , m_cosmeticRadius , m_cosmeticRing , 0.02f );
	//	g_theRenderer->DrawRing( m_position , m_physicsRadius , m_physicsRing , 0.02f );
	//}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Bullet::IsDead()
{
	m_isDead = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Bullet::InitializeBulletMemberVariables()
{
	Vec2 spawnOffset = Vec2::MakeFromPolarDegrees( m_orientationDegrees , 0.4f );
	m_position += spawnOffset;
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees , 2.0f );
	m_health = 1;
	m_isDead = false;
	m_isGarbage = false;
	m_physicsRadius = 0.05f;
	m_cosmeticRadius = 0.06f;
	m_age = 0.0f;

	m_isPushedByEntities = false;
	m_pushesEntiites = false;
	m_isPushedByWalls = false;
	m_isHitByBullets = false;

}

//--------------------------------------------------------------------------------------------------------------------------------------------