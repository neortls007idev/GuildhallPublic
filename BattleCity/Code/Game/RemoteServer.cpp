#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameMultiplayer.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"
#include "Game/PlayerClient.hpp"
#include "Game/RemoteServer.hpp"
#include "Game/Server.hpp"
#include "Game/TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

		RemoteServer*		g_theRemoteServer = nullptr;

extern	GameNetworkSystem*	g_theGameNetworkSys;
extern	TheApp*				g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

RemoteServer::RemoteServer()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

RemoteServer::~RemoteServer()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::Startup()
{
	CreateMultiPlayerGame();
	m_multiPlayerGame->Startup();
	m_multiPlayerGame->CreateAndAddPlayerAtpositionAndOrientation();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::Shutdown()
{
	EventArgs args;
	args.SetValue( "bindPort" , ToString( m_udpListenPort ) );

	if ( g_theGameNetworkSys != nullptr )
	{
		g_theGameNetworkSys->CloseUDPPort( args );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( m_gameType == MULTIPLAYER )
	{
		m_multiPlayerGame->m_world->m_currentMap->UpdateEntityListOfType( deltaSeconds , EVIL_BULLET_ENTITY );
		m_multiPlayerGame->m_world->m_currentMap->UpdateEntityListOfType( deltaSeconds , GOOD_BULLET_ENTITY );
		m_multiPlayerGame->m_world->m_currentMap->UpdateEntityListOfType( deltaSeconds , EXPLOSION_ENTITY );
		m_multiPlayerGame->m_world->m_currentMap->IsGarbageUpdate();
		m_multiPlayerGame->m_world->m_currentMap->CheckCollisions();
		m_multiPlayerGame->m_world->m_currentMap->GarbageCollection();
		m_frameID++;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::BeginFrame()
{
	if ( m_gameType == MULTIPLAYER )
	{
		for ( auto index : g_theGameNetworkSys->m_recievedTCPServerMesageBuffer )
		{
			if( m_isRemoteClientConnectionComplete )
			{
				break;
			}
			if( index != "" )
			{
				Strings data = SplitStringAtGivenDelimiter( index );
				
				if ( data.size() > 0 && _stricmp( data[ 0 ].c_str() , "UDPServerStarted" ) == 0 )
				{
					m_uniqueKey = atoi( data[ 1 ].c_str() );
					LOG_SYSMESSAGE( " UniqueKey = %d" , m_uniqueKey );
					m_hostIPaddr = data[ 2 ];/*g_theGameNetworkSys->GetAddress( g_theGameNetworkSys->m_linkSocket );*/
					m_udpSendToPort = atoi( data[ 3 ].c_str() );
					m_udpListenPort = atoi( data[ 4 ].c_str() );
					
					m_playerID = atoi( data[ 5 ].c_str() );
					
					AddRemoteNewRemotePlayer();

					if ( g_theApp->m_localclient == nullptr )
					{
						g_theApp->m_localclient = new PlayerClient( m_playerID );
						g_theApp->m_localclient->Startup();
					}

					AddPlayerClientToServer( g_theApp->m_localclient );

					EventArgs UDPArgs;

					UDPArgs.SetValue( "bindPort" , ToString( m_udpListenPort ) );
					UDPArgs.SetValue( "sendPort" , ToString( m_udpSendToPort ) );
					UDPArgs.SetValue( "host" , m_hostIPaddr.c_str() );
					
					if ( g_theGameNetworkSys->OpenUDPPort( UDPArgs ) )
					{
						EventArgs args;
						//g_theNetworkSys->DisconnectFromServer( args );
						m_isRemoteClientConnectionComplete = true;
					}
				}
			}
		}

		// If request to start UDP Server was just recieved ? then start UDP server and let the client know about it.
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::EndFrame()
{
	//ParseAndUpdateEntities();
	g_theGameNetworkSys->m_recieveBufferMutex.lock();
	ParseReceivedMessages( g_theGameNetworkSys->m_recievedUDPMesageBuffer , false );
	g_theGameNetworkSys->m_recieveBufferMutex.unlock();
	//Server::EndFrame();
	if( m_multiPlayerGame != nullptr )
	{
		m_multiPlayerGame->LateUpdate();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::ParseAndUpdateEntities()
{
	if ( m_gameType == MULTIPLAYER && m_isRemoteClientConnectionComplete )
	{
		Game* currentGame = GetGame();
		
		if ( currentGame == nullptr )
		{
			return;
		}

		for ( auto index : g_theGameNetworkSys->m_recievedUDPMesageBuffer )
		{
			if ( index != "" )
			{
				Strings data = SplitStringAtGivenDelimiter( index , '|' );
				if ( data.size() != 7 )
				{
					continue;
				}
				int identifier = atoi( data[ 0 ].c_str() );
				//LOG_SYSMESSAGE( " UniqueKey = %d" , identifier );
				if ( identifier != m_uniqueKey )
				{
					continue;
				}
				EntityType entityType = ( EntityType ) atoi( data[ 1 ].c_str() );
				int entityID = atoi( data[ 2 ].c_str() );
				Map* curMap = GetGame()->m_world->m_currentMap;
				if ( !curMap->IsEntityOfTypeWithIDPresent( entityType , entityID ) )
				{
					if( entityType == GOOD_BULLET_ENTITY )
					{
						curMap->SpawnNewEntity( GOOD_BULLET_ENTITY , FACTION_ALLY , Vec2::ZERO , 0.f );
					}
					else if ( entityType == EVIL_BULLET_ENTITY )
					{
						curMap->SpawnNewEntity( EVIL_BULLET_ENTITY , FACTION_ENEMY , Vec2::ZERO , 0.f );
					}
					else if ( entityType == EXPLOSION_ENTITY )
					{
						curMap->SpawnNewEntity( EVIL_BULLET_ENTITY , FACTION_NEUTRAL , Vec2::ZERO , 0.f );
					}
				}
				Entitylist& entityList = currentGame->m_world->m_currentMap->m_entityListsByType[ entityType ];

				for ( int entityIndex = 0; entityIndex < entityList.size(); entityIndex++ )
				{
					if ( entityList[ entityIndex ] == nullptr )
					{
						continue;
					}

					if ( entityList[ entityIndex ]->m_entityID == entityID )
					{
						entityList[ entityIndex ]->m_position = entityList[ entityIndex ]->m_position.SetFromText( data[ 3 ].c_str() );
						entityList[ entityIndex ]->m_orientationDegrees = StringConvertToValue( data[ 4 ].c_str() , entityList[ entityIndex ]->m_orientationDegrees );
						entityList[ entityIndex ]->m_faction = ( Faction ) StringConvertToValue( data[ 5 ].c_str() , ( int ) entityList[ entityIndex ]->m_faction );
						entityList[ entityIndex ]->m_health = StringConvertToValue( data[ 6 ].c_str() , entityList[ entityIndex ]->m_health );
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//Game* RemoteServer::GetGame()
//{
//	return m_multiPlayerGame;
//}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool RemoteServer::RequestAddPlayerEntityandUDPConnection( EventArgs args )
{
	m_hostIPaddr = args.GetValue( "ipaddr" , "127.0.0.1" );
	EventArgs newArgs;

	std::string message = "StartUDPServer," + m_hostIPaddr;
	newArgs.SetValue( "msg" , message );
	
	if ( g_theGameNetworkSys != nullptr )
	{
		return g_theGameNetworkSys->SendMessageToServer( newArgs );
	}

	return false;
}
