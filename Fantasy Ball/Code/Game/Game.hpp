#pragma once
#include <thread>

#include "PowerUps.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Camera.hpp"

class ParticleEmitter2D;
class SpriteSheet;
class Shader;
class LoadAudioJob;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{
public:
	Game();
	~Game();
	
	void			LoadAssets();
	void			LoadAllSounds();
	void			LoadAllTextures();
	void			LoadAllShaders();

	void			PostGameConstructDataOnce();
	void			PostGameConstruct();

	void			PlayRandomGameBackgroundMusic();
	void			StopGameBackgroundMusic();

	//--------------------------------------------------------------------------------------------------------------------------------------------

	void			Update( float deltaSeconds );
	void			UpdateMoveToNextMap();
	void			UpdateCamera();
	void			AddScreenShakeIntensity( float deltaShakeIntensity = 1.f );
	
	void			Render() const;
	void			GarbageCollection();
	void			EndFrame();

	SoundPlaybackID GetSFX( eGameAudioFX SFXid ) const;
	
	//----------------------------------------------------------------------------------------------------------
	int				GetPaddleHealth() const														{ return m_playerHealth; }
	Camera*			GetWorldCamera()															{ return &m_worldCamera; }
private:
	void			UpdateFromKeyBoard();

	//--------------------------------------------------------------------------------------------------------------------------------------------

private:
	int							m_controllerID									= -1;
	float						m_screenShakeIntensity							= 0.f;
	float						m_cameraHalfHeight								= 0.f;
	float						m_cameraAspectRatio								= 0.f;
	Rgba8						m_fadeColor;
	bool						m_hasLoadingStateStarted						= true;

public:
	bool						m_gameJustStarted								= false;
	bool						m_gameOverAudio									= false;
								
	Camera						m_worldCamera;
	Camera						m_uiCamera;
	int							m_currentLevelNumber							= 0;
	std::vector<Map*>			m_levels;
	Map*						m_currentLevel									= nullptr;
	Texture*					m_gameTex[ NUM_GAME_TEX ];
	SpriteSheet*				m_gameSS[ NUM_GAME_SS ];
	SoundPlaybackID				m_sounds[ NUM_GAME_SFX ];
	SoundPlaybackID				m_currentBackgroundsound						= 0;

	float						m_backgroundMusicVol							= 0.11f;
	float						m_sfxVol										= 0.1f;
	float						m_paddleSensitivity								= 1.f;
	
	Shader*						m_gameShader[ NUM_GAME_SHADERS ];			
	
	int							m_playerHealth									= 3;
	ePowerUpType				m_currentPowerUps								= PT_INVALID;
	
	bool						m_isBallLaunchable								= true;
	bool						m_isGameDirty									= false;

	IntVec2						m_ballDefaultSpriteCoords						= IntVec2::ZERO;
	float						m_highScore										= 0;
	std::string					m_playerName									= "";
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			DEBUG CODE
	//--------------------------------------------------------------------------------------------------------------------------------------------

	bool						m_isDebugDraw									= false;
	
	std::thread*				m_loadAudio										= nullptr;
	std::thread*				m_loadTextures;

};

//--------------------------------------------------------------------------------------------------------------------------------------------