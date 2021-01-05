#pragma once
#include "Game/Entity.hpp"

class Debris : public Entity
{

private:

public:
	Debris(Game* currentGameInstance, Vec2 pos, Vec2 velocity, float orientation, Rgba8 color);
	~Debris() {};

	void Update(float deltaSeconds) /*override*/;
	void Render() const /*override*/;
	void Die()	  /*override*/;

	void SpawnPosition(Vec2 pos);

public:
private:
	Rgba8 m_debrisColor = Rgba8(255, 0, 0);
	const Vertex_PCU m_debrisVerts[3] = 
	{
		Vertex_PCU(Vec3(1.f,0.f,0.f),  m_debrisColor,Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(-1.f,1.f,0.f), m_debrisColor,Vec2(0.f,0.f)),
		Vertex_PCU(Vec3(-1.f,-1.f,0.f),m_debrisColor,Vec2(0.f,0.f)),
	};
};
