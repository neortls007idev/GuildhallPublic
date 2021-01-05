#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class World
{
public:	
	World( Game* theGame );
	~World();
	void Update( float deltaSeconds );
	void Render() const;

	void UpdateFromKeyboard();
	
private:

//--------------------------------------------------------------------------------------------------------------------------------------------
public:
	Game*						   m_theGame		  = nullptr;
	Map*						   m_currentMap		  = nullptr;
	int							   m_currentMapNumber = -1;
	std::vector< Map* >			   m_maps;
};

//--------------------------------------------------------------------------------------------------------------------------------------------