#include "Game/Server.hpp"

#include "Engine/Core/StdExtensions.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/GameMultiPlayer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Networking/GameUDPData.hpp"
#include "Networking/GameNetworkSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
extern	GameNetworkSystem* g_theGameNetworkSys;
Server* g_theServer = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

Server::Server()				
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Server::~Server()
{
	SAFE_RELEASE_POINTER( m_singlePlayerGame );
	SAFE_RELEASE_POINTER( m_multiPlayerGame );
	
	for ( auto index : m_clients )
	{
		index = nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Startup()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Update( float deltaSeconds )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::EndFrame()
{
//	g_theGameNetworkSys->SendACKForRecievedMessages();
//	g_theGameNetworkSys->RecievedACKForSentMessages();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::CreateSinglePlayerGame()
{
	m_singlePlayerGame = new GameSinglePlayer();
	m_gameType = SINGLE_PLAYER;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::CreateMultiPlayerGame()
{
	m_multiPlayerGame = new GameMultiplayer();
	m_gameType = MULTIPLAYER;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::AddPlayerClientToServer( Client* client )
{
	EmplaceBackAtEmptySpace( m_clients , client );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Server::RemovePlayerClientFromServer( Client* client )
{
	for( int index = 0 ; index < m_clients.size(); index++ )
	{
		if( m_clients[ index ] == nullptr )
		{
			continue;
		}

		if( m_clients[ index ] == client )
		{
			m_clients[ index ] = nullptr;
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::AddRemoteNewRemotePlayer()
{
	Player* secondplayer = m_multiPlayerGame->CreateAndAddPlayerAtpositionAndOrientation( Vec2( 46.5 , 1.5f ) , 135.f );
	secondplayer->UpdatePlayerColor( MAGENTA );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game* Server::GetGame()
{
	if ( m_gameType == SINGLE_PLAYER )
	{
		return m_singlePlayerGame;
	}
	else if ( m_gameType == MULTIPLAYER )
	{
		return m_multiPlayerGame;
	}
	else
	{
		__debugbreak();
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::ParseReceivedMessages( std::vector< std::string > messageBuffer , bool isAuthserver )
{
	if ( m_gameType == MULTIPLAYER && m_isRemoteClientConnectionComplete )
	{
		for ( int index = 0; index < messageBuffer.size() ; index++ )
		{
			Game* currentGame = GetGame();

			if ( messageBuffer[ index ] != "" )
			{
				Strings data = SplitStringAtGivenDelimiter( messageBuffer[ index ] , '|' );
				int identifier = atoi( data[ D_UNIQUE_ID ].c_str() );
				//LOG_SYSMESSAGE( " UniqueKey = %d" , identifier );
				//if ( data.size() != D_NUM_TOTAL )
				//{
				//	index = "";
				//	continue;
				//}
				if ( identifier != m_uniqueKey )
				{
					continue;
				}

				int frameID = atoi( data[ D_FRAME_ID ].c_str() );
				
				//if ( isAuthserver && ( frameID < m_frameID ) )
				//{
				//	continue;
				//}

				EntityType entityType = ( EntityType ) atoi( data[ D_ENTITY_TYPE ].c_str() );
				int entityID = atoi( data[ D_ENTITY_ID ].c_str() );
				Map* curMap = GetGame()->m_world->m_currentMap;

				if ( !curMap->IsEntityOfTypeWithIDPresent( entityType , entityID ) )
				{
					if ( entityType == GOOD_BULLET_ENTITY )
					{
						curMap->SpawnNewEntity( GOOD_BULLET_ENTITY , FACTION_ALLY , Vec2::SetFromText( data[ D_POSITION ].c_str() ) , StringConvertToValue( data[ D_ORIENTATION_DEG ].c_str() , 0.f ) );
					}
					else if ( entityType == EVIL_BULLET_ENTITY )
					{
						curMap->SpawnNewEntity( EVIL_BULLET_ENTITY , FACTION_ENEMY , Vec2::SetFromText( data[ D_POSITION ].c_str() ) , StringConvertToValue( data[ D_ORIENTATION_DEG ].c_str() , 0.f ) );
					}
					else if ( entityType == EXPLOSION_ENTITY )
					{
						curMap->SpawnNewEntity( EXPLOSION_ENTITY , FACTION_NEUTRAL , Vec2::SetFromText( data[ D_POSITION ].c_str() ) , StringConvertToValue( data[ D_ORIENTATION_DEG ].c_str() , 0.f ) );
					}
				}
				Entitylist& entityList = currentGame->m_world->m_currentMap->m_entityListsByType[ entityType ];

				for ( int entityIndex = 0; entityIndex < entityList.size(); entityIndex++ )
				{
					if( entityList[ entityIndex ] == nullptr )
					{
						continue;
					}
					if ( entityList[ entityIndex ]->m_entityID == entityID )
					{
						entityList[ entityIndex ]->m_position = entityList[ entityIndex ]->m_position.SetFromText( data[ D_POSITION ].c_str() );
						entityList[ entityIndex ]->m_orientationDegrees = StringConvertToValue( data[ D_ORIENTATION_DEG ].c_str() , entityList[ entityIndex ]->m_orientationDegrees );
						entityList[ entityIndex ]->m_faction = ( Faction ) StringConvertToValue( data[ D_FACTION ].c_str() , ( int ) entityList[ entityIndex ]->m_faction );
						entityList[ entityIndex ]->m_health = StringConvertToValue( data[ D_HEALTH ].c_str() , entityList[ entityIndex ]->m_health );
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------