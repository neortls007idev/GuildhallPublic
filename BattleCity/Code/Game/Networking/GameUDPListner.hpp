#pragma once
#include "Engine/DataStructures/ThreadSafe/SynchronizedLockFreeQueue.hpp"
#include "Game/Networking/GameUDPSocket.hpp"
#include <string>
#include <thread>

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class GameUDPSocket;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class GameUDPListner
{
public:
	GameUDPListner();
	~GameUDPListner();

public:
	void StartSocket( int bindPort , int sendPort , std::string host = "127.0.0.1" );
	void AddMessage( std::string& message );
	void AddGameplayPacketMessage( GameUDPData& udpData );

	void Reader( GameUDPSocket& socket , SynchronizedLockFreeQueue<std::string>& readQueue );
	void Writer( GameUDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writeQueue );

	GameUDPSocket*	 m_listenSocket = nullptr;
	std::thread*	 readerThread	= nullptr;
	std::thread*	 writerThread	= nullptr;

	SynchronizedLockFreeQueue < std::string >	m_readQueue;
	SynchronizedLockFreeQueue < std::string >	m_writeQueue;
	SynchronizedLockFreeQueue < GameUDPData >	m_gameDataReadQueue;
	SynchronizedLockFreeQueue < GameUDPData >	m_gameDataWriteQueue;
};

//--------------------------------------------------------------------------------------------------------------------------------------------