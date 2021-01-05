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
//  GENERAL GAME CONSTANTS
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float CLIENT_ASPECT											=  16.f / 8.f; // We are requesting a 2:1 aspect (square) window area
constexpr float UI_SIZE_X												=  1600.f;
constexpr float UI_SIZE_Y												=  900.f;
constexpr float MIN_CAMERA_SHAKE										= -10.0f;
constexpr float MAX_CAMERA_SHAKE										=  10.0f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND						=  0.05f;
constexpr float PERCENTAGE_OF_STONE_TILES_IN_MAP						=  0.3f;
constexpr float PERCENTAGE_OF_MUD_TILES_IN_MAP							=  0.07f;
constexpr int	MAX_NUMBER_OF_WORMS										=   12;
constexpr int   MIN_WORM_LENGTH											=   3;
constexpr int   MAX_WORM_LENGTH											=   8;

//--------------------------------------------------------------------------------------------------------------------------------------------
// INITIAL MAP CONSTANTS
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float INTITAL_MAP_SIZE_X												= 20.f;
constexpr float INTITAL_MAP_SIZE_Y												= 30.f;
constexpr int	INTITAL_MAP_MAX_NUM_NPC_TURRETS									=  5;
constexpr int	INTITAL_MAP_MAX_NUM_NPC_TANKS									=  6;
constexpr int	INTITAL_MAP_MAX_NUM_BOULDERS									=  5;

//--------------------------------------------------------------------------------------------------------------------------------------------
