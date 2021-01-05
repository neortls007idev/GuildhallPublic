#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Game/Bullet.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Entity.hpp"


extern RenderContext* g_theRenderer;

Bullet::Bullet(Game* CurrentGameInstance, Vec2 pos, float orientation):
		Entity( CurrentGameInstance, 
				pos, 
				Vec2(0.f,0.f), 
				orientation),
	localVerts{ Vertex_PCU((Vec3(-2.f, 0.f, 0.f)),  Rgba8(255, 0, 0, 0),     Vec2(0.f, 0.f)), //tail
				Vertex_PCU((Vec3(0.f, -0.5f, 0.f)), Rgba8(255, 0, 0, 255),   Vec2(0.f, 0.f)),
				Vertex_PCU((Vec3(0.f, 0.5f, 0.f)),  Rgba8(255, 0, 0, 255),   Vec2(0.f, 0.f)),
				Vertex_PCU((Vec3(0.f, -0.5f, 0.f)), Rgba8(255, 0, 0, 255),   Vec2(0.f, 0.f)),
				Vertex_PCU((Vec3(0.5f, 0.f, 0.f)),  Rgba8(255, 225, 0, 255), Vec2(0.f, 0.f)),//nose
				Vertex_PCU((Vec3(0.f, 0.5f, 0.f)),  Rgba8(255, 0, 0, 255),   Vec2(0.f, 0.f)) }
{
	m_velocity = Vec2::MakeFromPolarDegrees(orientation, 50.f);

	m_isDead = false;
	m_isGarbage = false;
	m_physicsRadius = 0.5f;
	m_cosmeticRadius = 2.0f;
	m_health = 1;
}

void Bullet::Update(float deltaSeconds)
{
	
	Movement( deltaSeconds );
}

void Bullet::Render() const
{
	Vertex_PCU tempVerts[NUM_BULLET_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; vertIndex++)
	{
		tempVerts[vertIndex] = localVerts[vertIndex];
	}
	TransformVertexArray2D(NUM_BULLET_VERTS, tempVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_BULLET_VERTS, tempVerts);
}


void Bullet::Movement( float deltaSeconds )
{
	m_position += m_velocity * deltaSeconds;
}


void Bullet::Die()
{

}