#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"

class Game;
extern InputSystem* g_theInput;
extern Game* g_theGame;

constexpr float CLIENT_ASPECT											= 16.f/9.f; // We are requesting a 2:1 aspect (square) window area

constexpr float WORLD_SIZE_X											= 20.f;
constexpr float WORLD_SIZE_Y											= 30.f;
constexpr float WORLD_CAMERA_SIZE_X										= /*16.f;*/ WORLD_SIZE_Y*CLIENT_ASPECT;
constexpr float WORLD_CAMERA_SIZE_Y										= /*9.f;*/WORLD_SIZE_Y;
constexpr float UI_SIZE_X												= 1600.f;
constexpr float UI_SIZE_Y												= 800.f;
constexpr float WORLD_CENTER_X											= WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y											= WORLD_SIZE_Y / 2.f;

constexpr float MIN_CAMERA_SHAKE										= -10.0f;
constexpr float MAX_CAMERA_SHAKE										= 10.0f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND						= 0.05f;

constexpr float PERCENTAGE_OF_OBSTACLES_IN_MAP							= 0.3f;


//void DrawLine( const Vec2& start, const Vec2& end, const Rgba8& color, float thickness);
//void DrawRing( const Vec2& center, float radius, const Rgba8& color, float thickness);


