#include "Game/World.hpp"

World::World( Game* theGame ) : m_game( theGame )
{
	m_currentMap = new Map( m_game , IntVec2( 600 , 800 ) );
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
}

void World::Render() const
{
	m_currentMap->Render();
}
