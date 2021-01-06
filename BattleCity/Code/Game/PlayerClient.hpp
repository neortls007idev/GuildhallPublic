#pragma once
#include "Game/Client.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;

//--------------------------------------------------------------------------------------------------------------------------------------------

class PlayerClient : public Client
{
public:
	PlayerClient( int playerID );
	~PlayerClient();

	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	Render() override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;

public:
	Game* m_game = nullptr;
	int m_localPlayerID = 0;
	int m_uniqueKey = 0;
	int m_frameID = 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------