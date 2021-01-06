#include "Game/RemoteClient.hpp"
#include "Game/Game.hpp"
#include "Game/RemoteServer.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
extern	RemoteServer*	g_theRemoteServer;
extern	GameNetworkSystem*	g_theGameNetworkSys;

//--------------------------------------------------------------------------------------------------------------------------------------------

RemoteClient::RemoteClient( int PlayerID ) : Client()
{
	m_remotePlayerID = PlayerID;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

RemoteClient::~RemoteClient()
{
	m_game = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteClient::Startup()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteClient::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteClient::Update( float deltaSeconds )
{
	if ( m_game != nullptr )
	{
		if ( ( m_game->m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ].size() > m_remotePlayerID ) &&
			( nullptr != m_game->m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ m_remotePlayerID ] ) )
		{
			m_game->m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ m_remotePlayerID ]->Update( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteClient::BeginFrame()
{
	if ( m_game != nullptr )
	{
		m_game->Render();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteClient::EndFrame()
{
	m_game = g_theRemoteServer->GetGame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------