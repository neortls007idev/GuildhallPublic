#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/Game.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/GameMultiPlayer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/AuthServer.hpp"
#include "Game/TheApp.hpp"
#include "Game/PlayerClient.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/RemoteServer.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

		TheApp*								g_theApp			= nullptr;
extern	AudioSystem*						g_theAudioSystem;
extern	AuthoritativeServer*				g_theAuthServer;
extern	RemoteServer*						g_theRemoteServer;
extern	BitmapFont*							g_bitmapFont;
extern	DevConsole*							g_theDevConsole;	
extern	InputSystem*						g_theInput;
extern	GameNetworkSystem*					g_theGameNetworkSys;
extern	RenderContext*						g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

#ifdef UNIT_TEST
#undef UNIT_TEST
#endif
	

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::TheApp()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::~TheApp()
{
	delete g_theAuthServer;
	g_theAuthServer = nullptr;

	delete g_theGameNetworkSys;
	g_theGameNetworkSys = nullptr;

	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;

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
		g_bitmapFont = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	}

	if ( g_theDevConsole == nullptr )
	{
		g_theDevConsole = new DevConsole();
	}
	g_theDevConsole->Startup();

	if ( g_theAudioSystem == nullptr )
	{
		g_theAudioSystem = new AudioSystem();
	}
	g_theAudioSystem->Startup();
	
//	TileDefinition::CreateTileDefinitions( "Data/GamePlay/TileDefs.xml" );
//	MapDefinition::CreateMapDefinitions( "Data/GamePlay/MapDefs.xml" );

	if ( g_theAuthServer == nullptr )
	{
		g_theAuthServer = new AuthoritativeServer();
		g_theAuthServer->CreateSinglePlayerGame();
	}
	g_theAuthServer->Startup();
	g_theAuthServer->AddPlayers();
	
	if( m_localclient == nullptr )
	{
		m_localclient = new PlayerClient( 0 );
		m_localclient->Startup();
	}

	g_theAuthServer->AddPlayerClientToServer( m_localclient );
	
	if ( g_theGameNetworkSys == nullptr )
	{
		g_theGameNetworkSys = new GameNetworkSystem();
	}
	g_theGameNetworkSys->Startup();

	AddMultiplayerGameCommandsToConsole();
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
	g_theGameNetworkSys->BeginFrame();

	if( g_theAuthServer != nullptr )
	{
		g_theAuthServer->BeginFrame();
	}
	
	if( g_theRemoteServer != nullptr )
	{
		g_theRemoteServer->BeginFrame();
	}

	if( m_localclient != nullptr )
	{
		m_localclient->BeginFrame();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Update( float deltaSeconds )
{
	UpdateFromKeyboard();
	
	if ( g_theAuthServer != nullptr )
	{
		g_theAuthServer->Update( deltaSeconds );
	}
	
	if( g_theRemoteServer != nullptr )
	{
		g_theRemoteServer->Update( deltaSeconds );
	}

	if( m_localclient != nullptr )
	{
		m_localclient->Update( deltaSeconds );
	}
	
	if ( g_theDevConsole->IsOpen() )
	{
		g_theDevConsole->Update( deltaSeconds );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
	if( m_localclient != nullptr )
	{
		m_localclient->Render();
	}
	if ( g_theDevConsole->IsOpen() )
	{
		g_theDevConsole->Render( *g_theRenderer , *g_theDevConsole->GetDevConsoleCamera() , 14.f );
	}
	//g_theGame->RenderUI();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	// all engine things that must end at the end of the frame and not the game
/*	g_currentManager->EndFrame();*/
	StartMultiplayerServerCheck();
	ConnectToMultiplayerServerCheck();

	if( g_theRemoteServer != nullptr )
	{
		g_theRemoteServer->EndFrame();
	}
		
	if( g_theAuthServer != nullptr )
	{
		g_theAuthServer->EndFrame();
	}

	if( m_localclient != nullptr )
	{
		m_localclient->EndFrame();
	}

	g_theGameNetworkSys->EndFrame();
	g_theAudioSystem->EndFrame();
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
	Clock::EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::AddMultiplayerGameCommandsToConsole()
{
	EventArgs consoleArgs;
	g_theDevConsole->CreateCommand( "StartMultiplayerServer" , "StartMutiplayerServer port=<port number>" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "StartMultiplayerServer" , StartMultiplayerServer );

	g_theDevConsole->CreateCommand( "ConnectToMultiplayerServer" , "ConnectToMultiplayerServer ip=<IPv4 address> |port=<port number> " , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ConnectToMultiplayerServer" , ConnectToMultiplayerServer );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	if( m_localclient != nullptr )
	{
		m_localclient->Shutdown();
	}

	if( g_theRemoteServer != nullptr )
	{
		g_theRemoteServer->Shutdown();
	}

	if( g_theAuthServer != nullptr )
	{
		g_theAuthServer->Shutdown();
	}
	
	g_theGameNetworkSys->Shutdown();
	g_theAudioSystem->Shutdown();
	g_theDevConsole->Shutdown();
	g_theRenderer->Shutdown();
	g_theInput->Shutdown();
	g_theWindow->Shutdown();
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
 	if ( g_theInput->WasKeyJustPressed( KEY_TILDE ) )
 	{
 		g_theDevConsole->ToggleVisibility();
 	}

	if ( g_theDevConsole->IsOpen() && g_theInput->WasKeyJustPressed( KEY_ESC ) )
	{
 		g_theDevConsole->ToggleVisibility();
		return;
	}
	
	if ( g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() )
	{
		HandleQuitRequested();
		g_theWindow->HandleQuitRequested();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool TheApp::StartMultiplayerServer( EventArgs& args )
{
//	if( !g_theApp->m_hasServerJustStarted )
//	{
//		g_theApp->m_serverArgs = args;
//		g_theApp->m_hasServerJustStarted = true;
//		return false;
//	}

	LOG_SYSMESSAGE( "Stopping Current Game" );
	int port = args.GetValue( "port" , 48000 );
	
	if( g_theAuthServer != nullptr )
	{
		SAFE_RELEASE_POINTER( g_theAuthServer->m_singlePlayerGame );
		g_theAuthServer->CreateMultiPlayerGame();
		g_theGameNetworkSys->StartTCPServer( args );
		g_theAuthServer->AddPlayers();
		g_theWindow->SetTitle( "SD3 : A3 - Networked Multiplayer BATTLE CITY - Server" );
	}
	else
	{
		g_theAuthServer->CreateMultiPlayerGame();
		g_theGameNetworkSys->StartTCPServer( args );
		g_theAuthServer->AddPlayers();
		g_theWindow->SetTitle( "SD3 : A3 - Networked Multiplayer BATTLE CITY - Server" );
	}

	delete g_theApp->m_localclient;
	g_theApp->m_localclient = nullptr;

	if ( g_theApp->m_localclient == nullptr )
	{
		g_theApp->m_localclient = new PlayerClient( 0 );
		g_theApp->m_localclient->Startup();
	}

	g_theAuthServer->AddPlayerClientToServer( g_theApp->m_localclient );
	
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	
STATIC bool TheApp::ConnectToMultiplayerServer( EventArgs& args )
{
//	if ( !g_theApp->m_hasClientJustConnected )
//	{
//		g_theApp->m_clientArgs = args;
//		g_theApp->m_hasClientJustConnected = true;
//		return false;
//	}

	LOG_SYSMESSAGE( "Stopping Current Game" );
	std::string ipAddr = args.GetValue( "ipaddr" , "127.0.0.1" );
	int port = args.GetValue( "port" , 48000 );

	if ( g_theAuthServer != nullptr )
	{
		SAFE_RELEASE_POINTER( g_theAuthServer->m_singlePlayerGame );
		SAFE_RELEASE_POINTER( g_theAuthServer );

		if( g_theRemoteServer == nullptr )
		{
			g_theRemoteServer = new RemoteServer();
		}
		g_theRemoteServer->Startup();
				
		if( g_theGameNetworkSys->ConnectToServer( args ) )
		{
			g_theWindow->SetTitle( "SD3 : A3 - Networked Multiplayer BATTLE CITY - Client" );
			g_theRemoteServer->RequestAddPlayerEntityandUDPConnection( args );
		}
	}

	delete g_theApp->m_localclient;
	g_theApp->m_localclient = nullptr;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TheApp::StartMultiplayerServerCheck()
{
	if( m_hasServerJustStarted )
	{
		StartMultiplayerServer( m_serverArgs );
	}
	m_hasServerJustStarted = false;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TheApp::ConnectToMultiplayerServerCheck()
{
	if ( m_hasClientJustConnected )
	{
		ConnectToMultiplayerServer( m_clientArgs );
	}
	m_hasClientJustConnected = false;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	