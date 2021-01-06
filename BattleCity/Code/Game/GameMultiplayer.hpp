#pragma once
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameMultiplayer : public Game
{

public:

	GameMultiplayer();
	~GameMultiplayer();

	virtual void Startup() override;
	virtual void Update( float deltaSeconds ) override;
	
	//void UpdateFromText( std::string );
	virtual void Render() const override;
	Player* CreateAndAddPlayerAtpositionAndOrientation( Vec2 pos = Vec2( 1.5f , 1.5f ) , float orientation = 45.f );

	std::vector< Player* >	m_players;
	Camera*					m_playerCamera	= nullptr;
	Camera*					m_UICamera		= nullptr;

};

//--------------------------------------------------------------------------------------------------------------------------------------------
