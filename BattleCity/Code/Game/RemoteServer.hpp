#pragma once
#include "Game/Server.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class RemoteServer : public Server
{
public:
	RemoteServer();
	~RemoteServer();

	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;
			void	ParseAndUpdateEntities();

//	Game* GetGame();
	bool RequestAddPlayerEntityandUDPConnection( EventArgs args );

public:
	int m_playerID = 0;
	std::string m_hostIPaddr = "";
	int m_udpSendToPort = 48000;
	int m_udpListenPort = 48001;
};

//--------------------------------------------------------------------------------------------------------------------------------------------