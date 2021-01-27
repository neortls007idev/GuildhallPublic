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
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem*		g_theInput;
extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;		
extern DevConsole*		g_theDevConsole;			
	   TheApp*			g_theApp			= nullptr;
	   Game*			g_theGame			= nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::TheApp()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::~TheApp()
{
	delete g_theGame;
	g_theGame = nullptr;

	//delete g_theAudioSystem;
	//g_theAudioSystem = nullptr;
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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Update( float deltaSeconds )
{
		UpdateFromKeyboard();
	
		if ( m_isPaused )							{ deltaSeconds = 0.f; }
		else if ( m_isSloMo == true )				{ deltaSeconds /= 10.f; }

		m_theGame->Update( deltaSeconds );
		g_theInput->EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
		g_theRenderer->BeginCamera( m_theGame->m_worldCamera );
		g_theRenderer->SetBlendMode( ALPHA );
		m_theGame->Render();
		g_theRenderer->EndCamera( m_theGame->m_worldCamera );

		m_theGame->RenderUI();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	// all engine things that must end at the end of the frame and not the game
/*	g_currentManager->EndFrame();*/
/*	g_theAudioSystem->EndFrame();*/
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();

	Clock::EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	delete m_theGame;
	m_theGame = nullptr;

	//g_theAudioSystem->Shutdown();
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


bool TheApp::HandleQuitRequested()
{
	m_isQuitting = true;
	return m_isQuitting;
}

void TheApp::UpdateFromKeyboard()
{
	if ( g_theInput->GetButtonState( 'T' ).IsPressed() )						{ m_isSloMo = true; }
	else																			{ m_isSloMo = false; }

	if ( g_theInput->GetButtonState( 'P' ).WasJustPressed() )					{ m_isPaused = true; }
	else																			{ m_isPaused = false; }

	if ( g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() )
	{
		HandleQuitRequested();
		g_theWindow->HandleQuitRequested();
	}
	
	if ( g_theInput->GetButtonState( KEY_F8 ).WasJustPressed() )
	{
		delete m_theGame;
		m_theGame = nullptr;
		m_theGame = new Game();
	}
}

/*

//-----------------------------------------------------------------------------------------------------------------
//	REMOVE AT ONCE - ASAP ONCE THE INPUT SYSTEM
//-----------------------------------------------------------------------------------------------------------------


bool TheApp::HandleKeyReleased( unsigned char keyCode )
{
	
	if (keyCode == 'T')
	{
		m_isSloMo = false;
		
	}
	if (keyCode == KEY_SPACE)
	{
		m_theGame->m_wasSpacekeyJustReleased = true;
	}
	
	m_keyPressed[keyCode] = false;
	return(m_keyPressed[keyCode]);
}

bool TheApp::HandleKeyReleased()
{

	if (!g_theInput->IsKeyPressed('T'))
	{
		m_isSloMo = false;

	}
	if (g_theInput->IsKeyPressed(KEY_SPACE))
	{
		g_theInput->HandleKeyReleased(KEY_SPACE);
		m_theGame->m_wasSpacekeyJustReleased = true;
	}
	//g_theInput->HandleKeyReleased();
	return true;
}


bool TheApp::HandleKeyPressed()
{
	if (g_theInput->HandleKeyPressed('T'))
	{
		m_isSloMo = true;

	}

	if (g_theInput->HandleKeyPressed('P'))
	{
		//if (m_keyPressed['P'] == false)
		//{
			m_isPaused = !m_isPaused;
		//}

	}
	if (g_theInput->HandleKeyPressed(KEY_SPACE))
	{
		m_theGame->m_wasSpacekeyJustReleased = false;
	}

	if (g_theInput->HandleKeyPressed(KEY_ESC)) // 0x1B is the VK_ESCAPE macro defined in windows.h and used here directly in hexadecimal.
	{
		HandleQuitRequested();
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	if (g_theInput->HandleKeyPressed(KEY_F8))
	{
		delete m_theGame;
		m_theGame = nullptr;
		m_theGame = new Game();
	}
	//g_theInput->HandleKeyPressed()
	return false;
}

bool TheApp::HandleKeyPressed( unsigned char keyCode )
{
	if (keyCode == 'T' )
	{
		m_isSloMo = true;
	
	}

	if ( keyCode == 'P' )
	{
		if ( m_keyPressed['P'] == false )
		{
			m_isPaused = !m_isPaused;
		}
		
	}
	if (keyCode == KEY_SPACE)
	{
		m_theGame->m_wasSpacekeyJustReleased = false;
	}
  	
	if( keyCode == KEY_ESC ) // 0x1B is the VK_ESCAPE macro defined in windows.h and used here directly in hexadecimal.
	{
		HandleQuitRequested();
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	if ( keyCode == KEY_F8 )
	{
		delete m_theGame;
		m_theGame = nullptr;
		m_theGame = new Game();
	}

	m_keyPressed[keyCode] = true;
	

	return( m_keyPressed[keyCode] );
}*/