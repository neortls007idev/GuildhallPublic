#include "Game/Entites/Enemies/bettle.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Bettle::Bettle(Game* currentGameInstance, Vec2 pos, Vec2 velocity, float orientation) :Entity(currentGameInstance, pos, velocity, orientation)
{
	InitializeMemberVariables();
}

void Bettle::InitializeMemberVariables()
{
	m_health = 3;
	m_isDead = false;
	m_isGarbage = false;
	m_physicsRadius = 2.75f;
	m_cosmeticRadius = 3.5f;

	RandomNumberGenerator rngBettle;
	m_position.x = (float)rngBettle.RollRandomWithinTwoDistinctRanges(-2, 0, 200, 202);
	m_position.y = (float)rngBettle.RollRandomWithinTwoDistinctRanges(-2, 0, 200, 202);
}

void Bettle::Update(float deltaSeconds)
{
	PlayerShip* player = m_game->m_Ship;
	if (player)
	{
		if (m_game->m_Ship->m_health > 0)
		{
			Vec2 displacementToPlayer = player->m_position - m_position;
			m_orientationDegrees = displacementToPlayer.GetAngleDegrees();
		}
		m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, WASP_ACCELERATION);
		Entity::Movement(deltaSeconds);
		m_position.x = Clamp(m_position.x, -5.f, 205.f);
		m_position.y = Clamp(m_position.y, -5.f, 205.f);
	}
}

void Bettle::Render() const
{
	Vertex_PCU tempVerts[NUM_BETTLE_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BETTLE_VERTS; vertIndex++)
	{
		tempVerts[vertIndex] = m_localBettleVerts[vertIndex];
	}
	TransformVertexArray2D(NUM_BETTLE_VERTS, tempVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_BETTLE_VERTS, tempVerts);
}

void Bettle::Die()
{

}


