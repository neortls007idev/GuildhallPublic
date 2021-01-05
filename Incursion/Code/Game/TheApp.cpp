#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

		AudioSystem*						g_theAudioSystem	= nullptr;
		DevConsole*							g_theDevConsole		= nullptr;
		InputSystem*						g_theInput			= nullptr;
		RenderContext*						g_theRenderer		= nullptr;
		TheApp*								g_theApp			= nullptr;
		Game*								g_theGame			= nullptr;
extern	BitmapFont*							g_bitmapFont;

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::TheApp()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::~TheApp()
{
	delete g_theGame;
	g_theGame = nullptr;

	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;
	// 
	// 	delete g_thePhysicsSystem;
	// 	g_thePhysicsSystem = nullptr;

	delete g_theDevConsole;
	g_theDevConsole = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;

	delete g_theInput;
	g_theInput = nullptr;

	delete g_theEventSystem;
	g_theEventSystem = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Startup()
{
	Clock::Startup();

	if ( g_theEventSystem == nullptr )
	{
		g_theEventSystem = new EventSystem();
	}
	g_theEventSystem->Startup();

	if ( g_theWindow == nullptr )
	{
		g_theWindow = new Window();
	}

	if ( g_theInput == nullptr )
	{
		g_theInput = new InputSystem();
		g_theWindow->SetInputSystem( g_theInput );
	}
	g_theInput->Startup();

	if ( g_theRenderer == nullptr )
	{
		g_theRenderer = new RenderContext();
	}
	g_theRenderer->Startup( g_theWindow );

	if ( g_bitmapFont == nullptr )
	{
		g_bitmapFont = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" ); // TO DO PASS IN THE FONT ADDRESS AND THE TEXTURE POINTER TO IT.
	}

	if ( g_theDevConsole == nullptr )
	{
		g_theDevConsole = new DevConsole();
	}
	g_theDevConsole->Startup();
	// 	AddDebugRenderDevConsoleCommands( g_theDevConsole );

	// 	if ( g_currentManager == nullptr )
	// 	{
	// 		// instantiating a default DRO_Manager
	// 		g_currentManager = new DebugRenderObjectsManager();
	// 	}
	// 	g_currentManager->Startup();

	// 	if ( g_thePhysicsSystem == nullptr )
	// 	{
	// 		g_thePhysicsSystem = new Physics2D();
	// 	}
	// 	g_thePhysicsSystem->Startup();

	if ( g_theAudioSystem == nullptr )
	{
		g_theAudioSystem = new AudioSystem();
		g_theAudioSystem->Startup();
	}

	if ( g_theGame == nullptr )
	{
		g_theGame = new Game();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::RunFrame()
{
	static double timeLastFrameStarted = GetCurrentTimeSeconds();
	double        timeThisFrameStarted = GetCurrentTimeSeconds();
	double		  deltaSeconds		   = timeThisFrameStarted - timeLastFrameStarted;
	timeLastFrameStarted			   = timeThisFrameStarted;
	
	BeginFrame();                        // all engine system and not game systems
	Update( ( float ) deltaSeconds );	
	Render();
	EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::BeginFrame()
{
	// all engine things that must begin at the beginning of each frame and not the game
	Clock::BeginFrame();
	g_theEventSystem->BeginFrame();
	g_theWindow->BeginFrame();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_theAudioSystem->BeginFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Update( float deltaSeconds )
{
	UpdateFromKeyboard();
	
	if ( m_isPaused )							{ deltaSeconds = 0.f; }
	else if ( m_isSloMo == true )				{ deltaSeconds /= 10.f; }
	if ( m_isSpeedMo )							{ deltaSeconds = deltaSeconds * 4.0f; }

	g_theGame->Update( deltaSeconds );

	if ( g_theDevConsole->IsOpen() )
	{
		g_theDevConsole->Update( deltaSeconds );
	}

	g_theInput->EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
		g_theRenderer->BeginCamera( g_theGame->m_worldCamera );
		g_theRenderer->SetBlendMode( ALPHA );
		g_theGame->Render();
		g_theRenderer->EndCamera( g_theGame->m_worldCamera );

		//g_theGame->RenderUI();

		if ( g_theDevConsole->IsOpen() )
		{
			//g_theGame->m_worldCamera.SetOrthoView( Vec2( 5 , 5 ) , Vec2( 35 , 35 ) );
			g_theDevConsole->Render( *g_theRenderer , *g_theDevConsole->GetDevConsoleCamera() , 14.f );
		}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	// all engine things that must end at the end of the frame and not the game
/*	g_currentManager->EndFrame();*/
	g_theAudioSystem->EndFrame();
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();

	Clock::EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	delete g_theGame;
	g_theGame = nullptr;

	g_theAudioSystem->Shutdown();
	// 	g_thePhysicsSystem->Shutdown();
	// 	g_currentManager->Shutdown();
	g_theDevConsole->Shutdown();
	g_theRenderer->Shutdown();
	g_theInput->Shutdown();
	// TODO :- write me g_theWindow->Shutdown();
	g_theEventSystem->Shutdown();
	Clock::Shutdown();
	g_theRenderer->Shutdown();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TheApp::HandleQuitRequested()
{
	m_isQuitting = true;
	return m_isQuitting;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::UpdateFromKeyboard()
{
	if ( g_theInput->GetButtonState( 'T' ).IsPressed() )									{ m_isSloMo = true; }
	else																						{ m_isSloMo = false; }
	
	if ( g_theInput->GetButtonState( 'Y' ).IsPressed() )									{ m_isSpeedMo = true; }
	else																						{ m_isSpeedMo = false; }

	if ( g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() && g_theGame->m_gameState == GameStates::GAME_STATE_ATTRACT )
	{
		HandleQuitRequested();
		g_theWindow->HandleQuitRequested();
	}
	
	if ( g_theGame->m_gameState == GameStates::GAME_STATE_PLAYING && m_isPaused && g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() )
	{
		g_theGame->m_gameState = GameStates::GAME_STATE_ATTRACT;
	}
	
	if ( g_theGame->m_gameState == GameStates::GAME_STATE_PLAYING && ( g_theInput->GetButtonState( 'P' ).WasJustPressed() || g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() ) ) 
	{ 
		m_isPaused = !m_isPaused; 
		
		if ( m_isPaused == true )
		{
			g_theAudioSystem->PlaySound( g_theGame->m_sounds[ SOUND_GAME_PAUSED ] );
		}
		if ( m_isPaused == false  )
		{
			g_theAudioSystem->PlaySound( g_theGame->m_sounds[ SOUND_GAME_UNPAUSED ] );
		}
	}
	
	if ( g_theInput->GetButtonState( KEY_F4 ).WasJustPressed() )
	{
		m_debugCamera = !m_debugCamera;
	}
	
	if ( g_theInput->GetButtonState( KEY_F8 ).WasJustPressed() ) 
	{
		delete g_theGame;
		g_theGame = nullptr;
		g_theGame = new Game();
	}	
}

//--------------------------------------------------------------------------------------------------------------------------------------------
