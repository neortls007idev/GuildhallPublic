#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"
#include "Game/Networking/GameUDPData.hpp"
#include "Game/Networking/GameUDPListner.hpp"
#include "Game/Networking/GameUDPSocket.hpp"
#include <thread>
#include "Engine/Core/StdExtensions.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole* g_theDevConsole;
extern GameNetworkSystem* g_theGameNetworkSys;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
GameUDPListner::GameUDPListner()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameUDPListner::~GameUDPListner()
{
	if ( readerThread != nullptr )
	{
		readerThread->join();
	}
	if ( writerThread != nullptr )
	{
		writerThread->join();
	}

	SAFE_RELEASE_POINTER( readerThread );
	SAFE_RELEASE_POINTER( writerThread );
	SAFE_RELEASE_POINTER( m_listenSocket );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	
void GameUDPListner::StartSocket( int bindPort , int sendPort , std::string host /*= "127.0.0.1" */ )
{
	m_listenSocket = new GameUDPSocket( host , sendPort );
	m_listenSocket->Bind( bindPort );

	 readerThread = new std::thread( &GameUDPListner::Writer , this , std::ref(*m_listenSocket), std::ref( m_writeQueue ) );
	 writerThread = new std::thread( &GameUDPListner::Reader, this , std::ref(*m_listenSocket), std::ref( m_readQueue ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameUDPListner::AddMessage( std::string& message )
{
	m_writeQueue.push( message );

	if ( message != "" )
	{
		bool wasMessageAddedToBuffer = false;
		for ( int index = 0; index < g_theGameNetworkSys->m_sentUDPMesageBuffer.size(); index++ )
		{
			if ( g_theGameNetworkSys->m_sentUDPMesageBuffer[ index ] == "" )
			{
				g_theGameNetworkSys->m_sentUDPMesageBuffer[ index ] = message;
				wasMessageAddedToBuffer = true;
				break;
			}
		}
		if ( !wasMessageAddedToBuffer )
		{
			g_theGameNetworkSys->m_sentUDPMesageBuffer.push_back( message );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameUDPListner::Reader( GameUDPSocket& socket , SynchronizedLockFreeQueue<std::string>& readQueue )
{
	while ( socket.IsValid() )
	{
		UDPMessageHeader const* pMsg = nullptr;
		if ( socket.Receive() > 0 )
		{
			auto& buffer = socket.ReceiveBuffer();
			pMsg = reinterpret_cast< UDPMessageHeader const* > ( &buffer[ 0 ] );
			std::string receivedMessage = &buffer[ sizeof( UDPMessageHeader ) ];
			//LOG_SYSMESSAGE( "UDP Received Message: %s" , receivedMessage.c_str() );
			readQueue.push( receivedMessage );

			if( receivedMessage != "" )
			{
				bool wasMessageAddedToBuffer = false;
				g_theGameNetworkSys->m_recieveBufferMutex.lock();
				for( int index = 0; index < g_theGameNetworkSys->m_recievedUDPMesageBuffer.size() ; index++ )
				{
					if( g_theGameNetworkSys->m_recievedUDPMesageBuffer[ index ] == "" )
					{
						g_theGameNetworkSys->m_recievedUDPMesageBuffer[ index ] = receivedMessage;
						wasMessageAddedToBuffer = true;
						break;
					}
				}
				if( !wasMessageAddedToBuffer )
				{
					g_theGameNetworkSys->m_recievedUDPMesageBuffer.push_back( receivedMessage );
				}
				g_theGameNetworkSys->m_recieveBufferMutex.unlock();
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	
void GameUDPListner::Writer( GameUDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writeQueue )
{
	UDPMessageHeader header;
	while ( socket.IsValid() )
	{
		std::string message = writeQueue.pop();
		header.id = 1;
		header.length = ( uint16_t ) strlen( message.c_str() );
		header.sequenceNo = 1;
		if ( message != "" )
		{
			auto& buffer = socket.SendBuffer();

			*reinterpret_cast< UDPMessageHeader* >( &buffer[ 0 ] ) = header;
			memcpy( &( socket.SendBuffer()[ sizeof( UDPMessageHeader ) ] ) , message.c_str() , header.length );
			socket.SendBuffer()[ sizeof( UDPMessageHeader ) + header.length ] = NULL;
			m_listenSocket->Send( sizeof( UDPMessageHeader ) + ( int ) message.length() + 1 );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------