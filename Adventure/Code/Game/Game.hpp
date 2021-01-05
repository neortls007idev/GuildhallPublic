#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World.hpp"
//#include "Game/Player.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{
public:
	Game();

	void LoadAssets();
	void LoadAllSounds();
	void LoadAllTextures();

	void Update( float deltaSeconds );
	void UpdateCamera();
	void InspectOnMouseOver() const;
	bool LoadingState();
	bool AttractState();
	void GameHasJustStarted();

	void UpdateGameplayFromXbox();
	void XboxUpdateOnVictory( float& victoryTimer );

	bool CheckIfPlayerLost( float deltaSeconds );
	
	void Render() const;

	void RenderVictory() const;

	void RenderLoadingScreen() const;
	void RenderAttractScreen() const;
	void RenderGameplay() const;
	void RenderUI() const;
	void RenderUIVerts() const;

	void AddScreenShakeIntensity( float deltaShakeIntensity );

	void Die();

private:
	void UpdateFromKeyBoard();

//--------------------------------------------------------------------------------------------------------------------------------------------

private:
	int m_controllerID = -1;
	float m_screenShakeIntensity = 0.f;
	float m_tilesInViewHorizontally = 16.0f;
	float m_tilesInViewVertically = 9.0f;
	float m_victoryPanelFadeTimer = 0.5f;
	Rgba8 m_fadeColor;
	bool  m_hasLoadingStateStarted = true;
	Vec2 m_CameraCenter= Vec2( m_tilesInViewHorizontally / 2 , m_tilesInViewVertically / 2 );

public:
	bool					  m_isClipping	   = false;
	bool					  m_debugDraw	   = false;
	bool					  m_gameJustStarted = false;
	bool					  m_gameOverAudio   = false;
							  
	World*					  m_world1		   = nullptr;
	Camera					  m_worldCamera;
	Camera					  m_uiCamera;

	Texture* m_textures[ NUM_TOTAL_TEXTURES ];	
};
//--------------------------------------------------------------------------------------------------------------------------------------------

