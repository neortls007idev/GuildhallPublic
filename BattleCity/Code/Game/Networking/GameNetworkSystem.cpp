#include "Engine/Core/DevConsole.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"
#include "Game/Networking/GameTCPServer.hpp"
#include "Game/Networking/GameTCPClient.hpp"
#include "Game/Networking/GameUDPListner.hpp"
#include <WinSock2.h>
#include "ws2tcpip.h"
#include <array>
#pragma comment(lib, "Ws2_32.lib")

//--------------------------------------------------------------------------------------------------------------------------------------------
		GameNetworkSystem*		g_theGameNetworkSys;
extern	DevConsole*				g_theDevConsole;
static	bool					areDevConsoleCommandsAdded = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::m_wasMessageJustSentByServer = false;
STATIC bool GameNetworkSystem::m_wasMessageJustSentByClient = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

//--------------------------------------------------------------------------------------------------------------------------------------------

GameNetworkSystem::GameNetworkSystem() :
								m_isListening( false )
								
{
	m_linkSocket = INVALID_SOCKET;
	m_TCPclient = new GameTCPClient( INVALID_SOCKET );
	if ( !areDevConsoleCommandsAdded )
	{
		AddNetowrkingCommandsToConsole();
	}
	m_UDPListner = new GameUDPListner();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameNetworkSystem::~GameNetworkSystem()
{
	m_linkSocket = INVALID_SOCKET;
	m_isListening = false;

	if ( nullptr != m_TCPclient )
	{
		delete m_TCPclient;
		m_TCPclient = nullptr;
	}

	if ( nullptr != m_TCPServer )
	{
		delete m_TCPServer;
		m_TCPServer = nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::Startup()
{
	// Initialize WinSock
	WSAData wsaData;
	WORD wVersion MAKEWORD( 2 , 2 );
	int iResult = WSAStartup( wVersion , &wsaData );

	if( iResult != 0 )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to WSAStartup Failed %i" , WSAGetLastError() );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::Shutdown()
{
	if ( m_UDPListner != nullptr )
	{
		SAFE_RELEASE_POINTER( m_UDPListner );
	}

	int iResult = WSACleanup();

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to WSACleanup Failed %i" , WSAGetLastError() );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::BeginFrame()
{
	if( m_isListening )
	{
		if( ( m_TCPServer != nullptr ) && ( m_TCPServer->m_listenSocket != INVALID_SOCKET ) )
		{
			if( m_linkSocket == INVALID_SOCKET )
			{
				m_linkSocket = m_TCPServer->Accept();

				if( m_linkSocket != INVALID_SOCKET )
				{
					//m_clientAddr = GetAddress( m_linkSocket ) ;
					g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Client Connected from %s" , GetConnectClientAddress( m_linkSocket ).c_str() );
				}
			}
			if( m_linkSocket != INVALID_SOCKET )
			{
				std::array<char , 256> bufferRecieve;
				std::string recievedClientMessage = m_TCPServer->ReceiveClientMessage( m_linkSocket , &bufferRecieve[ 0 ] , ( int ) ( bufferRecieve.size() - 1 ) );
				if( recievedClientMessage != "" )
				{
					m_recievedTCPClientMesageBuffer.emplace_back( recievedClientMessage );
				}
				
				if( m_wasMessageJustSentByServer )
				{
					//std::array<char , 256> buffer;
					m_TCPServer->SendClientMessage( m_linkSocket );
					m_wasMessageJustSentByServer = false;
				}
			}
		}
	}

	if( m_TCPclient != nullptr )
	{
		if( m_TCPclient->m_clientSocket != INVALID_SOCKET )
		{
			std::array<char , 256> bufferRecieve;
			
			std::string recievedServerMessage = m_TCPclient->ReceiveServerMessage( m_TCPclient->m_clientSocket , &bufferRecieve[ 0 ] , ( int ) ( bufferRecieve.size() - 1 ) );
			if ( recievedServerMessage != "" )
			{
				m_recievedTCPServerMesageBuffer.emplace_back( recievedServerMessage );
			}


			if( m_wasMessageJustSentByClient )
			{
				m_TCPclient->SendServerMessage( m_TCPclient->m_clientSocket );
				m_wasMessageJustSentByClient = false;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::EndFrame()
{
	m_recievedTCPClientMesageBuffer.clear();
	m_recievedTCPServerMesageBuffer.clear();
	
	g_theGameNetworkSys->m_recieveBufferMutex.lock();
	m_recievedUDPMesageBuffer.clear();
	m_recievedUDPMesageBuffer.resize( 0 );
	g_theGameNetworkSys->m_recieveBufferMutex.unlock();
	
	m_sentUDPMesageBuffer.clear();
	m_sentUDPMesageBuffer.resize( 0 );
	//SendACKForRecievedMessages();
	//RecievedACKForSentMessages();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::SendACKForRecievedMessages()
{
	g_theGameNetworkSys->m_recieveBufferMutex.lock();
	for ( int index = 0; index < m_recievedUDPMesageBuffer.size(); index++ )
	{
		if( m_recievedUDPMesageBuffer[ index ] != "" )
		{
			std::string curString = m_recievedUDPMesageBuffer[ index ];
			Strings rMsg = SplitStringAtGivenDelimiter( curString , '|' );
			if( rMsg.size() == D_NUM_TOTAL )
			{
				std::string msg = "ACK|" +
				  				   rMsg[ D_FRAME_ID ] + "|" +
								   rMsg[ D_ENTITY_TYPE ] + "|" +
								   rMsg[ D_ENTITY_ID ];
				
				EventArgs ackUpdateArgs;
				ackUpdateArgs.SetValue( "msg" , msg.c_str() );
				g_theGameNetworkSys->SendUDPMessage( ackUpdateArgs );
			}
		}
	}
	g_theGameNetworkSys->m_recieveBufferMutex.unlock();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::RecievedACKForSentMessages()
{
	g_theGameNetworkSys->m_recieveBufferMutex.lock();
	for ( auto index = 0; index < m_recievedUDPMesageBuffer.size(); index++ )
	{
		if ( m_recievedUDPMesageBuffer[ index ] != "" )
		{
			std::string curString = m_recievedUDPMesageBuffer[ index ];
			Strings rMsg = SplitStringAtGivenDelimiter( curString , '|' );
			if ( rMsg.size() == 4 )
			{
				if( rMsg[ 0 ] == "ACK" )
				{
					FindAndEraseSentMessageFromBuffer( rMsg[ 1 ] , rMsg[ 2 ] , rMsg[ 3 ] );
					m_recievedUDPMesageBuffer[ index ] = "";
				}
			}
		}
	}
	g_theGameNetworkSys->m_recieveBufferMutex.unlock();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::FindAndEraseSentMessageFromBuffer( std::string& frameID , std::string& entityType , std::string& entityID )
{
	for ( int index = 0; index < m_sentUDPMesageBuffer.size(); index++ )
	{
		if ( m_sentUDPMesageBuffer[ index ] != "" )
		{
			std::string curString = m_sentUDPMesageBuffer[ index ];
			Strings sentMsg = SplitStringAtGivenDelimiter( curString , '|' );

			if ( sentMsg.size() != D_NUM_TOTAL )
			{
				m_sentUDPMesageBuffer[ index ] = "";
				//m_sentUDPMesageBuffer.erase( index.begin() , index.end() );
				continue;
			}
			if ( sentMsg.size() == D_NUM_TOTAL )
			{
				if ( ( sentMsg[ D_FRAME_ID ] == frameID ) && ( sentMsg[ D_ENTITY_TYPE ] == entityType ) && ( sentMsg[ D_ENTITY_ID ] == entityID ) )
				{
					m_sentUDPMesageBuffer[ index ] = "";
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string GameNetworkSystem::GetConnectClientAddress( SOCKET socket )
{

	std::array<char , 128> addressString;

	sockaddr clientAddress;
	int addressSize = sizeof( clientAddress );
	int iResult = getpeername( socket , &clientAddress , &addressSize );

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Get Peer Name Failed %i" , WSAGetLastError() );
	}

	DWORD outLength = static_cast< DWORD >( addressString.size() );
#pragma warning( push )
#pragma warning( disable : 4996  )
	iResult = WSAAddressToStringA( &clientAddress , addressSize , NULL , &addressString[ 0 ] , &outLength );
#pragma warning( pop )

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Get Peer Name Failed %i" , WSAGetLastError() );
	}

	addressString[ outLength ] = NULL;
	return std::string( &addressString[ 0 ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string GameNetworkSystem::GetConnectClientAddress()
{
	std::array<char , 128> addressStr;
	sockaddr clientAddr;
	int addrSize = sizeof( clientAddr );
	int iResult = getpeername( m_linkSocket , &clientAddr , &addrSize );
	if ( iResult == SOCKET_ERROR )
	{
		//error
	}
	DWORD outlen = ( DWORD ) addressStr.size();
#pragma warning( push )
#pragma warning( disable : 4996  )
	iResult = WSAAddressToStringA( &clientAddr , addrSize , NULL , &addressStr[ 0 ] , &outlen );
#pragma warning( pop )
	if ( iResult == SOCKET_ERROR )
	{
		//error
	}
	return &addressStr[ 0 ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::AddNetowrkingCommandsToConsole()
{
	EventArgs consoleArgs;
	g_theDevConsole->CreateCommand( "StartTCPServer" , "Start a TCP Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "StartTCPServer" , StartTCPServer );

	g_theDevConsole->CreateCommand( "CloseTCPServer" , "Close TCP Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "CloseTCPServer" , CloseTCPServer );

	g_theDevConsole->CreateCommand( "ServerSendMessage" , "Send Message from Server to Client" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ServerSendMessage" , SendMessageToClient );

	g_theDevConsole->CreateCommand( "ConnectToServer" , "Connect Client to Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ConnectToServer" , ConnectToServer );

	g_theDevConsole->CreateCommand( "ClientSendMessage" , "Send Message from Client to Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ClientSendMessage" , SendMessageToServer );

	g_theDevConsole->CreateCommand( "DisconnectClient" , "Disconnect Client from the Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "DisconnectClient" , DisconnectFromServer );

	g_theDevConsole->CreateCommand( "OpenUDPPort" , "Open UDP Socket at listen and send ports" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "OpenUDPPort" , OpenUDPPort );

	g_theDevConsole->CreateCommand( "SendUDPMessage" , "Send Message over UDP Socket" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "SendUDPMessage" , SendUDPMessage );

	g_theDevConsole->CreateCommand( "CloseUDPPort" , "Close UDP Socket" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "CloseUDPPort" , CloseUDPPort );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::StartTCPServer( EventArgs& args )
{
	int port = args.GetValue( "port" , 48000 );

	if ( nullptr != g_theGameNetworkSys )
	{
		g_theGameNetworkSys->SetIsListening( true );

		if ( nullptr == g_theGameNetworkSys->m_TCPServer )
		{
			g_theGameNetworkSys->m_TCPServer = new GameTCPServer( port );
			g_theGameNetworkSys->m_TCPServer->Bind();
			g_theGameNetworkSys->m_TCPServer->Listen();
		}
		
		g_theGameNetworkSys->m_TCPServer->SetListenPort( port );
		g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Server Started" );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::CloseTCPServer( EventArgs& args )
{
	UNUSED( args );
	
	if( nullptr != g_theGameNetworkSys )
	{
		g_theGameNetworkSys->SetIsListening( false );
		g_theGameNetworkSys->m_TCPServer->SetListenPort( -1 );
		closesocket( g_theGameNetworkSys->m_linkSocket );
		closesocket( g_theGameNetworkSys->m_TCPServer->m_listenSocket );
		
		g_theGameNetworkSys->m_linkSocket = INVALID_SOCKET;
		if ( g_theGameNetworkSys->m_TCPServer != nullptr )
		{
			delete g_theGameNetworkSys->m_TCPServer;
			g_theGameNetworkSys->m_TCPServer = nullptr;
		}
		
		g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "SERVER CLOSED" );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::SendMessageToClient( EventArgs& args )
{
	if( ( nullptr != g_theGameNetworkSys->m_TCPServer ) && ( g_theGameNetworkSys->m_linkSocket != INVALID_SOCKET ) )
	{
		std::string message = args.GetValue( "msg" , "InvalidMessage" );
		g_theGameNetworkSys->m_TCPServer->SetServerSendMessage( message );
		m_wasMessageJustSentByServer = true;

		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::SendMessageToServer( EventArgs& args )
{
	if( ( nullptr != g_theGameNetworkSys->m_TCPclient ) && ( g_theGameNetworkSys->m_TCPclient->m_clientSocket != INVALID_SOCKET ) )
	{
		std::string message = args.GetValue( "msg" , "InvalidMessage" );
		g_theGameNetworkSys->m_TCPclient->SetClientSendMessage( message );
		m_wasMessageJustSentByClient = true;

		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::ConnectToServer( EventArgs& args )
{
	if( ( nullptr != g_theGameNetworkSys->m_TCPclient ) )
	{
		std::string host = args.GetValue( "ipaddr" , "" );		
		std::string port = args.GetValue( "port" , "48000" );
	
		g_theGameNetworkSys->m_TCPclient->m_clientSocket = g_theGameNetworkSys->m_TCPclient->Connect( host.c_str() ,
		                                                                                      ( uint16_t )atoi( port.c_str() ) ,
		                                                                                      Mode::Nonblocking );
		if( g_theGameNetworkSys->m_TCPclient->m_clientSocket != INVALID_SOCKET )
		{
			g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Server Connected to %s" ,
										  g_theGameNetworkSys->GetConnectClientAddress( g_theGameNetworkSys->m_TCPclient->m_clientSocket ).c_str() );
		}
		
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::DisconnectFromServer( EventArgs& args )
{
	UNUSED( args );

	if( nullptr != g_theGameNetworkSys )
	{
		closesocket( g_theGameNetworkSys->m_linkSocket );
		closesocket( g_theGameNetworkSys->m_TCPclient->m_clientSocket );

		g_theGameNetworkSys->m_TCPclient->m_clientSocket = INVALID_SOCKET;
		g_theGameNetworkSys->m_linkSocket = INVALID_SOCKET;

		g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "CLIENT DISCONNECTED" );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::OpenUDPPort( EventArgs& args )
{
	if ( nullptr != g_theGameNetworkSys )
	{
		int bindPort	= args.GetValue( "bindPort" , 48000 );
		int sendToPort	= args.GetValue( "sendPort" , 48001 );
		std::string host = args.GetValue( "host" , "127.0.0.1" );

		g_theGameNetworkSys->StartUDPListner( bindPort , sendToPort , host );
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::SendUDPMessage( EventArgs& args )
{
	if ( nullptr != g_theGameNetworkSys )
	{
		std::string msg = args.GetValue( "msg" , "" );
		if( msg != "" )
		{
			g_theGameNetworkSys->SendUDPMessage( msg );
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::CloseUDPPort( EventArgs& args )
{
	if ( nullptr != g_theGameNetworkSys )
	{
		std::string bindPort = args.GetValue( "bindPort" , "48000" );
		g_theGameNetworkSys->m_UDPListner->m_listenSocket->Close();
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	
void GameNetworkSystem::StartUDPListner( int bindPort , int sendPort , std::string host /*= "127.0.0.1"*/ )
{
	m_UDPListner->StartSocket( bindPort , sendPort , host );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::SendUDPMessage( std::string message )
{
	m_UDPListner->AddMessage( message );
}

//--------------------------------------------------------------------------------------------------------------------------------------------