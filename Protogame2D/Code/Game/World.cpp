#include "Game/World.hpp"

World::World( Game* theGame ) : m_theGame( theGame )
{
	m_currentMap = new Map( m_theGame , IntVec2( ( int ) WORLD_SIZE_X , ( int ) WORLD_SIZE_Y ) );
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
}

void World::Render() const
{
	m_currentMap->Render();
}
