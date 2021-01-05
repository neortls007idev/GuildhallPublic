#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class Asteroid :public Entity {

private:
	

public:
	Vertex_PCU m_asteroidVerts[48];
	Rgba8 asteroidColor = Rgba8(100, 100, 100, 255);
	float m_asteroidInitialVertexlength = 0.f;

public:

	Asteroid(Game* currentGameInstance);

	void InitializeMemberVariables();
	
	void InitializeAsteroidVertices();

	void Update(float deltaSeconds) override;
	void Render() const;
	void Die();

	void RollAsteroid(float deltaSeconds);

};
