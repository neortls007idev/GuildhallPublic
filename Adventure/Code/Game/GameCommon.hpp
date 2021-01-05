#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Window;

//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern Game*		g_theGame;
extern Window*		g_theWindow;

//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//  GENERAL GAME CONSTANTS
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float CLIENT_ASPECT											=  16.f / 9.f; // We are requesting a 2:1 aspect (square) window area
constexpr float UI_SIZE_X												=  1600.f;
constexpr float UI_SIZE_Y												=  900.f;
constexpr float MIN_CAMERA_SHAKE										= -2.5f;
constexpr float MAX_CAMERA_SHAKE										=  2.5f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND						=  0.05f;

constexpr int   MIN_WORM_LENGTH											= 3;
constexpr int   MAX_WORM_LENGTH											= 13;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum TextureType
{
	TEXTURE_NONE = -1 ,
	TEXTURE_TERRAIN_32x32 ,
	TEXTURE_CHARACTERS,
	
	NUM_TOTAL_TEXTURES
};

//--------------------------------------------------------------------------------------------------------------------------------------------