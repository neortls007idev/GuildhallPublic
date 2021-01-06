#include "Game/PlayerClient.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Game/Server.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/AuthServer.hpp"
#include "Game/RemoteServer.hpp"
#include "Game/Player.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole*							g_theDevConsole;	
extern	AudioSystem*						g_theAudioSystem;
extern	InputSystem*						g_theInput;
extern	RenderContext*						g_theRenderer;
extern	Window*								g_theWindow;
extern	BitmapFont*							g_bitmapFont;
extern	AuthoritativeServer*				g_theAuthServer;
extern	RemoteServer*						g_theRemoteServer;
extern	GameNetworkSystem*					g_theGameNetworkSys;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
PlayerClient::PlayerClient( int playerID ) : Client()
{
	m_localPlayerID = playerID;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

PlayerClient::~PlayerClient()
{
	m_game = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::Startup()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::Update( float deltaSeconds )
{
	if( m_game != nullptr )
	{
		if( ( m_game->m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ].size() > m_localPlayerID ) &&
			( nullptr != m_game->m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ m_localPlayerID ] ) )
		{
			Player* player = ( Player* ) m_game->m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ m_localPlayerID ];
			player->Update( deltaSeconds );

			if( player->m_didPlayerMoveLastFrame )
			{
				EventArgs ClientUpdateArgs;
				std::string clientDataAsString = ToString( m_uniqueKey ) + "|" +
												 ToString( m_frameID ) + "|" +
												 ToString( PLAYERTANK_ENTITY ) + "|" + 
												 ToString( player->m_entityID ) + "|" +
												 ToString( player->m_position ) + "|" +
												 ToString( player->m_orientationDegrees ) + "|" +
												 ToString( player->m_faction ) + "|" +
												 ToString( player->m_health );
				
				ClientUpdateArgs.SetValue( "msg" , clientDataAsString.c_str() );
				g_theGameNetworkSys->SendUDPMessage( ClientUpdateArgs );
			}

			if( player->m_lastSpawnedBullet != nullptr )
			{
				EventArgs ClientUpdateArgs;
				std::string clientDataAsString = ToString( m_uniqueKey ) + "|" +
												 ToString( m_frameID ) + "|" +
												 ToString( GOOD_BULLET_ENTITY ) + "|" + 
												 ToString( player->m_lastSpawnedBullet->m_entityID ) + "|" +
												 ToString( player->m_lastSpawnedBullet->m_position ) + "|" +
												 ToString( player->m_lastSpawnedBullet->m_orientationDegrees ) + "|" +
												 ToString( player->m_lastSpawnedBullet->m_faction ) + "|" +
												 ToString( player->m_lastSpawnedBullet->m_health );
				
				ClientUpdateArgs.SetValue( "msg" , clientDataAsString.c_str() );
				g_theGameNetworkSys->SendUDPMessage( ClientUpdateArgs );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::Render()
{
	if ( m_game != nullptr )
	{
		m_game->Render();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::BeginFrame()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::EndFrame()
{
	if( g_theAuthServer != nullptr )
	{
		m_game = g_theAuthServer->GetGame();
		m_uniqueKey = g_theAuthServer->m_uniqueKey;
		m_frameID = g_theAuthServer->GetFrameID();
	}
	else if( m_game == nullptr && g_theRemoteServer != nullptr )
	{
		m_game = g_theRemoteServer->GetGame();
		m_uniqueKey = g_theRemoteServer->m_uniqueKey;
		m_frameID = g_theRemoteServer->GetFrameID();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------