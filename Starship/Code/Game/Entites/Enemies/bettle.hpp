#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"

class Bettle :public Entity {

private:

public:

	Bettle(Game* currentGameInstance, Vec2 pos, Vec2 velocity, float orientation);

	void InitializeMemberVariables();
	
	void Update(float deltaSeconds) override;
	void Render() const;
	void Die();

public:


private:
	Vertex_PCU m_localBettleVerts[NUM_BETTLE_VERTS] = {
						Vertex_PCU( ( Vec3( 1.5f, -2.f, 0.f))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3( 1.5f, 2.f, 0.f))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-1.5f, 2.f, 0.f))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(-1.5f, 2.f, 0.f ))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-1.5f, -2.f, 0.f))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3( 1.5f, -2.f, 0.f))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
						//--------- - -------------------------------------------------------------------------------------------------------
						Vertex_PCU( ( Vec3( 0.f, -0.5f, 0.f))  , Rgba8(123, 173, 44, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3( 3.5f, -0.5f, 0.f)) , Rgba8(123, 173, 44, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3( 3.5f, 1.f, 0.f))	, Rgba8(123, 173, 44, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(0.f, 0.5f, 0.f))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(3.5f, 1.f, 0.f))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.f, -0.5f, 0.f))	, Rgba8(102, 135, 14, 255), Vec2(0.f, 0.f)),
						//--------- - -------------------------------------------------------------------------------------------------------
								      
						Vertex_PCU( ( Vec3(-1.f, 2.f, 0.f))	, Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-0.5f, 2.75f, 0.f)), Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-0.5f, 3.5f, 0.f))	, Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(-0.5f, 2.75f, 0.f)), Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-0.5f, 3.5f, 0.f))	, Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.0f, 2.f, 0.f))	, Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(0.f, 2.f, 0.f))	, Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.5f, 2.75f, 0.f))	, Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.5f, 3.5f, 0.f))	, Rgba8(179, 200, 38, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(0.5f, 2.75f, 0.f))	, Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.5f, 3.5f, 0.f))	, Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(1.0f, 2.f, 0.f))	, Rgba8(198, 223, 31, 255), Vec2(0.f, 0.f)),
						//--------- - -------------------------------------------------------------------------------------------------------
								      
						Vertex_PCU( ( Vec3(-1.f, -2.f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-1.f, -3.f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-0.5f, -2.5f, 0.f)), Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(-1.f, -2.f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.f, -2.f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-0.5f, -2.5f, 0.f)), Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(0.0f, -2.f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.0f, -3.f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-1.0f, -2.5f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(0.0f, -3.f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-1.0f, -3.f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(-0.5f, -2.5f, 0.f)), Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
								      
						//--------- - -------------------------------------------------------------------------------------------------------
								      
						Vertex_PCU( ( Vec3(0.5f, -2.f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.5f, -3.f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(1.0f, -2.5f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(0.5f, -2.f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(1.5f, -2.f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(1.0f, -2.5f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(1.5f, -2.f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(1.5f, -3.f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(1.0f, -2.5f, 0.f))	, Rgba8(184, 21, 48, 255), Vec2(0.f, 0.f)),
								      
						Vertex_PCU( ( Vec3(1.5f, -3.f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(0.5f, -3.f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f)),
						Vertex_PCU( ( Vec3(1.0f, -2.5f, 0.f))	, Rgba8(127, 12, 31, 255), Vec2(0.f, 0.f))
	};

};
