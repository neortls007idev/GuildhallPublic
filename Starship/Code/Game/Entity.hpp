#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class Game;

class Entity {

public:
	
	Vec2 m_position;
	Vec2 m_velocity = Vec2(0.f,0.f);

	float m_orientationDegrees = 0.f;
	float m_angularVelocity = 0.f;
	int m_health = 1;
	bool m_isDead = true;
	bool m_isGarbage = true;

	float m_physicsRadius = 0.f;
	float m_cosmeticRadius = 0.f;

	float m_age = 0.f;

	const Rgba8 m_cosmeticRing = Rgba8(255, 0, 255, 255);
	const Rgba8 m_physicsRing  = Rgba8(0, 255, 255, 255);


	Game* m_game;
	
public:
	Entity(Game* pointerToGameInstance, Vec2 pos, Vec2 velocity, float orientation);
	virtual void Update(float deltaSeconds);

	void Movement(float deltaSeconds);
	void WrapAroundScreen();

	virtual void Render() const;

	virtual void IsDead();

	bool IsAlive();
	
};