#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "World.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"


class Game 
{

private:

	int m_controllerID = -1;
	float m_screenShakeIntensity = 0.f;
	World* m_world1 = nullptr;
	AABB2 m_aabbtest = AABB2( 10.f , 10.f , 30.f , 30.f );
	Rgba8 m_testingColor = Rgba8( 255 , 255 , 255 , 255 );
	Texture* m_tankBaseTexture = nullptr;

public:

	Texture* m_Waldo = nullptr;
	Camera m_worldCamera;
	Camera m_uiCamera;

public:
	Game();

	

	void Update(float deltaSeconds);
	void Render() const;
	void UpdateCamera();
	void RenderUI() const;

	void KeyboardKeyPressEvents();

	void AddScreenShakeIntensity( float deltaShakeIntensity );
		
	void GarbageCollection();
	void GarbageDeletion();

	void Die();

};