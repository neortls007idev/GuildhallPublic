#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/World.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------


class Game
{

public:

	Game();

	void LoadAssets();
	void LoadAllSounds();
	void LoadAllEntityTextures();

	virtual void Startup();
	virtual void Update( float deltaSeconds );
	virtual void LateUpdate();
	void UpdateCamera();

	void UpdateGameplayFromXbox();
		
	virtual void Render() const;

	void RenderGameplay() const;

	void AddScreenShakeIntensity( float deltaShakeIntensity );

	void Die();

//--------------------------------------------------------------------------------------------------------------------------------------------

private:

//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	int				m_controllerID							= -1;
	float			m_screenShakeIntensity					= 0.f;
	float			m_tilesInViewHorizontally				= 48.0f;
	float			m_tilesInViewVertically					= 27.0f;
	float			m_victoryPanelFadeTimer					= 0.5f;
	Vec2			m_CameraCenter							= Vec2( m_tilesInViewHorizontally / 2 , m_tilesInViewVertically / 2 );

//--------------------------------------------------------------------------------------------------------------------------------------------

public:
	World*			 m_world								= nullptr;
	Texture*		 m_textures[ NUM_TOTAL_TEXTURES ];
	SoundID			 m_sounds[ NUM_TOTAL_SOUNDS ];
};

//--------------------------------------------------------------------------------------------------------------------------------------------