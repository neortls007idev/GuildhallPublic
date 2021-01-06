#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Game/UISystemStructs.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Camera;
namespace std { class thread; }

//--------------------------------------------------------------------------------------------------------------------------------------------

class UISystem
{
public:
	UISystem();

	void		LoadUITextures();
	void		LoadMainMenuAnimTex();
	void		LoadUIFonts();
	void		InitalizeMainMenuLabels();
	void		InitalizeMainMenuButtons();
	void		InitalizeHUDLabels();
	void		InitalizeHighScoreData();
	void		InitalizeHighScoreLabels();
	void		InitalizeSettingsMenuLabels();
	void		InitializeBackButton();
	void		InitializeSliders();
	void		InitializeGameOverLabels();

	//--------------------------------------------------------------------------------------------------------------------------------------------

	~UISystem();

	void		Update( float deltaSeconds );
	bool		LoadingState();

	void		PlayRandomUIBackgroundMusic();

	//--------------------------------------------------------------------------------------------------------------------------------------------

	void		MainMenuState( float deltaSeconds );
	void		GameOverState( float deltaSeconds );
	void		GameOverInputState();
	void		GameOverHighScoreState( float deltaSeconds );
	void		PauseState();
	void		HighScoreMenuState();
	void		SettingsMenuState();
	void		UpdateBackGroundBranches();
	
	void		Render() const;
	void		RenderLoadingScreen() const;
	void		RenderMainMenuScreen() const;
	void		RenderSettingsMenuScreen() const;
	void		RenderHighScoreMenuScreen() const;
	void		RenderPauseMenuScreen() const;
	void		RenderHUD() const;
	void		RenderGameOver() const;
	void		RenderGameOverInput() const;
	void		RenderDebugMouse() const;

	void		EndFrame();

	void		SetGameState( const eUISTATE state )																			{ m_systemState = state; }
	eUISTATE	GetGameState()const																								{ return m_systemState; }
	void		ResetAnimTime()																									{ m_currentBGAnimFrame = 0.f; }
	Texture*	GetUITexture( eUITEX texType ) const																			{ return m_UITextures[ texType ]; }
	AABB2		GetUIButton( eUIButtons buttonType ) const																		{ return m_buttons[ buttonType ]; }
	AABB2		GetUILabel( eUILABLES labelType ) const																			{ return m_labels[ labelType ]; }
	AABB2		GetUIScreenSpace() const																						{ return m_screenSpace; }
	
public:
	bool										m_UIDebugDraw				= false;
	
private:
	eUISTATE									m_systemState				= INVALID_STATE;
	Camera*										m_UICamera					= nullptr;
	AABB2										m_screenSpace				= AABB2::ZERO_TO_ONE;
	bool										m_state[ NUM_UI_STATE ];
	Texture*									m_UITextures[ NUM_UI_TEX ];
	BitmapFont*									m_UIFonts[ NUM_UI_FONTS ];

	AABB2										m_labels[ NUM_UI_LABELS ];
	AABB2										m_buttons[ NUM_UI_BUTTONS ];
	UISlider*									m_settingsSliders[ NUM_SLIDERS ];
	
	float										m_BGAnimationDuration		= 2.1f;
	float										m_currentBGAnimFrame		= 0.f;

	std::vector< HighscoreData >				m_highScores;

	SoundPlaybackID								m_currentBackgroundsound	= 0;
	float										m_backgroundMusicVol		= 0.11f;

	float										m_gameOverHighScoreTimer	= 0.f;
	float										m_gameOverTimer				= 0.f;

	std::thread*								m_loadMainMenuTex;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
