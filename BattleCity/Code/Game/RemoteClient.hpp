#pragma once
#include "Game/Client.hpp"

class Game;

//--------------------------------------------------------------------------------------------------------------------------------------------

class RemoteClient : public Client
{
public:
	RemoteClient( int PlayerID );
	~RemoteClient();

	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;
			
public:
	Game* m_game = nullptr;
	int m_remotePlayerID = 1;
};

//--------------------------------------------------------------------------------------------------------------------------------------------