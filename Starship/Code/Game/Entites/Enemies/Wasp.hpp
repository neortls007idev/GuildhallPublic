#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"

class Wasp :public Entity {

private:
	Vec2 m_acceleration = Vec2( 0.f, 0.f );
	
public:

	Wasp(Game* currentGameInstance, Vec2 pos, Vec2 velocity, float orientation);

	void InitializeMemberVariables();
	
	void Update(float deltaSeconds) override;
	void Render() const;
	void Die();
	
public:
	
	Vertex_PCU m_localWaspVerts[NUM_WASP_VERTS] = 
	{
					Vertex_PCU((Vec3(3.f, 0.f, 0.f)), Rgba8(246, 193, 32, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(2.5f, 1.f, 0.f)), Rgba8(246, 193, 32, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(2.f, 0.f, 0.f)), Rgba8(246, 193, 32, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(3.f, 0.f, 0.f)), Rgba8(216, 31, 26, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(3.5f, 0.f, 0.f)), Rgba8(216, 31, 26, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(2.5f, 1.f, 0.f)), Rgba8(216, 31, 26, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(3.f, 0.f, 0.f)), Rgba8(246, 193, 32, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(2.5f, -1.f, 0.f)), Rgba8(246, 193, 32, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(2.f, 0.f, 0.f)), Rgba8(246, 193, 32, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(3.f, 0.f, 0.f)), Rgba8(216, 31, 26, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(3.5f, 0.f, 0.f)), Rgba8(216, 31, 26, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(2.5f, -1.f, 0.f)), Rgba8(216, 31, 26, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(2.f, 1.f, 0.f)), Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(2.f, -1.f, 0.f)), Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.f, -1.f, 0.f)), Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(2.f, 1.f, 0.f)), Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.f, 1.f, 0.f)), Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.f, -1.f, 0.f)), Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(-1.f, 1.f, 0.f)), Rgba8(29, 86, 232, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-2.0f, 1.5f, 0.f)), Rgba8(29, 86, 232, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.f, .25f, 0.f)), Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(-1.f, -1.f, 0.f)), Rgba8(29, 86, 232, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-2.0f, -1.5f, 0.f)), Rgba8(29, 86, 232, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.f, -.25f, 0.f)), Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(-1.f, .5f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-2.0f, .125f, 0.f)), Rgba8(29, 86, 232, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.f, -.5f, 0.f)), Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(2.f, 1.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(0.f, 2.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(1.f, 1.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(2.f, -1.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(0.f, -2.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(1.f, -1.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(0.f, 1.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.5f, 2.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.f, 1.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),

					Vertex_PCU((Vec3(0.f, -1.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.5f, -2.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
					Vertex_PCU((Vec3(-1.f, -1.f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f))
	};

};
