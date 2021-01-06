#pragma once
#include "Game/Server.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameSinglePlayer;
class GameMultiplayer;
class Timer;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class AuthoritativeServer : public Server
{
public:
	AuthoritativeServer();
	~AuthoritativeServer();
	
	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;

			void	AddPlayers();
//			void	AssignInputSystem( InputSystem* inputSystem );
			void	ParseAndSendEntityData();
public:
	
private:
	int m_numRemotePlayers = 0;
	Timer* m_sendFreq = nullptr;
	int m_udpSendToPort = 48000;
	int m_udpListenPort = 48001;
};

//--------------------------------------------------------------------------------------------------------------------------------------------