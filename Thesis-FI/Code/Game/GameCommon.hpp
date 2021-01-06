#pragma once
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Window;
class ParticleEmitter3D;

//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern Game*		g_theGame;
extern Window*		g_theWindow;

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float		CLIENT_ASPECT											= 16.f/9.f;

constexpr float		GAME_CAM_NEAR_Z											= 0.01f;
constexpr float		GAME_CAM_FAR_Z											= 1000.0f;
constexpr float		GAME_CAM_FOV											= 60.0f;
constexpr size_t	FRAME_RATE_BUFFER_SIZE									= 9000;
constexpr size_t	PARTICLE_EMITTER_LIMIT									= 25000;
//constexpr size_t	PARTICLE_EMITTER_LIMIT									= 100;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum LitShaderTypes
{
	LIT_SHADOW ,
	LIT ,
	UNLIT ,
	UV ,
	NORMAL ,
	TANGENT ,
	BITANGENT ,
	SURFACE_NORMAL ,
	DISSOLVE ,
	FRESNEL ,
	TRIPLANAR_UNLIT ,
	TRIPLANAR_LIT ,

	TOTAL_LITSHADERS ,
};

enum ToneMap
{
	NO_TONE ,
	GRAYSCALE ,
	SEPHIA ,

	TOTAL_TONEMAPS ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  LightType
{
	POINT_LIGHT ,
	DIRECTIONAL_LIGHT ,
	SPOT_LIGHT ,

	TOTAL_LIGHT_TYPES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct fresnelData_t
{
	Vec3	fresnelColor = Vec3::UNIT_VECTOR_ALONG_J_BASIS;
	float	fresnelPower = 1.f;

	Vec3	padding00 = Vec3::ZERO;
	float	fresnelfactor = 1.f;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct dissolveData_t
{
	Vec3	startColor;
	float	burnEdgeWidth;

	Vec3	endColor;
	float	burnValue;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eGameObjModels
{
	SPACESHIP ,
	LUMINARIS_SHIP ,
//	W_CRUSIER ,
//	MINI_SHIP ,
//	STAR_SHIP ,
//	STRIDER ,
	
	NUM_GAME_MODELS
};

//--------------------------------------------------------------------------------------------------------------------------------------------
	
enum eGamePlanetaryObjModels
{
	
	
	

	NUM_GAME_PLANET_MODELS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum GAME_DEBUG_SWITCHS
{
	GAME_CAMERA_VIEW_FRUSTUM_CULLING , // rename to view frustum culling since it culls for lights too
	THREADED_VIEW_FRUSTUM_CULLING ,
	VIEW_FRUSTUM_DEBUG_DRAW ,
	THREADED_PARTICLE_UPDATE ,
	THREADED_PARTICLE_SPAWN ,

	NUM_GAME_DEBUG_SWITCHS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum DIRTY_UBOS
{
	GAME_CAMERA ,

	NUM_DIRTY_UBOS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct GameParticleEmitter
{
	ParticleEmitter3D*	m_emitter;
//	SpriteSheet*		m_spriteSheetInUse;
	Vec3				m_center						= Vec3::ZERO;
	float				m_movementRadius				= 5.f;
	Rgba8				m_particleStartColor;
	Rgba8				m_particleEndColor;
	int					m_particleMinLifeTime			= 15;
	int					m_particleMaxLifeTime			= 120;
	float				m_particleVelocity				= 0.f;
	Vec2				m_particleSize					= Vec2::ONE;
	uint				m_numParticlesToSpawnPerFrame	= 500;

	//uint				m_numAliveParticles				= 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum GameStarEmitters
{
	RED_ORANGE_STAR,
	BLUE_CYAN_STAR,
	YELLOW_GREEN_STAR,
	PINK_PURPLE_STAR,

//	TEST_STAR,

	NUM_STARS_EMITTERS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum GameEmitterSpriteSheets
{
	STAR_SS ,
	
	NUM_GAME_SS
};
