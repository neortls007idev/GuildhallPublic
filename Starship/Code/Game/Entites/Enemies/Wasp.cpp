#include "Game/Entites/Enemies/Wasp.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Wasp::Wasp(Game* currentGameInstance, Vec2 pos, Vec2 velocity, float orientation): Entity(currentGameInstance, Vec2(0.f, 0.f), Vec2(0.f, 0.f), orientation)
{
	InitializeMemberVariables();
}

void Wasp::InitializeMemberVariables()
{
	m_health = 3;
	m_isDead = false;
	m_isGarbage = false;
	m_physicsRadius = 2.75f;
	m_cosmeticRadius = 3.5f;

	RandomNumberGenerator rngWasp;
	m_position.x = (float)rngWasp.RollRandomWithinTwoDistinctRanges(-2, 0, 200, 202);
	m_position.y = (float)rngWasp.RollRandomWithinTwoDistinctRanges(-2, 0, 200, 202);
}

void Wasp::Update(float deltaSeconds)
{
	PlayerShip* player = m_game->m_Ship;
	if (player)
	{
		if (m_game->m_Ship->m_health>0)
		{
		Vec2 displacementToPlayer = player->m_position - m_position;
		m_orientationDegrees = displacementToPlayer.GetAngleDegrees();
		m_acceleration = Vec2::MakeFromPolarDegrees(m_orientationDegrees, WASP_ACCELERATION);
		m_velocity += (m_acceleration * deltaSeconds);
		}
		m_velocity.ClampLength(20.f);
		Entity::Movement(deltaSeconds);
		m_position.x = Clamp(m_position.x, -5.f, 205.f);
		m_position.y = Clamp(m_position.y, -5.f, 205.f);

	}
}

void Wasp::Render() const
{
	Vertex_PCU tempVerts[NUM_WASP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; vertIndex++)
	{
		tempVerts[vertIndex] = m_localWaspVerts[vertIndex];
	}
	TransformVertexArray2D(NUM_WASP_VERTS, tempVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_WASP_VERTS, tempVerts);
}

void Wasp::Die()
{

}

