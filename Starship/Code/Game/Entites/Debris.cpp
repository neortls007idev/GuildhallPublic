#include "Game/Entites/Debris.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"


extern RenderContext* g_theRenderer;

Debris::Debris(Game* currentGameInstance, Vec2 pos, Vec2 velocity, float orientation, Rgba8 color) :
		Entity(currentGameInstance,
				pos,
				velocity,
				orientation)
{
	m_age = 2.0f;
	m_debrisColor = color;
	m_debrisColor.a = 127;
	m_position = pos;
	m_orientationDegrees = orientation;
	m_isGarbage = false;
}

void Debris::Update(float deltaSeconds) 
{
	m_age -= deltaSeconds;
	float deltaAlpha = RangeMapFloat(0.f, 2.0f, 0, 127, m_age);
	m_debrisColor.a = (unsigned char)deltaAlpha;
	if(m_age>0)
	m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, 20.f);
	Entity::Movement(deltaSeconds);
	if ( m_age <= 0.f)
	{
	
		Die();
	}
}

void Debris::Render() const 
{
	if (m_age>0)
	{
		Vertex_PCU tempVerts[3];
		for (int vertIndex = 0; vertIndex < 3; vertIndex++)
		{
			tempVerts[vertIndex] = m_debrisVerts[vertIndex];
			tempVerts[vertIndex].m_color = m_debrisColor;

		}
		TransformVertexArray2D(3,tempVerts,0.5f,m_orientationDegrees,m_position);
		g_theRenderer->DrawVertexArray(3, tempVerts);
	}
}

void Debris::Die() 
{
	m_isGarbage = true;
}

void Debris::SpawnPosition(Vec2 pos)
{
	m_position = pos;
}

