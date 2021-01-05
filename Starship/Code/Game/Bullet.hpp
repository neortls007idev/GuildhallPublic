#pragma once

#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"



class Bullet:public Entity{

public:
	explicit Bullet(Game* CurrentGameInstance, Vec2 pos, float orientation);
	void Update(float deltaSeconds);
	void Render() const;
	void Die();
	void Movement( float deltaSeconds );

private:
	
	const Vertex_PCU localVerts[6];

};