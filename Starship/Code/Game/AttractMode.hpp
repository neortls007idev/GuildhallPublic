#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class AttractMode
{
public:

	AttractMode();

	void InitializeAttractModeVertices();
	void BeginFrame();
	void Update(float deltaSeconds);
	void Render() const;
	void Die();
	void EndFrame();

private:
	bool m_attractModeStatus = true;
	Vec2 m_position = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
	float m_orientationDegrees = 0.f;
	int m_controllerID = -1;

	Rgba8 attractMode = Rgba8(0, 0, 0, 255);

	Vertex_PCU m_localAttractModeVerts[NUM_ATTRACTMODE_VERTS] = {

		//-----------------------------------------------------------------------------------------------------------------
		// Character S : 15 vertices
		//-----------------------------------------------------------------------------------------------------------------

			Vertex_PCU((Vec3(-70.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-67.f, 35.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-80.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-80.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-85.f, 15.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-80.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-70.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-70.f, 23.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-80.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-70.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-70.f, 10.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-65.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-70.f, 10.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-65.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-80.f, 10.f, 0.f)), attractMode, Vec2(0.f, 0.f)),

			//-----------------------------------------------------------------------------------------------------------------
			// Character T : 6 vertices
			//-----------------------------------------------------------------------------------------------------------------

			Vertex_PCU((Vec3(-65.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-50.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-47.f, 35.f, 0.f)), attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-61.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-54.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-56.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			//-----------------------------------------------------------------------------------------------------------------
			// Character A : 9 vertices
			//-----------------------------------------------------------------------------------------------------------------

			Vertex_PCU((Vec3(-35.f, 35.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-40.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-45.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-35.f, 35.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-25.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-30.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-48.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-22.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-35.f, 13.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			//-----------------------------------------------------------------------------------------------------------------
			// Character R : 18 vertices
			//-----------------------------------------------------------------------------------------------------------------

			Vertex_PCU((Vec3(-20.f, 27.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-15.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-15.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-15.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-7.f, 30.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-10.f, 35.f, 0.f)), attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-7.f, 30.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-2.f, 33.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-7.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-7.f, 20.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-15.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-7.f, 18.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-15.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-18.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-12.f, 7.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(-15.f, 20.f, 0.f)), attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-7.f, 7.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(-2.f, 7.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			//-----------------------------------------------------------------------------------------------------------------
			// Character S : 15 Vertices 
			//-----------------------------------------------------------------------------------------------------------------

			Vertex_PCU((Vec3(18.f, 30.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(21.f, 35.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(8.f, 30.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(8.f, 30.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(3.f, 15.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(8.f, 20.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(18.f, 20.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(18.f, 23.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(8.f, 20.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(18.f, 20.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(18.f, 10.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(23.f, 7.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(18.f, 10.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(23.f, 7.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(8.f, 10.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			//-----------------------------------------------------------------------------------------------------------------
			// Character H : 12 Vertices
			//-----------------------------------------------------------------------------------------------------------------
			Vertex_PCU((Vec3(25.f, 31.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(32.f, 35.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(30.f, 7.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(22.f, 20.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(48.f, 20.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(35.f, 25.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(22.f, 20.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(48.f, 20.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(35.f, 15.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(38.f, 35.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(45.f, 31.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(40.f, 7.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			//-----------------------------------------------------------------------------------------------------------------
			// character I : 9 Vertices
			//-----------------------------------------------------------------------------------------------------------------

			Vertex_PCU((Vec3(48.f, 30.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(64.f, 31.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(61.f, 35.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(52.f, 31.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(60.f, 31.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(56.f, 10.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(48.f, 10.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(64.f, 10.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(51.f, 7.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			//-----------------------------------------------------------------------------------------------------------------
			// character P : 9 vertices
			//-----------------------------------------------------------------------------------------------------------------

			Vertex_PCU((Vec3(68.f, 31.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(72.f, 35.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(70.f, 7.f, 0.f))  , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(72.f, 35.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(70.f, 31.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(80.f, 25.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),

			Vertex_PCU((Vec3(80.f, 25.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(77.f, 28.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
			Vertex_PCU((Vec3(65.f, 15.f, 0.f)) , attractMode, Vec2(0.f, 0.f)),
	};
};

//--------------------------------------------------------------------------------------------------------------------------------------------