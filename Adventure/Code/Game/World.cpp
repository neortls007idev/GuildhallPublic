#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

World::World( Game* theGame ) : m_theGame( theGame )
{

	for ( auto totalMaps = MapDefinition::s_definitions.cbegin(); totalMaps != MapDefinition::s_definitions.cend(); totalMaps++ )
	{
		Map* temp = new Map( m_theGame , totalMaps->second , totalMaps->second->m_name );
		m_maps.push_back( temp );
	}
	
	m_currentMapNumber = 0;
	m_currentMap = m_maps[ m_currentMapNumber ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

World::~World()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
	UpdateFromKeyboard();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::Render() const
{
	m_currentMap->Render();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::UpdateFromKeyboard()
{
	if ( g_theInput->WasKeyJustPressed( 'N' ) )
	{
		m_currentMapNumber++;
		if ( m_currentMapNumber == ( m_maps.size() ) )
		{
			m_currentMapNumber = 0;
		}
		m_currentMap = m_maps[ m_currentMapNumber ];
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
