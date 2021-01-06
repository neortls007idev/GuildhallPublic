#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "World.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/Player.hpp"

class Game
{

public:

	Game();
	void Update( float deltaSeconds );
	void Render() const;
	void UpdateCamera();
	void RenderUI() const;
	void CheckCollision();
	
	void AddScreenShakeIntensity( float deltaShakeIntensity );

	void GarbageCollection();
	void GarbageDeletion();
	void Die();

private:

	void UpdateFromKeyBoard();


private:

	int m_controllerID = -1;
	float m_screenShakeIntensity = 0.f;
	World* m_world1 = nullptr;
	bool m_isClipping = false;
	float m_tilesInViewHorizontally = 16.0f;
	float m_tilesInViewVertically = 9.0f;
	Vec2 m_CameraCenter= Vec2( m_tilesInViewHorizontally / 2 , m_tilesInViewVertically / 2 );

public:

	bool m_debugDraw = false;
	Texture* m_tankBaseTexture = nullptr;
	Texture* m_tankTurretTexture = nullptr;
	Camera m_worldCamera;
	Camera m_uiCamera;

};