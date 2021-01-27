#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#include "UISystem.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Game/resource.h"
#include "Engine/Time/Timer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem*			g_theInput;						
extern RenderContext*		g_theRenderer;					
extern AudioSystem*			g_theAudioSystem;				
extern DevConsole*			g_theDevConsole;				
extern ParticleSystem2D*	g_theParticleSystem2D;
TheApp*						g_theApp						= nullptr;
Game*						g_theGame						= nullptr;
UISystem*					g_theGamplayUISystem			= nullptr;
//JobSystem*					g_theJobSystem					= nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::TheApp()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::~TheApp()
{
	delete g_theGame;
	g_theGame = nullptr;

	delete g_theGamplayUISystem;
	g_theGamplayUISystem = nullptr;
	
	delete g_theParticleSystem2D;
	g_theParticleSystem2D = nullptr;
	
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

//	delete g_theJobSystem;
//	g_theJobSystem = nullptr;

	delete m_timer;
	m_timer = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Startup()
{
	Clock::Startup();

	m_timer = new Timer();
	m_timer->SetSeconds( &Clock::g_theMasterClock , FRAME_RATE );
	
//	if ( g_theJobSystem == nullptr )
//	{
//		g_theJobSystem = new JobSystem();
//	}
//	g_theJobSystem->Startup();
	
	if ( g_theEventSystem == nullptr )
	{
		g_theEventSystem = new EventSystem();
	}
	g_theEventSystem->Startup();

	if ( g_theWindow == nullptr )
	{
		g_theWindow = new Window();
	}
	g_theWindow->Startup();

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

	SetGameIconAndCursor();


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

	if ( g_theParticleSystem2D == nullptr )
	{
		g_theParticleSystem2D = new ParticleSystem2D();
		g_theParticleSystem2D->Startup();
	}

	if ( g_theGamplayUISystem == nullptr )
	{
		g_theGamplayUISystem = new UISystem();
	}
	
// 	if ( g_theGame == nullptr )
// 	{
// 		g_theGame = new Game();
// 		g_theGame->PostGameConstructDataOnce();
// 		g_theGame->PostGameConstruct();
// 		g_theDevConsole->PrintString( MAGENTA , "GAME HAS STARTED" );
// 	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::SetGameIconAndCursor()
{
	void* gameIcon = reinterpret_cast< HICON >( ::LoadImage( GetModuleHandle( NULL ) , MAKEINTRESOURCE( IDI_ICON1 ) , IMAGE_ICON , 0 , 0 , LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE ) );
	g_theWindow->SetNewIcon( gameIcon );

	void* cursorIcon = reinterpret_cast< HCURSOR >( ::LoadImage( GetModuleHandle( NULL ) , MAKEINTRESOURCE( IDI_ICON2 ) , IMAGE_ICON , 0 , 0 , LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE ) );
	g_theWindow->SetNewCursorIcon( cursorIcon );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::RunFrame()
{
	//static double timeLastFrameStarted = GetCurrentTimeSeconds();
	//double        timeThisFrameStarted = GetCurrentTimeSeconds();
	//double		  deltaSeconds		   = timeThisFrameStarted - timeLastFrameStarted;
	//timeLastFrameStarted			   = timeThisFrameStarted;
	
	BeginFrame();                        // all engine system and not game systems

	while( m_timer->CheckAndDecrement() )
	{
		Update( FRAME_RATE );
	}
	
	Render();
	EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::BeginFrame()
{
	// all engine things that must begin at the beginning of each frame and not the game
	Clock::BeginFrame();
//	g_theJobSystem->BeginFrame();
	g_theEventSystem->BeginFrame();
	g_theWindow->BeginFrame();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_theAudioSystem->BeginFrame();
	g_theParticleSystem2D->BeginFrame();
	//g_theRenderer->BeginCamera( g_theGame->m_worldCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Update( float deltaSeconds )
{
	UpdateFromKeyboard();

	if ( m_isPaused )								{ deltaSeconds = 0.f; }
	else if ( m_isSloMo == true )					{ deltaSeconds /= 50.f; }
	if ( m_isSpeedMo )								{ deltaSeconds = deltaSeconds * 4.0f; }

	if ( m_isPaused && g_theGamplayUISystem->GetGameState() == HUD_STATE )
	{
		g_theGamplayUISystem->SetGameState( PAUSE_STATE );
	}
	else if( !m_isPaused && g_theGamplayUISystem->GetGameState() == PAUSE_STATE )
	{
		g_theGamplayUISystem->SetGameState( HUD_STATE );
	}

	
	//g_theGame->Update( deltaSeconds );
	g_theGamplayUISystem->Update( deltaSeconds );
	
	g_theParticleSystem2D->Update( deltaSeconds );

	if ( g_theDevConsole->IsOpen() )
	{
		g_theDevConsole->Update( deltaSeconds );
	}
	
	g_theInput->EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
	//g_theGame->Render();
	g_theGamplayUISystem->Render();
	
	if ( g_theDevConsole->IsOpen() )
	{
		g_theDevConsole->Render( *g_theRenderer , *g_theDevConsole->GetDevConsoleCamera() , 14.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	if ( g_theGamplayUISystem->GetGameState() == HUD_STATE )
	{
		//g_theGame->EndFrame();
		g_theGame->GarbageCollection();
	}
	// all engine things that must end at the end of the frame and not the game
/*	g_currentManager->EndFrame();*/
	g_theParticleSystem2D->EndFrame();
	g_theAudioSystem->EndFrame();
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
//	g_theJobSystem->EndFrame();
	Clock::EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	delete g_theGame;
	g_theGame			= nullptr;

	g_theParticleSystem2D->Shutdown();
	
	g_theAudioSystem->Shutdown();
	// 	g_thePhysicsSystem->Shutdown();
	// 	g_currentManager->Shutdown();
	g_theDevConsole->Shutdown();
	g_theRenderer->Shutdown();
	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	g_theEventSystem->Shutdown();
//	g_theJobSystem->Shutdown();
	Clock::Shutdown();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TheApp::HandleQuitRequested()
{
//	g_theJobSystem->HandleQuitRequested();
	
	m_isQuitting			= true;
	return m_isQuitting;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::UpdateFromKeyboard()
{
	if ( g_theInput->IsKeyHeldDown( 'T' ) )										{ m_isSloMo = true; }
	else																		{ m_isSloMo = false; }
	
	if ( g_theInput->IsKeyHeldDown( 'Y' ) )								{ m_isSpeedMo = true; }
	else																		{ m_isSpeedMo = false; }
		
	if ( g_theGamplayUISystem->GetGameState() == HUD_STATE &&
	   ( g_theInput->GetButtonState( 'P' ).WasJustPressed() || g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() ) ) 
	{ 
		m_isPaused = true; 
		g_theGamplayUISystem->SetGameState( PAUSE_STATE );
		
	}
	else if( g_theGamplayUISystem->GetGameState() == PAUSE_STATE &&
		( g_theInput->GetButtonState( 'P' ).WasJustPressed() || g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() ) )
	{
		m_isPaused = false;	
		g_theGamplayUISystem->SetGameState( HUD_STATE );
		g_theInput->PushCursorSettings( CursorSettings( RELATIVE_MODE , MOUSE_IS_WINDOWLOCKED , false ) );
	}

	if( g_theInput->WasKeyJustPressed( KEY_F1 ) )
	{
		g_theGamplayUISystem->m_UIDebugDraw = !g_theGamplayUISystem->m_UIDebugDraw;
	}
	
	if ( g_theInput->GetButtonState( KEY_F8 ).WasJustPressed() ) 
	{
		delete g_theGame;
		g_theGame = nullptr;
		g_theGame = new Game();
		g_theGame->PostGameConstructDataOnce();
		g_theGame->PostGameConstruct();
	}
	
	if ( g_theInput->GetButtonState( KEY_TILDE ).WasJustPressed() )
	{
		g_theDevConsole->ToggleVisibility();
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------
