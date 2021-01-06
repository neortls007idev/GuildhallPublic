#pragma once
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameSinglePlayer : public Game
{

public:
	GameSinglePlayer();
	~GameSinglePlayer();

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

			void CreatePlayer();
	
public:
	Player* m_player	= nullptr;
	Camera* m_playerCamera = nullptr;
	Camera* m_UICamera = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------