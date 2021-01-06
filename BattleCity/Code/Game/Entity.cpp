#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"

static int s_entityID = 0;

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::Entity( Game* pointerToGameInstance, Vec2 pos, Vec2 velocity, float orientation, EntityType type , Faction faction )
{
	m_entityID = s_entityID;
	s_entityID++;

	m_theGame = pointerToGameInstance;
	m_position = pos;
	m_velocity = velocity;
	m_orientationDegrees = orientation;
	m_entityType = type;
	m_faction = faction;
}

Entity::Entity( Game* pointerToGameInstance , Vec2 pos , float orientation )
{
	m_theGame = pointerToGameInstance;
	m_position = pos;
	m_orientationDegrees = orientation;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Update (float deltaSeconds)
{
	Movement(deltaSeconds);

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Movement(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::WrapAroundScreen()
{
	if ((m_position.x - m_cosmeticRadius) < 0 || (m_position.x + m_cosmeticRadius) > 200)
	{
		m_velocity.x = -m_velocity.x;
	}
	if ((m_position.y - m_cosmeticRadius) < 0 || (m_position.y + m_cosmeticRadius) > 100)
	{
		m_velocity.y = -m_velocity.y;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::TakeDamage()
{
	m_health -= 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Render() const
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::IsDead()
{
	if (m_health <= 0)
	{
		m_isDead = true;
		m_theGame->AddScreenShakeIntensity(1.f);
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsAlive() const
{
	return !m_isDead;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsGarbage() const
{
	return this==nullptr || m_isGarbage;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

