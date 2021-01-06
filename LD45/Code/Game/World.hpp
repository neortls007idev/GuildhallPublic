#pragma once
#include "Game/Map.hpp"

class World
{

public:

	World(Game* theGame);
	void Update( float deltaSeconds );
	void Render() const;

private:
	
	Map* m_currentMap = nullptr;
	Game* m_game = nullptr;

};