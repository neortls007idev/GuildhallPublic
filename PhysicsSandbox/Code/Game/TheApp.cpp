#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"


#include "Engine/Core/DebugRender.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Timer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern	InputSystem*					g_theInput;		
extern	RenderContext*					g_theRenderer;	
extern	AudioSystem*					g_theAudioSystem;
extern	DevConsole*						g_theDevConsole;	
extern	Physics2D*						g_thePhysicsSystem;
extern	BitmapFont*						g_bitmapFont;
extern	DebugRenderObjectsManager*		g_currentManager;
extern	NamedStrings					g_gameConfigBlackboard;
		TheApp*							g_theApp			= nullptr;
		Game*							g_theGame			= nullptr;

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

	delete g_thePhysicsSystem;
	g_thePhysicsSystem = nullptr;

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
	AddDebugRenderDevConsoleCommands( g_theDevConsole );

	if ( g_currentManager == nullptr )
	{
		// instantiating a default DRO_Manager
		g_currentManager = new DebugRenderObjectsManager();
	}
	g_currentManager->Startup();
	
	if ( g_thePhysicsSystem == nullptr )
	{
		g_thePhysicsSystem = new Physics2D();
	}
	g_thePhysicsSystem->Startup();
	
	if ( g_theAudioSystem == nullptr )
	{
		g_theAudioSystem = new AudioSystem();
	}
	g_theAudioSystem->Startup();

	if ( g_theGame == nullptr )
	{
		g_theGame = new Game();
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::RunFrame()
{
// 	static double timeLastFrameStarted = GetCurrentTimeSeconds();
// 	double        timeThisFrameStarted = GetCurrentTimeSeconds();
// 	double		  deltaSeconds		   = timeThisFrameStarted - timeLastFrameStarted;
// 	timeLastFrameStarted			   = timeThisFrameStarted;
	
	BeginFrame();                        // all engine system and not game systems
	Update( ( float ) Clock::g_theMasterClock.GetLastDeltaSeconds() );	
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
	g_theInput->SetCursorMode( ABSOLUTE_MODE );
	g_theRenderer->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_currentManager->BeginFrame();
	
	if ( m_isPaused )
	{
		g_thePhysicsSystem->s_clock->Pause();
	}
	else
	{
		g_thePhysicsSystem->s_clock->Resume();
	}
	g_thePhysicsSystem->BeginFrame();
	g_theAudioSystem->BeginFrame();
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Update( float deltaSeconds )
{

	g_theInput->Update( deltaSeconds );
	UpdateFromKeyboard();

	g_currentManager->Update( deltaSeconds );
	g_thePhysicsSystem->Update();

	if ( g_theDevConsole->IsOpen() )
	{
		g_theDevConsole->Update( deltaSeconds );
	}

	if ( m_isPaused ) { deltaSeconds = 0; }
	else if ( m_isSloMo == true ) { deltaSeconds /= 10.f; }

	if ( m_isSpeedMo ) { deltaSeconds = deltaSeconds * 4.0f; }
	
	g_theGame->Update( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
		g_theGame->Render();

		if ( g_theDevConsole->IsOpen() )
		{
			g_theDevConsole->Render( *g_theRenderer , *g_theDevConsole->GetDevConsoleCamera() , 15.f );
		}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	g_theGame->EndFrame();
	
	g_theAudioSystem->EndFrame();
	g_thePhysicsSystem->EndFrame();
	g_currentManager->EndFrame();
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
	// TODO :- Create this function g_theWindow->EndFrame();
	g_theEventSystem->EndFrame();
	Clock::EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	g_theAudioSystem->Shutdown();
	g_thePhysicsSystem->Shutdown();
	g_currentManager->Shutdown();
	g_theDevConsole->Shutdown();
	g_theRenderer->Shutdown();
	g_theInput->Shutdown();
	// TODO :- write me g_theWindow->Shutdown();
	g_theEventSystem->Shutdown();
	Clock::Shutdown();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::UpdateFromKeyboard()
{
	if ( g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() && !g_theGame->m_isDrawModeActive ) { g_theWindow->HandleQuitRequested(); }

	if ( g_theInput->WasKeyJustPressed( KEY_TILDE ) )
	{
		g_theDevConsole->ToggleVisibility();
	}

	if ( g_theDevConsole->IsOpen() )
	{
		return;
	}
	if ( g_theInput->GetButtonState( 'T' ).IsPressed() ) { m_isSloMo = true; }
	else { m_isSloMo = false; }
	
	if ( g_theInput->GetButtonState( 'Y' ).IsPressed() ) { m_isSpeedMo = true; }
	else { m_isSpeedMo = false; }

	if ( g_theInput->GetButtonState( 'P' ).WasJustPressed() ) 
	{ m_isPaused = !m_isPaused; }
	
	if ( g_theInput->WasKeyJustPressed('8') )
	{
		double currentTimeScale = g_thePhysicsSystem->s_clock->GetScale();
		g_thePhysicsSystem->s_clock->SetScale( currentTimeScale * 0.5 );
	}

	if ( g_theInput->WasKeyJustPressed( '9' ) )
	{
		double currentTimeScale = g_thePhysicsSystem->s_clock->GetScale();
		g_thePhysicsSystem->s_clock->SetScale( currentTimeScale * 2.0 );
	}

	if ( g_theInput->WasKeyJustPressed( '0' ) )
	{
		g_thePhysicsSystem->s_clock->SetScale( 1.0 );
		g_thePhysicsSystem->s_clock->Reset();
		g_thePhysicsSystem->s_timer->Reset();
	}
	
	if ( g_theInput->GetButtonState( KEY_F8 ).WasJustPressed() )
	{
		delete g_theGame;
		g_theGame = nullptr;
		delete g_thePhysicsSystem;
		g_thePhysicsSystem = new Physics2D();
		g_thePhysicsSystem->Startup();
		g_theGame = new Game();
		
	}	
}

//--------------------------------------------------------------------------------------------------------------------------------------------


