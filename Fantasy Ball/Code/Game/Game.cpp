#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Engine/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Ball.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/PowerUpDefinition.hpp"
#include "Game/TheApp.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/UISystem.hpp"
//#include <utility>

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*			g_theRenderer;
extern BitmapFont*				g_bitmapFont;
extern AudioSystem*				g_theAudioSystem;
extern TheApp*					g_theApp;
extern InputSystem*				g_theInput;
extern ParticleSystem2D*		g_theParticleSystem2D;
extern UISystem*				g_theGamplayUISystem;
extern RandomNumberGenerator*	g_RNG;
extern JobSystem*				g_theJobSystem;

//--------------------------------------------------------------------------------------------------------------------------------------------

SpriteSheet* g_tileSpriteSheet		= nullptr;
SpriteSheet* g_characterSpriteSheet = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	
	m_loadAudio		= new std::thread( &Game::LoadAllSounds , this );
	m_loadTextures	= new std::thread( &Game::LoadAllTextures , this );
	m_loadTextures->join();
	m_loadAudio->join();
	
	//m_loadAudioJob = new LoadAudioJob();
	//g_theJobSystem->PostJob( *m_loadAudioJob );
	
	//g_theInput->PushCursorSettings( CursorSettings( RELATIVE_MODE , MOUSE_IS_WINDOWLOCKED , false ) );
	m_cameraHalfHeight	= g_gameConfigBlackboard.GetValue( "cameraHalHeight" , 540.f );
	m_cameraAspectRatio	= g_gameConfigBlackboard.GetValue( "windowAspect" , 1.77f );
	
	m_worldCamera.SetOrthoView( m_cameraHalfHeight , m_cameraAspectRatio );
	// LoadAssets();

	delete m_loadAudio;
	m_loadAudio = nullptr;
	delete m_loadTextures;
	m_loadTextures = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	for ( int index = 0 ; index < NUM_GAME_SS ; index++ )
	{
		delete m_gameSS[ index ];
		m_gameSS[ index ] = nullptr;
	}

	for( int index = 0 ; index < NUM_GAME_TEX ; index++ )
	{
		m_gameTex[ index ] = nullptr;
	}

	for( size_t index = 0 ; index < m_levels.size() ; index++ )
	{
		delete m_levels[ index ];
		m_levels[ index ] = nullptr;
	}
	
	m_currentLevel = nullptr;
	g_theAudioSystem->StopSound( m_currentBackgroundsound );

	//delete m_loadAudio;
	//m_loadAudio = nullptr;
	//
	//delete m_loadTextures;
	//m_loadTextures = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAssets()
{
//	LoadAllTextures();

//	g_theJobSystem->
	
//	LoadAllSounds();
//	LoadAllShaders();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllSounds()
{
	m_sounds[ SFX_BACKGROUND_1 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/LSCarolOfTheBells.mp3" );
	m_sounds[ SFX_BACKGROUND_2 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/LSCrystallize.mp3" );
	m_sounds[ SFX_BACKGROUND_3 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/LSCelticCarol.mp3" );
	m_sounds[ SFX_BACKGROUND_4 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/LSSenbonzakura.mp3" );
	m_sounds[ SFX_BACKGROUND_5 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/KSHMRKashmir.mp3" );
	m_sounds[ SFX_BACKGROUND_6 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/PiratesoftheCaribbean.mp3" );
	m_sounds[ SFX_BACKGROUND_7 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/HesAPirate.mp3" );
	m_sounds[ SFX_BACKGROUND_8 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/ArabianNights.mp3" );
	m_sounds[ SFX_BACKGROUND_9 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/GenieInALamp.mp3" );
	m_sounds[ SFX_BACKGROUND_10 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/FaolanPrincessofPersia.mp3" );
	
	m_sounds[ SFX_LEAVES_RUSTLE ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/Rustle/rustle1.mp3" );
	
	m_sounds[ SFX_GLASS_BREAK_1 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/Shatter/glassBreak1.mp3" );
	m_sounds[ SFX_GLASS_BREAK_2 ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/Shatter/LightBulbBreaking.wav" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllTextures()
{
	m_gameTex[ TEX_BACKGROUND_FOREST_1 ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Background/ParallaxForestBlue.png" );
	m_gameTex[ TEX_BACKGROUND_FOREST_2 ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Background/enchantedForest.png" );
	m_gameTex[ TEX_BACKGROUND_AURORA_1 ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Background/aurora1.png" );
	
	m_gameTex[ TEX_PADDLE ]					= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Paddle/Paddle.png" );
	m_gameTex[ TEX_LEFT_WALL ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Bounds/LeftWall.png" );
	m_gameTex[ TEX_RIGHT_WALL ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Bounds/RightWall.png" );
	m_gameTex[ TEX_TOP_WALL_SECTION ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Bounds/TopWallSection.png" );

	Texture* FlowersTex						= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FlowerSpriteSheet.png" );
	m_gameSS[ SS_VFX_FLOWERS ]				= new SpriteSheet( *FlowersTex , IntVec2( 2 , 2 ) );

	Texture* LeavesTex						= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/LeavesSpriteSheet.png" );
	m_gameSS[ SS_VFX_LEAVES ]				= new SpriteSheet( *LeavesTex , IntVec2( 3 , 3 ) );

	Texture* ballGrabTex					= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/BallHold2.png" );
	m_gameSS[ SS_BALLGRAB ]					= new SpriteSheet( *ballGrabTex , IntVec2( 5 , 2 ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllShaders()
{
	m_gameShader[ GSHADER_VMOVE ]		= g_theRenderer->GetOrCreateShader( "Data/Shaders/vMovementEffect.hlsl" );
	m_gameShader[ GSHADER_HMOVE ]		= g_theRenderer->GetOrCreateShader( "Data/Shaders/hMovementEffect.hlsl" );
	m_gameShader[ GSHADER_COMBINEIMG ]	= g_theRenderer->GetOrCreateShader( "Data/Shaders/combineImage.hlsl" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PostGameConstructDataOnce()
{
	TileDefinition::s_definitions.clear();
	MapDefinition::s_definitions.clear();
	PowerUpDefinition::s_definitions.clear();
	
	TileDefinition::CreateTileDefinitions( "Data/GamePlay/TileDefs.xml" );
	MapDefinition::CreateMapDefinitions( "Data/GamePlay/MapDefs.xml" );
	PowerUpDefinition::CreatePowerUpDefinitions( "Data/GamePlay/PowerUpDefs.xml" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PostGameConstruct()
{
	for( auto totalMaps = MapDefinition::s_definitions.cbegin(); totalMaps != MapDefinition::s_definitions.cend(); totalMaps++ )
	{
		Map* temp = new Map( this , totalMaps->second , totalMaps->second->m_name );
		m_levels.push_back( temp );
	}

	m_currentLevel = m_levels[ 0 ];
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			DebugTile
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//m_currentLevel->SpawnNewEntity( TILE , Vec2::ZERO , TileDefinition::s_definitions.at( "NormalOrange" ) );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PlayRandomGameBackgroundMusic()
{
	int backgroundSoundIndex = g_RNG->RollRandomIntInRange( SFX_BACKGROUND_1 , SFX_BACKGROUND_7 );
	m_currentBackgroundsound = g_theAudioSystem->PlaySound( m_sounds[ backgroundSoundIndex ] , true , m_backgroundMusicVol );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::StopGameBackgroundMusic()
{
	g_theAudioSystem->StopSound( m_currentBackgroundsound );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	UpdateCamera();
	m_currentLevel->Update( deltaSeconds );
	UpdateFromKeyBoard();
	
	if( m_playerHealth == 0 )
	{
		g_theGamplayUISystem->SetGameState( GAME_OVER_STATE );
		m_isGameDirty = true;
	}
	
	UpdateMoveToNextMap();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateMoveToNextMap()
{
	if ( m_currentLevel->m_numAliveTiles == 0 )
	{
		m_currentLevelNumber++;
		m_isBallLaunchable = true;
		m_currentLevel->DestroyEmitters();
		
		if( m_currentLevelNumber == ( ( int ) m_levels.size() ) )
		{
			g_theGamplayUISystem->SetGameState( GAME_OVER_STATE );
			g_theGamplayUISystem->ResetAnimTime();
			m_isGameDirty = true;
			g_theInput->PushCursorSettings( CursorSettings( ABSOLUTE_MODE , MOUSE_IS_UNLOCKED , true ) );
			return;
		}
		m_currentLevel = m_levels[ m_currentLevelNumber ];
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
	m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	m_screenShakeIntensity = Clamp( m_screenShakeIntensity , 0.f , 1.f );

	float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast< float >( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	float cameraShakeY = static_cast< float >( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );

	m_worldCamera.SetOrthoView( m_cameraHalfHeight , m_cameraAspectRatio );
	m_worldCamera.Translate2D( Vec2( cameraShakeX , cameraShakeY ) );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_worldCamera );
	g_theRenderer->SetBlendMode( ALPHA );

	m_currentLevel->Render();

	g_theParticleSystem2D->Render();
	
	if ( m_isDebugDraw )
	{
		Vec2 cameraMins = m_worldCamera.GetOrthoMin().GetXYComponents();
		Vec2 cameraMaxs = m_worldCamera.GetOrthoMax().GetXYComponents();
		g_theRenderer->DrawLine( Vec2( cameraMins.x , 0.f ) , Vec2( cameraMaxs.x , 0.f ) , MAGENTA , 5.f );
		g_theRenderer->DrawLine( Vec2( 0.f , cameraMins.y ) , Vec2( 0.f , cameraMaxs.y ) , MAGENTA , 5.f );
	}

	g_theRenderer->EndCamera( m_worldCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity /* = 1.f */ )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::GarbageCollection()
{
	m_currentLevel->GarbageCollection();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::EndFrame()
{
	//UpdateMoveToNextMap();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

SoundPlaybackID Game::GetSFX( eGameAudioFX SFXid ) const
{
	return m_sounds[ SFXid ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard()
{
	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_isBallLaunchable = false;
	}
	
	if( g_theInput->WasKeyJustPressed( KEY_F2 ) )
	{
		//Entitylist& ballList = m_currentLevel->m_entityListsByType[ BALL ];
		Ball* theBall = nullptr;
			  theBall = ( Ball* ) m_currentLevel->GetFirstValidEntryInList( BALL );
		theBall->m_pos = Vec2::ZERO_ONE * 300.f;
	}
	
	if( g_theInput->WasKeyJustPressed( KEY_F1 ) )
	{
		m_isDebugDraw = !m_isDebugDraw;
	}

	if( g_theInput->WasKeyJustPressed( KEY_ESC ) )
	{
		//g_theGamplayUISystem->SetGameState( GAME_OVER_STATE );
		//StopGameBackgroundMusic();
		//g_theGamplayUISystem->ResetAnimTime();
		//m_isGameDirty = true;
		//g_theInput->PushCursorSettings( CursorSettings( ABSOLUTE_MODE , MOUSE_IS_UNLOCKED , true ) );
	}

	if( g_theInput->WasKeyJustPressed( 'N' ) )
	{
		m_currentLevelNumber++;
		if( m_currentLevelNumber == ( ( int ) m_levels.size() ) )
		{
			m_currentLevelNumber = 0;
		}
		m_currentLevel = m_levels[ m_currentLevelNumber ];
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
