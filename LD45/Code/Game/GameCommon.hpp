#pragma once
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Window;

//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern Game* g_theGame;
extern Window* g_theWindow;

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float CLIENT_ASPECT											= 2.f; // We are requesting a 2:1 aspect (square) window area

constexpr int NUM_STARTING_ASTEROIDS									= 6;
constexpr int MAX_ASTEROIDS												= 12;
constexpr int MAX_BULLETS												= 2000;
constexpr float WORLD_SIZE_X											= 600.f;
constexpr float WORLD_SIZE_Y											= 800.f;
constexpr float UI_SIZE_X												= 1600.f;
constexpr float UI_SIZE_Y												= 800.f;
constexpr float WORLD_CENTER_X											= WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y											= WORLD_SIZE_Y / 2.f;
constexpr float ASTEROID_SPEED											= 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS									= 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS								= 2.0f;
constexpr float BULLET_LIFETIME_SECONDS									= 2.0f;
constexpr float BULLET_SPEED											= 50.f;
constexpr float BULLET_PHYSICS_RADIUS									= 0.5f;
constexpr float BULLET_COSMETIC_RADIUS									= 2.0f;
constexpr float PLAYER_SHIP_ACCELERATION								= 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED									= 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS								= 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS								= 2.25f;

constexpr int NUM_ATTRACTMODE_VERTS										= 93;
constexpr int NUM_BULLET_VERTS											= 6;
constexpr int NUM_ASTEROID_VERTS										= 30;
constexpr int NUM_SHIP_VERTS											= 15;
constexpr int NUM_WASP_VERTS											= 39;
constexpr int NUM_BETTLE_VERTS											= 48;
constexpr int NUM_LINE_VERTS											= 6;

constexpr float BETTLE_SPEED											= 10.0f;
constexpr float WASP_ACCELERATION										= 10.0f;
constexpr int MAX_BETTLES												= 6;
constexpr int	MAX_WASPS												= 6;

constexpr int MAX_NUM_WAVES												= 5;

constexpr float MIN_CAMERA_SHAKE										= -10.0f;
constexpr float MAX_CAMERA_SHAKE										= 10.0f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND						= 0.05f;

constexpr int	MAX_DEBRIS = 500;
constexpr float DEBRIS_VELOCITY = 10.f;

//void DrawLine( const Vec2& start, const Vec2& end, const Rgba8& color, float thickness);
//void DrawRing( const Vec2& center, float radius, const Rgba8& color, float thickness);


