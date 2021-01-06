#pragma once
#include "Game/Map.hpp"

class World
{

public:

	World( Game* theGame );
	void Update( float deltaSeconds );
	void Render() const;

private:

	Game* m_theGame = nullptr;

public:
	Map* m_currentMap = nullptr;
};