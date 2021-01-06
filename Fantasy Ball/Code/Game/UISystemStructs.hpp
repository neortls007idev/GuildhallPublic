#pragma once
#include <string>

#include "Engine/Primitives/AABB2.hpp"

class Texture;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eUIFONTS
{
	UI_FONT_WOOD1 ,
	UI_FONT_TITLE ,

	NUM_UI_FONTS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eUITEX
{
	UI_BACKGROUND ,
	UI_BACKGROUND_BORDER ,
	UI_BACKGROUNDANIM ,
	UI_WOODBARK_T1 ,
	UI_TITLE ,
	UI_PLAY ,
	UI_HIGHSCORES ,
	UI_SETTINGS ,
	UI_EXIT ,
	UI_MM_BRANCH_TEX ,
	UI_MM_BRANCHT2 ,
	UI_MM_BRANCHT3 ,
	UI_MM_BRANCHT4 ,
	UI_MM_BRANCHT5 ,
	UI_MM_BRANCHT6 ,

	UI_HS_TITLE ,
	UI_HS_HBORDER ,
	UI_HS_VBORDER ,
	UI_HS_NAME_HEADER ,
	UI_HS_SCORE_HEADER ,

	HUD_HEALTH ,

	SETTINGS_TITLE ,
	SETTINGS_SFX_VOL ,
	SETTINGS_BGM_VOL ,
	SETTINGS_PADDLE_SENSE ,

	PAUSE_TITLE ,
	MM_BUTTON,

	GAME_OVER_TITLE,
	GAME_OVER_MESSAGE,
	GAME_OVER_INPUTBOX,
	
	GEN_BACK_BTN ,
	GEN_RESUME_BTN ,
	GEN_OK_BTN ,
		
	GEN_SLIDER_BASE ,
	GEN_SLIDER_FILLBAR ,
	GEN_SLIDER_BORDER ,
	GEN_SLIDER_BUTTON ,

	NUM_UI_TEX
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eUILABLES
{
	UI_TITLEBOX ,
	UI_LOADINGBOX ,

	UI_MM_BRANCH1 ,
	UI_MM_BRANCH2 ,
	UI_MM_BRANCH3 ,
	UI_MM_BRANCH4 ,
	UI_MM_BRANCH5 ,
	UI_MM_BRANCH6 ,

	MM_PLAY_BUTTON ,
	MM_HIGHSCORE_BUTTON ,
	MM_SETTINGS_BUTTON ,
	MM_EXIT_BUTTON ,

	HS_TITLE ,
	HS_TOPBORDER ,
	HS_BOTTOMBORDER ,
	HS_THEADER_NAME ,
	HS_THEADER_SCORE ,
	HS_ENTRY_NAME ,
	HS_ENTRY_SCORE ,
	
	HUD_HEALTHBOX ,

	SETT_MENU_TITLE ,
	SETT_MENU_SFX_VOL ,
	SETT_MENU_BGM_VOL ,
	SETT_MENU_PADDLE_SENSE ,
	
	UI_BACK_BUTTON ,

	UI_SLIDER ,
	UI_SLIDER_FILLBAR ,
	UI_SLIDER_BUTTON ,

	GO_MESSAGEBOX ,
	GO_INPUTBOX ,
	GO_OKBUTTON , 
	
	NUM_UI_LABELS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eUISTATE
{
	INVALID_STATE = -1 ,
	LOADING_STATE ,
	MAIN_MENU_STATE ,
	HUD_STATE ,
	PAUSE_STATE ,
	PAUSE_STATE_SETTINGS ,
	PAUSE_STATE_HIGHSCORE , 
	GAME_OVER_STATE ,
	GAME_OVER_INPUT ,
	GAME_OVER_HIGHSCORE ,
	HIGHSCORE_MENU ,
	SETTINGS_MENU ,
	TUTORIAL_MENU ,

	NUM_UI_STATE
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eUIButtons
{
	PLAY_BUTTON ,
	HIGHSCORE_BUTTON ,
	SETTINGS_BUTTON ,
	EXIT_BUTTON ,
	BACK_BUTTON ,

	SLIDER_SFX_BUTTON ,
	SLIDER_MUSIC_BUTTON ,
	SLIDER_PADDLE_SPEED_BUTTON ,
	
	NUM_UI_BUTTONS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eSettingsSliders
{
	SFX_SLIDER,
	BGM_SLIDER,
	CURSOR_SENSITIVITY_SLIDER,

	NUM_SLIDERS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct HighscoreData
{
	HighscoreData( std::string nameVal, size_t scoreVal )
	{
		name = nameVal;
		score = scoreVal;
	}
	
	std::string name;
	size_t		score;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct UISlider
{
public:
	UISlider( AABB2	cosmeticBase , AABB2 cosmeticFillBar , AABB2 cosmeticButton , Vec2 centerPos , float minValue , float maxValue , float value );
	void	Update();
	void	Render();
	float	GetValue() const																								{ return m_value;  }
	void	SetValue( const float value );
	
public:
	AABB2		m_cosmeticBase;
	AABB2		m_cosmeticFillBar;
	AABB2		m_cosmeticButton;
	AABB2		m_physicalButton;
	Vec2		m_centerPos;
	Vec2		m_buttonMovementRange;
	
	float		m_minRange;
	float		m_maxRange;
	float		m_value;
	
	//static	Texture*	m_Base;
	//static	Texture*	m_Border;
	//static	Texture*	m_fillBar;
	//static	Texture*	m_button;
};

//--------------------------------------------------------------------------------------------------------------------------------------------