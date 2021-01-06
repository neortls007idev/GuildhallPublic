#include "Game/World.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/MapDefinition.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

World::World( Game* theGame ) : m_theGame( theGame )
{
	int mapNumber = 0;

	for( auto mapDefIndex : MapDefinition::s_definitions )
	{
		m_Maps[ mapNumber ] = new Map( m_theGame , mapDefIndex.second, mapDefIndex.second->m_name.c_str() );
		mapNumber++;
	}

	m_currentMap = m_Maps[ 0 ];
	m_currentMapNumber = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

World::~World()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
	if ( CheckForNextMap() )
	{
		if ( m_currentMapNumber >= TOTAL_MAPS_IN_THE_WORLD )
		{
			return;
		}
		m_currentMap = m_Maps[ m_currentMapNumber ];
		m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] = m_Maps[ m_currentMapNumber - 1 ]->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ];
		m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_position = Vec2( 1.5f , 1.5f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::Render() const
{
	m_currentMap->Render();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool World::CheckForNextMap()
{
	if ( m_currentMapNumber < TOTAL_MAPS_IN_THE_WORLD )
	{
		return false;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------