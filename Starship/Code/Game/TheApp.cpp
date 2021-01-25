#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Game/resource.h"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	RenderContext*						g_theRenderer;
extern	InputSystem*						g_theInput;
extern	DevConsole*							g_theDevConsole;
		TheApp*								g_theApp			= nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::TheApp()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::~TheApp()
{
	delete m_theGame;
	m_theGame = nullptr;

// 	delete g_theAudioSystem;
// 	g_theAudioSystem = nullptr;
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
	g_theCamera->SetOrthoView( Vec2::ZERO , Vec2( WORLD_SIZE_X , WORLD_SIZE_Y ) );
	g_theRenderer->ClearScreen( BLACK );

	void* gameIcon = reinterpret_cast< HICON >( ::LoadImage( GetModuleHandle( NULL ) , MAKEINTRESOURCE( IDI_ICON1 ) , IMAGE_ICON , 0 , 0 , LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE ) );
	g_theWindow->SetNewIcon( gameIcon );

// 	if ( g_bitmapFont == nullptr )
// 	{
// 		g_bitmapFont = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" ); // TO DO PASS IN THE FONT ADDRESS AND THE TEXTURE POINTER TO IT.
// 	}

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
	
// 	if ( g_theAudioSystem == nullptr )
// 	{
// 		g_theAudioSystem = new AudioSystem();
// 	}
// 	g_theAudioSystem->Startup();

	if ( m_theAttractMode == nullptr )
	{
		m_theAttractMode = new AttractMode();
		m_attractModeStatus = true;
	}

	if ( m_theGame == nullptr )
	{
		m_theGame = new Game();
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
//	g_currentManager->BeginFrame();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Update( float deltaSeconds )
{
	HandleKeyPressed();
	if ( m_attractModeStatus )
	{
		m_theAttractMode->Update( deltaSeconds );

	}
	if ( m_attractModeStatus == false )
	{
		if ( m_isPaused )										{ deltaSeconds = 0.f; }
		else if ( m_isSloMo == true )							{ deltaSeconds /= 10.f; }
		else													{ m_theGame->Update( deltaSeconds ); }
	}
	
	if ( m_theGame->m_waveNumber == 5 && m_theGame->m_Ship->m_age <= 0 )
	{
		m_keyPressed[ KEY_SPACE ]	= false;
		m_keyPressed[ 'N' ]			= false;
		m_attractModeStatus			= true;
		
		delete m_theGame;
		m_theGame = new Game;
	}

	if( g_theDevConsole->IsOpen() )
	{
		g_theDevConsole->Update( deltaSeconds );
	}
	
	g_theInput->EndFrame();
	HandleKeyReleased();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BeginCamera( *g_theCamera );
	
	if ( m_attractModeStatus )
	{
		m_theAttractMode->Render();
	}
	if ( m_attractModeStatus == false && m_theGame->m_waveNumber < 5 )
	{
		m_theGame->Render();
		m_theGame->RenderUI();
	}

	if( g_theDevConsole->IsOpen() )
	{
		g_theDevConsole->Render( *g_theRenderer , *g_theDevConsole->GetDevConsoleCamera() , 14.f );
	}
	
	g_theRenderer->EndCamera(*g_theCamera);	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	// all engine things that must end at the end of the frame and not the game
/*	g_currentManager->EndFrame();*/
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();

	Clock::EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	delete m_theGame;
	m_theGame					= nullptr;
	
	delete m_theAttractMode;
	m_theAttractMode			= nullptr;

	delete g_theCamera;
	g_theCamera					= nullptr;
	
// 	g_theAudioSystem->Shutdown();
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
	m_isQuitting				= true;
	return m_isQuitting;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TheApp::IsKeyPressed( unsigned char keyCode )
{
	return m_keyPressed[ keyCode ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TheApp::HandleKeyReleased()
{
	if ( !g_theInput->IsKeyHeldDown( 'T' ) )								{ m_isSloMo = false; }
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TheApp::HandleKeyPressed()
{
	if ( g_theInput->IsKeyHeldDown( 'T' ) )									{ m_isSloMo = true;	}

	if ( g_theInput->WasKeyJustPressed( 'P' ) )								{ m_isPaused = !m_isPaused; }
	
	if ( g_theInput->WasKeyJustPressed( KEY_ESC ) )
	{
		HandleQuitRequested();
		g_theWindow->HandleQuitRequested();
		return false; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	if ( g_theInput->WasKeyJustPressed( KEY_F8 ) )
	{
		delete m_theGame;
		m_theGame = nullptr;
		m_theGame = new Game();
	}
	//g_theInput->HandleKeyPressed()
	return false;
}
