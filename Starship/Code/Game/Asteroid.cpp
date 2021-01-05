#include "Game/Asteroid.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"

RandomNumberGenerator rng;
extern RenderContext* g_theRenderer;

Asteroid::Asteroid(Game* currentGameInstance) : Entity(currentGameInstance, Vec2(0.f, 0.f), Vec2(0.f, 0.f), 0.f)
{
	InitializeMemberVariables();
	InitializeAsteroidVertices();
}

void Asteroid::Update(float deltaSeconds)
{
	Entity::Movement(deltaSeconds);
	RollAsteroid(deltaSeconds);
	Entity::WrapAroundScreen();
}

void Asteroid::Render() const
{
	Vertex_PCU tempVerts[NUM_ASTEROID_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; vertIndex++)
	{
		tempVerts[vertIndex] = m_asteroidVerts[vertIndex];
	}
	TransformVertexArray2D(NUM_ASTEROID_VERTS, tempVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_ASTEROID_VERTS, tempVerts);

}

void Asteroid::Die()
{

}

void Asteroid::RollAsteroid(float deltaSeconds)
{
	m_orientationDegrees += m_angularVelocity * deltaSeconds;
}

void Asteroid::InitializeMemberVariables()
{
	{
		m_position = Vec2((float)rng.RollRandomIntInRange(20, 180), (float)rng.RollRandomIntInRange(20, 80));

		m_orientationDegrees = (float)rng.RollRandomIntInRange(0, 360);
		m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, 10.f);
		m_angularVelocity = (float)rng.RollRandomIntInRange(-200, 200);

		m_health = 3;
		m_isDead = false;
		m_isGarbage = false;
		m_physicsRadius = 1.6f;
		m_cosmeticRadius = 2.0f;
	}
}

void Asteroid::InitializeAsteroidVertices()
{
	m_asteroidInitialVertexlength = rng.RollRandomIntInRange(16, 20) / 10.f;
	float angleInDegreesBetweenAsteroidTriangles = 0.f;

	m_asteroidVerts[0] = Vertex_PCU((Vec3(0.f, 0.f, 0.f)), asteroidColor, Vec2(0.f, 0.f));
	m_asteroidVerts[1] = Vertex_PCU((Vec3(m_asteroidInitialVertexlength, 0.f, 0.f)), asteroidColor, Vec2(0.f, 0.f));
	angleInDegreesBetweenAsteroidTriangles = (360.f * 3.f) / (NUM_ASTEROID_VERTS);

	//-----------------------------------------------------------------------------------------------------------------
	float costheta = CosDegrees(angleInDegreesBetweenAsteroidTriangles);
	float intialXVertex = m_asteroidInitialVertexlength * costheta;
	
	float sintheha = SinDegrees(angleInDegreesBetweenAsteroidTriangles);
	float initialYVertex = m_asteroidInitialVertexlength * sintheha;

	m_asteroidVerts[2] = Vertex_PCU((Vec3(intialXVertex, initialYVertex, 0.f)),asteroidColor, Vec2(0.f, 0.f));
	
	//-----------------------------------------------------------------------------------------------------------------
	int asteroidVertIndex = 3;
	for (asteroidVertIndex = 3; asteroidVertIndex < NUM_ASTEROID_VERTS; asteroidVertIndex += 3)
	{
		angleInDegreesBetweenAsteroidTriangles = angleInDegreesBetweenAsteroidTriangles + ( (360.f * 3.f) / (NUM_ASTEROID_VERTS));
		m_asteroidVerts[asteroidVertIndex] = m_asteroidVerts[asteroidVertIndex - 3];
		m_asteroidVerts[asteroidVertIndex + 1] = m_asteroidVerts[asteroidVertIndex - 1];

		float newRandomLengthForTheThirdVertex = rng.RollRandomIntInRange(16, 20) / 10.f;
		
		m_asteroidVerts[asteroidVertIndex + 2].m_position = Vec3(newRandomLengthForTheThirdVertex * CosDegrees(angleInDegreesBetweenAsteroidTriangles),
			newRandomLengthForTheThirdVertex * SinDegrees(angleInDegreesBetweenAsteroidTriangles), 0.f);
		m_asteroidVerts[asteroidVertIndex + 2].m_color = asteroidColor;
		m_asteroidVerts[asteroidVertIndex + 2].m_uvTexCoords = Vec2(0.f, 0.f);
	}

	m_asteroidVerts[NUM_ASTEROID_VERTS - 1] = m_asteroidVerts[1];
}