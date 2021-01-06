#pragma once
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Window;

//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern Game*		g_theGame;
extern Window*		g_theWindow;

//--------------------------------------------------------------------------------------------------------------------------------------------
//									GENERAL GAME CONSTANTS
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float CLIENT_ASPECT											=  16.f / 9.f; // We are requesting a 2:1 aspect (square) window area
constexpr float UI_SIZE_X												=  1600.f;
constexpr float UI_SIZE_Y												=  900.f;
constexpr float MIN_CAMERA_SHAKE										= -2.5f;
constexpr float MAX_CAMERA_SHAKE										=  2.5f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND						=  0.05f;
constexpr float PERCENTAGE_OF_STONE_TILES_IN_MAP						=  0.3f;
constexpr float PERCENTAGE_OF_MUD_TILES_IN_MAP							=  0.07f;
constexpr int	MAX_NUMBER_OF_STONE_WORMS								=   30;
constexpr int	MAX_NUMBER_OF_MUD_WORMS									=	30;
constexpr int	MAX_NUMBER_OF_LAVA_WORMS								=   30;
constexpr int	MAX_NUMBER_OF_WATER_WORMS								=   30;
constexpr int   MIN_WORM_LENGTH											=   3;
constexpr int   MAX_WORM_LENGTH											=   13;

constexpr int	PLAYER_HEALTH_PER_LIVES									=    2;
constexpr int	TOTAL_PLAYER_LIVES										=    5;

//--------------------------------------------------------------------------------------------------------------------------------------------
// INITIAL MAP AND WORLD CONSTANTS
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr int TOTAL_MAPS_IN_THE_WORLD									= 1;
constexpr int TOTAL_WORLDS_IN_THE_GAME									= 1;
constexpr int TOTAL_MAPS_IN_THE_GAME									= TOTAL_MAPS_IN_THE_WORLD * TOTAL_WORLDS_IN_THE_GAME;

constexpr float INTITAL_MAP_SIZE_X										= 48.f;
constexpr float INTITAL_MAP_SIZE_Y										= 27.f;
constexpr int	INTITAL_MAP_MAX_NUM_NPC_TURRETS							=  5;
constexpr int	INTITAL_MAP_MAX_NUM_NPC_TANKS							=  6;
constexpr int	INTITAL_MAP_MAX_NUM_BOULDERS							=  5;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum class GameStates
{
	GAME_STATE_LOADING ,
	GAME_STATE_ATTRACT ,
	GAME_STATE_PLAYING ,
	GAME_STATE_VICTORY ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum TextureType
{
	TEXTURE_NONE = -1 ,
	TEXTURE_PLAYER_TANK ,
	TEXTURE_PLAYER_TURRET ,
	TEXTURE_ENEMY_TANK1 ,
	TEXTURE_ENEMY_TANK2 ,
	TEXTURE_ENEMY_TANK3 ,
	TEXTURE_ENEMY_TANK4 ,
	TEXTURE_FRIENDLY_TANK1 ,
	TEXTURE_FRIENDLY_TANK2 ,
	TEXTURE_FRIENDLY_TANK3 ,
	TEXTURE_FRIENDLY_TANK4 ,
	TEXTURE_ENEMY_TURRET_BASE ,
	TEXTURE_ENEMY_TURRET ,
	TEXTURE_FRIENDLY_TURRET_BASE ,
	TEXTURE_FRIENDLY_TURRET ,
	TEXTURE_NEMESIS_TANK ,
	TEXTURE_NEMESIS_TURRET ,
	TEXTURE_BULLET ,
	TEXTURE_EXPLOSION ,
	TEXTURE_TILE_TERRAIN_8x8 ,
	TEXTURE_TILE_BOULDER ,

	NUM_TOTAL_TEXTURES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum AudioType
{
	SOUND_NONE = -1 ,
	SOUND_GAME_START ,
	SOUND_GAME_OVER ,
	SOUND_GAME_WON ,
	SOUND_MAP_CHANGE ,
	SOUND_PLAYER_SHOOT ,
	SOUND_ENEMY_SHOOT ,
	SOUND_ENEMY_SPOTTED ,
	SOUND_BOULDER_MOVED ,
	SOUND_PLAYER_DAMAGED ,
	SOUND_ENEMY_DAMAGED ,
	SOUND_PLAYER_DIED ,
	SOUND_ENEMY_DIED ,

	SOUND_ATTRACT_MUSIC,
	SOUND_GAME_PAUSED,
	SOUND_GAME_UNPAUSED ,

	NUM_TOTAL_SOUNDS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum GameType
{
	SINGLE_PLAYER ,
	MULTIPLAYER ,
};