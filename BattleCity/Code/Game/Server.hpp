#pragma once
#include <vector>

#include "GameCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameMultiplayer;
class GameSinglePlayer;
class Game;
class PlayerClient;
class Client;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class Server
{
public:
	Server();
	~Server();

	virtual void Startup();
	virtual void Shutdown();
	
	virtual void BeginFrame();
	virtual void Update( float deltaSeconds );
	virtual void EndFrame();

	void CreateSinglePlayerGame();
	void CreateMultiPlayerGame();
	
	void AddPlayerClientToServer( Client* client );
	bool RemovePlayerClientFromServer( Client* client );
	void	AddRemoteNewRemotePlayer();
	Game*	GetGame();
	int		GetFrameID()																		{ return m_frameID;  }
	void	ParseReceivedMessages( std::vector< std::string > messageBuffer , bool isAuthserver );
	
public:
	bool						m_isRemoteClientConnectionComplete = false;
	int							m_uniqueKey = 0;	
	int							m_frameID = 0;

	GameSinglePlayer*			m_singlePlayerGame	= nullptr;
	GameMultiplayer*			m_multiPlayerGame	= nullptr;
	GameType					m_gameType			= SINGLE_PLAYER;
	std::vector< Client* >		m_clients;
};

//--------------------------------------------------------------------------------------------------------------------------------------------