#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TheApp.hpp"

extern DevConsole* g_theDevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;
extern TheApp*			g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	LoadAssets();
	m_world = new World( this );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	UpdateCamera();
	
	if( !g_theDevConsole->IsOpen() )
	{
		UpdateGameplayFromXbox();
	}
		
	m_world->Update( deltaSeconds );
}
	
//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LateUpdate()
{
	if ( ( m_world != nullptr ) && ( m_world->m_currentMap != nullptr ) )
	{
		m_world->m_currentMap->GarbageCollection();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateGameplayFromXbox()
{
	m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
	if ( m_controllerID < 0 )
		return;
	const XboxController& controller = g_theInput->GetXboxController( m_controllerID );
	if ( !controller.IsConnected() )
		return;
	const KeyButtonState& startButton = controller.GetButtonState( XBOX_BUTTON_ID_START );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	RenderGameplay();
}


//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderGameplay() const
{
	if ( m_world->m_currentMapNumber >= 0 && m_world->m_currentMapNumber <= TOTAL_MAPS_IN_THE_GAME )
	{
		g_theRenderer->BindTexture( nullptr );
		m_world->Render();
		
		//if ( !m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
		//{
		//	g_theRenderer->BindTexture( m_textures[ TEXTURE_TILE_TERRAIN_8x8 ] );
		//	g_theRenderer->BindTexture( nullptr );
		//}
		//g_theRenderer->BindTexture( nullptr );
		//if ( m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
		//{
		//	//int healthIcons = m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_health;
		//	//for ( healthIcons ; healthIcons > 0; healthIcons-- )
		//	//{
		//		
		//	//}
		//}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
//--------------------------------------------------------------------------------------------------------------------------------------------
//					CLAMPING THE CAMERA 
//--------------------------------------------------------------------------------------------------------------------------------------------

	//Entitylist& playerEntityList = m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ];
	//static Vec2 tankPosition;
	//if ( playerEntityList[0] )
	//{
	//	tankPosition = playerEntityList[ 0 ]->m_position;
	//}
	//
	//Vec2 cameraCoords;
	//
	//cameraCoords.x = Clamp( tankPosition.x , ( m_tilesInViewHorizontally / 2 ) , m_world->m_currentMap->m_size.x - ( m_tilesInViewHorizontally / 2 ) );
	//cameraCoords.y = Clamp( tankPosition.y , ( m_tilesInViewVertically / 2 )   , m_world->m_currentMap->m_size.y - ( m_tilesInViewVertically   / 2 ) );
	//
	//m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	//m_screenShakeIntensity = ClampZeroToOne( m_screenShakeIntensity );
	//
	//float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	//float cameraShakeX = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	//float cameraShakeY = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	//m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAssets()
{
	TileDefinition::CreateTileDefinitions( "Data/GamePlay/TileDefs.xml" );
	MapDefinition::CreateMapDefinitions( "Data/GamePlay/MapDefs.xml" );
	LoadAllEntityTextures();
	//LoadAllSounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllEntityTextures()
{
	m_textures[ TEXTURE_PLAYER_TANK ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_textures[ TEXTURE_PLAYER_TURRET ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankTop.png" );
	m_textures[ TEXTURE_ENEMY_TANK1 ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTank1.png" );
	m_textures[ TEXTURE_ENEMY_TANK2 ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTank2.png" );
	m_textures[ TEXTURE_ENEMY_TANK3 ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTank3.png" );
	m_textures[ TEXTURE_ENEMY_TANK4 ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTank4.png" );
	m_textures[ TEXTURE_FRIENDLY_TANK1 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTank1.png" );
	m_textures[ TEXTURE_FRIENDLY_TANK2 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTank2.png" );
	m_textures[ TEXTURE_FRIENDLY_TANK3 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTank3.png" );
	m_textures[ TEXTURE_FRIENDLY_TANK4 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTank4.png" );
	m_textures[ TEXTURE_ENEMY_TURRET ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTurretTop.png" );
	m_textures[ TEXTURE_ENEMY_TURRET_BASE ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTurretBase.png" );
	m_textures[ TEXTURE_FRIENDLY_TURRET_BASE ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTurretBase.png" );
	m_textures[ TEXTURE_FRIENDLY_TURRET ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTurretTop.png" );
	m_textures[ TEXTURE_NEMESIS_TANK ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/NemesisTankBase.png" );
	m_textures[ TEXTURE_NEMESIS_TURRET ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/NemesisTankTop.png" );
	m_textures[ TEXTURE_BULLET ]					= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Bullet.png" );
	m_textures[ TEXTURE_TILE_TERRAIN_8x8 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	m_textures[ TEXTURE_TILE_BOULDER ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Extras_4x4.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Startup()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllSounds()
{
	m_sounds[ SOUND_GAME_START ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/GoGoGO.mp3" );
	m_sounds[ SOUND_GAME_OVER ]		= g_theAudioSystem->CreateOrGetSound( "Data/Audio/terroristswin.mp3" );
	m_sounds[ SOUND_GAME_WON ]		= g_theAudioSystem->CreateOrGetSound( "Data/Audio/counterterroristswin.mp3" );
	m_sounds[ SOUND_MAP_CHANGE ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/MapExited.mp3" );
	m_sounds[ SOUND_PLAYER_SHOOT ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerShoot.wav" );
	m_sounds[ SOUND_ENEMY_SHOOT ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
	m_sounds[ SOUND_ENEMY_SPOTTED ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemySpotted.mp3" );
	m_sounds[ SOUND_BOULDER_MOVED ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Rock_DebrisFall2.wav" );
	m_sounds[SOUND_PLAYER_DAMAGED]  = g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerHit.wav" );
	m_sounds[ SOUND_ENEMY_DAMAGED ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );
	m_sounds[ SOUND_PLAYER_DIED ]   = g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerDied.wav" );
	m_sounds[ SOUND_ENEMY_DIED ]    = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );

	m_sounds[ SOUND_ATTRACT_MUSIC ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/AttractMusic.mp3" );
	m_sounds[ SOUND_GAME_PAUSED ]   = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Pause.mp3" );
	m_sounds[ SOUND_GAME_UNPAUSED ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Unpause.mp3" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
