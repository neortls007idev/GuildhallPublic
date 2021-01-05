#include "Game/GameCommon.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	m_font = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( INTITAL_MAP_SIZE_X , INTITAL_MAP_SIZE_Y ) );
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_uiCamera.SetClearMode( CLEAR_NONE , BLACK );
	m_gameState = GameStates::GAME_STATE_LOADING;
}


//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	if ( LoadingState() ) { return; }

	else if ( AttractState() ) { return; }
	
	else if ( m_gameState == GameStates::GAME_STATE_PLAYING && m_world1->m_currentMapNumber >= 0 && m_world1->m_currentMapNumber < TOTAL_MAPS_IN_THE_GAME )
	{
		if ( CheckIfPlayerLost( deltaSeconds ) )
		{ return; }
				
		UpdateCamera();
		UpdateFromKeyBoard();
		UpdateGameplayFromXbox();
		GameHasJustStarted();
		m_world1->Update( deltaSeconds );
	}
	
	else if ( m_world1->m_currentMapNumber == TOTAL_MAPS_IN_THE_GAME )
	{
		m_gameState = GameStates::GAME_STATE_VICTORY;
		static bool hasVictorySoundPlayed = true;
		if ( hasVictorySoundPlayed )
		{
			g_theAudioSystem->PlaySound( m_sounds[ SOUND_GAME_WON ] );
			hasVictorySoundPlayed = false;
		}
		if ( m_victoryPanelFadeTimer > 0.f)
		{

			m_world1->Update( deltaSeconds );
			m_fadeColor = Rgba8( 0 , 0 , 0 , 255 );
			int fadeColorAlpha = RoundDownToInt( RangeMapFloat( 0.5f , 0.f , 0.f , 255.f , m_victoryPanelFadeTimer ) );
			m_fadeColor.a = ( uchar ) fadeColorAlpha;
		}
		m_victoryPanelFadeTimer -= deltaSeconds;
		static float victoryAttractTimer = 1.f;
		victoryAttractTimer -= deltaSeconds;
		if ( victoryAttractTimer <=0 )
		{
			XboxUpdateOnVictory( victoryAttractTimer );
			if ( g_theInput->WasKeyJustPressed('P') || g_theInput->WasKeyJustPressed( KEY_ESC ) )
			{
				delete m_world1;
				m_world1 = nullptr;
				m_world1 = new World( this );
				victoryAttractTimer = 1.f;
				hasVictorySoundPlayed = true;
				m_gameState = GameStates::GAME_STATE_ATTRACT;
			}
		}
	}
	
	if ( g_theInput->WasKeyJustPressed( 'M' ) )
	{
		m_world1->m_currentMapNumber++;
		if ( m_world1->m_currentMapNumber > TOTAL_MAPS_IN_THE_GAME )
		{
			m_world1->m_currentMapNumber = -1;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::XboxUpdateOnVictory( float& victoryTimer )
{
	m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
	if ( m_controllerID < 0 )
		return;
	const XboxController& controller = g_theInput->GetXboxController( m_controllerID );
	if ( !controller.IsConnected() )
		return;
	const KeyButtonState& startButton = controller.GetButtonState( XBOX_BUTTON_ID_START );
	const KeyButtonState& backButton = controller.GetButtonState( XBOX_BUTTON_ID_BACK );
	if ( startButton.WasJustPressed() || backButton.WasJustPressed() )
	{
		delete m_world1;
		m_world1 = nullptr;
		m_world1 = new World( this );
		victoryTimer = 1.f;
		m_gameState = GameStates::GAME_STATE_ATTRACT;
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
	
	if ( startButton.WasJustPressed() )
	{
		g_theApp->m_isPaused = !g_theApp->m_isPaused;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::CheckIfPlayerLost( float deltaSeconds )
{
	if ( !m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] || m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] <= 0 )
	{
		if ( m_gameOverAudio )
		{
			g_theAudioSystem->PlaySound( m_sounds[ SOUND_GAME_OVER ] );
		}
		m_gameOverAudio = false;
		m_world1->Update( deltaSeconds );
		static float gameOverAttractTimer = 3.f;
		gameOverAttractTimer -= deltaSeconds;
		if ( gameOverAttractTimer <= 0 )
		{
			delete m_world1;
			m_world1 = nullptr;
			m_world1 = new World( this );
			gameOverAttractTimer = 3.f;
			m_gameState = GameStates::GAME_STATE_ATTRACT;
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::GameHasJustStarted()
{
	if ( m_gameJustStarted )
	{
		UpdateCamera();
		g_theAudioSystem->PlaySound( m_sounds[ SOUND_GAME_START ] ) ;
		m_gameJustStarted = false;
		g_theApp->m_isPaused = false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	switch ( m_gameState )
	{
	case GameStates::GAME_STATE_LOADING:
		RenderLoadingScreen();
		return;
	case GameStates::GAME_STATE_ATTRACT:
		RenderAttractScreen();
		return;
	case GameStates::GAME_STATE_PLAYING:
		RenderGameplay();
		break;
	case GameStates::GAME_STATE_VICTORY:
		RenderVictory();
		break;
	default:
		break;
	}



}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderVictory() const
{
	if ( m_world1->m_currentMapNumber == TOTAL_MAPS_IN_THE_GAME )
	{
		RenderGameplay();
		g_theRenderer->BeginCamera( m_worldCamera );
		g_theRenderer->BindTexture( nullptr );
		AABB2 pausePanel( Vec2::ZERO , Vec2( m_world1->m_currentMap->m_size.x , m_world1->m_currentMap->m_size.y ) );
		std::vector<Vertex_PCU> victoryPanelVerts;
		AppendVertsForAABB2( victoryPanelVerts , pausePanel , m_fadeColor );
		g_theRenderer->DrawVertexArray( victoryPanelVerts );
		g_theRenderer->BindTexture( m_textures[ TEXTURE_TILE_TERRAIN_8x8 ] );
		Vec2 textPosition = m_worldCamera.GetOrthoMin().GetXYComponents() +
							( ( m_worldCamera.GetOrthoMax().GetXYComponents() - m_worldCamera.GetOrthoMin().GetXYComponents() ) * 0.5f );
		DrawTextTriangles2D( *g_theRenderer , "VICTORY!!!" , Vec2( textPosition.x - 4.3f , textPosition.y ) , 1.5f , PINK , 0.5f );
		g_theRenderer->BindTexture( nullptr );
		std::vector<Vertex_PCU> textVerts;
		m_font->AddVertsForText2D( textVerts , Vec2( textPosition.x - 6.f , textPosition.y - .5f ) , .25f , "CONTROLLER START BUTTON OR CONTROLLER BACK BUTTON TO GO BACK TO ATTRACT SCREEN" , GREEN , 0.6f );
		m_font->AddVertsForText2D( textVerts , Vec2( textPosition.x - 4.75f , textPosition.y - 1.f ) , .25f , "OR ESC KEY OR P KEY ON THE KEYBOARD TO GO BACK TO ATTRACT SCREEN" , GREEN , 0.6f );
		g_theRenderer->BindTexture( m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( textVerts );
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->EndCamera( m_worldCamera );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderGameplay() const
{
	if ( m_world1->m_currentMapNumber >= 0 && m_world1->m_currentMapNumber <= TOTAL_MAPS_IN_THE_GAME )
	{
		g_theRenderer->BindTexture( nullptr );
		m_world1->Render();

		if ( !m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
		{
			g_theRenderer->BindTexture( m_textures[ TEXTURE_TILE_TERRAIN_8x8 ] );
			Vec2 textPosition = m_worldCamera.GetOrthoMin().GetXYComponents() +
								( ( m_worldCamera.GetOrthoMax().GetXYComponents() - m_worldCamera.GetOrthoMin().GetXYComponents() ) * 0.5f );
			DrawTextTriangles2D( *g_theRenderer , "GAME OVER" , Vec2( textPosition.x - 4 , textPosition.y ) , 1.5f , RED , 0.5f );
			g_theRenderer->BindTexture( nullptr );
		}
		g_theRenderer->BindTexture( nullptr );
		if ( m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
		{
			//int healthIcons = m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_health;
			//for ( healthIcons ; healthIcons > 0; healthIcons-- )
			//{
				RenderUI();
			//}
		}
		if ( g_theApp->m_isPaused )
		{
			m_worldCamera.SetClearMode( CLEAR_NONE , BLACK );
			g_theRenderer->BeginCamera( m_worldCamera );
			g_theRenderer->SetBlendMode( ALPHA );
			g_theRenderer->BindTexture( nullptr );
			AABB2 pausePanel( Vec2::ZERO , Vec2( m_world1->m_currentMap->m_size.x , m_world1->m_currentMap->m_size.y ) );
			std::vector<Vertex_PCU> pausePanelVerts;
			AppendVertsForAABB2( pausePanelVerts , pausePanel , Rgba8( 0 , 0 , 0 , 150 ) );
			g_theRenderer->DrawVertexArray( pausePanelVerts );
			Vec2 textPosition = m_worldCamera.GetOrthoMin().GetXYComponents() +
							( ( m_worldCamera.GetOrthoMax().GetXYComponents() - m_worldCamera.GetOrthoMin().GetXYComponents() ) * 0.5f );
			DrawTextTriangles2D( *g_theRenderer , "GAME PAUSED" , Vec2( textPosition.x - 4.f , textPosition.y ) , 1.f , YELLOW , 0.5f );
			DrawTextTriangles2D( *g_theRenderer , "PRESS P or START BUTTON TO RESUME" , Vec2( textPosition.x - 3.15f, textPosition.y - 0.5f ) , 0.5f , YELLOW , 0.25f );
			DrawTextTriangles2D( *g_theRenderer , "OR PRESS ESC BUTTON TO RETURN TO THE ATTRACT SCREEN" , Vec2( textPosition.x - 4.5f , textPosition.y - 1.f ) , 0.5f , YELLOW , 0.25f );
			g_theRenderer->EndCamera( m_worldCamera );
			m_worldCamera.SetClearMode( CLEAR_COLOR_BIT , BLACK );
		}

	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderLoadingScreen() const
{
	std::vector<Vertex_PCU> textVerts;
	m_font->AddVertsForText2D( textVerts , Vec2( INTITAL_MAP_SIZE_X - 10 , INTITAL_MAP_SIZE_Y ) / 2 , 1.5f , "LOADING..." , CYAN , 0.6f );
	g_theRenderer->BindTexture( m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( textVerts );
	g_theRenderer->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderAttractScreen() const
{
		g_theRenderer->BindTexture( m_textures[ TEXTURE_TILE_TERRAIN_8x8 ] );
		DrawTextTriangles2D( *g_theRenderer , "WELCOME TO INCURSION" , Vec2( INTITAL_MAP_SIZE_X - 18 , INTITAL_MAP_SIZE_Y ) / 2 , 1.5f , RED , 0.5f );
		g_theRenderer->BindTexture( nullptr );
		Vec2 textPosition = m_worldCamera.GetOrthoMin().GetXYComponents() +
							( ( m_worldCamera.GetOrthoMax().GetXYComponents() - m_worldCamera.GetOrthoMin().GetXYComponents() ) * 0.5f );
		DrawTextTriangles2D( *g_theRenderer , "CONTROLLER START BUTTON OR ENTER KEY TO START" , Vec2( textPosition.x - 7 , textPosition.y - 2.5f ) , 1.0f , GREEN , 0.25f );
		DrawTextTriangles2D( *g_theRenderer , "ESC KEY TO QUIT" , Vec2( textPosition.x - 3.f , textPosition.y - 4.0f ) , 1.0f , YELLOW , 0.25f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderUI() const
{
	g_theRenderer->BeginCamera( m_uiCamera );
	g_theRenderer->SetBlendMode( ALPHA );
	RenderUIVerts();
	g_theRenderer->EndCamera( m_uiCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
//--------------------------------------------------------------------------------------------------------------------------------------------
//					CLAMPING THE CAMERA 
//--------------------------------------------------------------------------------------------------------------------------------------------

	Entitylist& playerEntityList = m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ];
	static Vec2 tankPosition;
	if ( playerEntityList[0] )
	{
		tankPosition = playerEntityList[ 0 ]->m_position;
	}

	Vec2 cameraCoords;
	
	cameraCoords.x = Clamp( tankPosition.x , ( m_tilesInViewHorizontally / 2 ) , m_world1->m_currentMap->m_size.x - ( m_tilesInViewHorizontally / 2 ) );
	cameraCoords.y = Clamp( tankPosition.y , ( m_tilesInViewVertically / 2 )   , m_world1->m_currentMap->m_size.y - ( m_tilesInViewVertically   / 2 ) );

	m_worldCamera.SetOrthoView( cameraCoords - m_CameraCenter , cameraCoords + m_CameraCenter );

	if ( g_theApp->m_debugCamera )
	{
		// TO DO :- MOVE TO MAP
		Vec2 currentMapSize = Vec2( g_theGame->m_world1->m_currentMap->m_size );
		if ( currentMapSize.x > currentMapSize.y )
		{
			g_theGame->m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( currentMapSize.x , currentMapSize.y * CLIENT_ASPECT ) );
		}
		if ( currentMapSize.x < currentMapSize.y )
		{
			g_theGame->m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( currentMapSize.x * CLIENT_ASPECT , currentMapSize.y ) );
		}
		else
		{
			g_theGame->m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( currentMapSize.x , currentMapSize.y ) );
		}
	}

	m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	m_screenShakeIntensity = ClampZeroToOne( m_screenShakeIntensity );

	float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	float cameraShakeY = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	
	m_worldCamera.Translate2D( Vec2( cameraShakeX , cameraShakeY ) );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::LoadingState()
{
	if ( m_gameState == GameStates::GAME_STATE_LOADING )
	{
		if ( m_hasLoadingStateStarted )
		{
			m_hasLoadingStateStarted = false;
			return true;
		}
		else
		{
			m_gameState = GameStates::GAME_STATE_ATTRACT;
			LoadAssets();
			m_world1 = new World( this );
			m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( m_world1->m_currentMap->m_size.x , m_world1->m_currentMap->m_size.y ) );
			m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
			return false;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::AttractState()
{
	if ( m_gameState == GameStates::GAME_STATE_ATTRACT )
	{
		m_worldCamera.SetOrthoView( Vec2::ZERO , Vec2( INTITAL_MAP_SIZE_X , INTITAL_MAP_SIZE_Y ) );
		m_victoryPanelFadeTimer = 0.5f;
		static bool startAttractMusic = true;
		if ( startAttractMusic )
		{
			//SoundPlaybackID attractMusic = g_theAudioSystem->PlaySound( SOUND_ATTRACT_MUSIC , true );
			startAttractMusic = false;
		}
		m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
		XboxController controller = g_theInput->GetXboxController( m_controllerID );
		const KeyButtonState& startButton = controller.GetButtonState( XBOX_BUTTON_ID_START );
		if ( g_theInput->WasKeyJustPressed( KEY_ENTER ) || startButton.WasJustPressed() )
		{
			if (m_world1)
			{
				delete m_world1;
				m_world1 = nullptr;
				m_world1 = new World( this );
			}
			m_worldCamera.SetOrthoView( Vec2::ZERO , Vec2( INTITAL_MAP_SIZE_X , INTITAL_MAP_SIZE_Y ) );
			//m_world1->m_currentMapNumber++;
			m_gameJustStarted = true;
			m_gameOverAudio = true;
			m_gameState = GameStates::GAME_STATE_PLAYING;
			//g_theAudioSystem->StopSound( attractMusic );
			return false;
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}


//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Die()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard()
{
	if ( g_theInput->GetButtonState( KEY_F1 ).WasJustPressed() )
	{ m_debugDraw = !m_debugDraw; }
	
	if ( g_theInput->GetButtonState( KEY_F3 ).WasJustPressed() ) 
	{ m_isClipping = !m_isClipping; }
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderUIVerts() const
{
	int lifeIcons = ( m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_health ) + ( PLAYER_HEALTH_PER_LIVES - 1 );
	lifeIcons /= PLAYER_HEALTH_PER_LIVES;
	for ( lifeIcons ; lifeIcons > 0; lifeIcons-- )
	{
//--------------------------------------------------------------------------------------------------------------------------------------------
//										PLAYER TANK VERTS
//--------------------------------------------------------------------------------------------------------------------------------------------
		Vertex_PCU livesTankVerts[] = {
											Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , WHITE, Vec2( 0.f, 0.f ) ),
											Vertex_PCU( Vec3(  0.4f , -0.4f , 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
											Vertex_PCU( Vec3( -0.4f ,  0.4f , 0.f ) , WHITE, Vec2( 0.f, 1.f ) ),

											Vertex_PCU( Vec3(  0.4f , -0.4f , 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
											Vertex_PCU( Vec3(  0.4f ,  0.4f , 0.f ) , WHITE, Vec2( 1.f, 1.f ) ),
											Vertex_PCU( Vec3( -0.4f ,  0.4f , 0.f ) , WHITE, Vec2( 0.f, 1.f ) ) };

//--------------------------------------------------------------------------------------------------------------------------------------------
//										PLAYER TANK TURRET VERTS
//--------------------------------------------------------------------------------------------------------------------------------------------
		
		Vertex_PCU livesTankTurretVerts[] = {   Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , WHITE , Vec2( 0.f , 0.f ) ),
												Vertex_PCU( Vec3(  0.4f , -0.4f , 0.f ) , WHITE , Vec2( 1.f , 0.f ) ),
												Vertex_PCU( Vec3( -0.4f ,  0.4f , 0.f ) , WHITE , Vec2( 0.f , 1.f ) ),

												Vertex_PCU( Vec3(  0.4f , -0.4f , 0.f ) , WHITE , Vec2( 1.f , 0.f ) ),
												Vertex_PCU( Vec3(  0.4f ,  0.4f , 0.f ) , WHITE , Vec2( 1.f , 1.f ) ),
												Vertex_PCU( Vec3( -0.4f ,  0.4f , 0.f ) , WHITE , Vec2( 0.f , 1.f ) )
		};

		constexpr int NUM_LIVES_TANKVERTS = sizeof( livesTankVerts ) / sizeof( Vertex_PCU );
		constexpr int NUM_LIVES_TANKTURRETVERTS = sizeof( livesTankTurretVerts ) / sizeof( Vertex_PCU );

		TransformVertexArray2D( NUM_LIVES_TANKVERTS , livesTankVerts , 85.f , 0.f , Vec2( ( 10.f + ( 75 * lifeIcons ) ) , UI_SIZE_Y - 50.f ) );
		g_theRenderer->BindTexture( m_textures[ TEXTURE_PLAYER_TANK ] );
		g_theRenderer->DrawVertexArray( NUM_LIVES_TANKVERTS , livesTankVerts );
		g_theRenderer->BindTexture( nullptr );
		TransformVertexArray2D( NUM_LIVES_TANKTURRETVERTS , livesTankTurretVerts , 85.f , 0.f , Vec2( ( 10.f + ( 75 * lifeIcons ) ) , UI_SIZE_Y - 50.f ) );
		g_theRenderer->BindTexture( m_textures[ TEXTURE_PLAYER_TURRET ] );
		g_theRenderer->DrawVertexArray( NUM_LIVES_TANKTURRETVERTS , livesTankTurretVerts );
		g_theRenderer->BindTexture( nullptr );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAssets()
{
	TileDefinition::CreateTileDefinitions();
	LoadAllEntityTextures();
	LoadAllSounds();
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
