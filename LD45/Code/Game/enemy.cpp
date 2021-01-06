#include "Game/enemy.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"

RandomNumberGenerator rng;
extern RenderContext* g_theRenderer;

Enemy::Enemy( Game* currentGameInstance ) : Entity( currentGameInstance , Vec2( 0.f , 0.f ) , Vec2( 0.f , 0.f ) , 0.f )
{
	InitializeMemberVariables();
}

void Enemy::Update( float deltaSeconds )
{
	Entity::Movement( deltaSeconds );
	RollEnemy( deltaSeconds );
	Entity::WrapAroundScreen();
}

void Enemy::Render() const
{
//	TransformVertexArray2D( NUM_ASTEROID_VERTS , tempVerts , 1.f , m_orientationDegrees , m_position );
//	g_theRenderer->DrawVertexArray( NUM_ASTEROID_VERTS , tempVerts );

}

void Enemy::Die()
{

}

void Enemy::RollEnemy( float deltaSeconds )
{
	m_orientationDegrees += m_angularVelocity * deltaSeconds;
}

void Enemy::InitializeMemberVariables()
{
	{
		m_position = Vec2( ( float ) rng.RollRandomIntInRange( 20 , 180 ) , ( float ) rng.RollRandomIntInRange( 20 , 80 ) );
		rng.manuallyIncrementPosition();
		m_orientationDegrees = ( float ) rng.RollRandomIntInRange( 0 , 360 );
		rng.manuallyIncrementPosition();
		m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees , 10.f );
		m_angularVelocity = ( float ) rng.RollRandomIntInRange( -200 , 200 );
		rng.manuallyIncrementPosition();

		m_health = 3;
		m_isDead = false;
		m_isGarbage = false;
		m_physicsRadius = 1.6f;
		m_cosmeticRadius = 2.0f;
	}
}