#include "Game/World.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

World::World( Game* theGame ) : m_theGame( theGame )
{
	//m_currentMap = new Map( m_theGame , IntVec2( ( int ) INTITAL_MAP_SIZE_X , ( int ) INTITAL_MAP_SIZE_Y ), m_mapNumber );
	for ( int mapNumber = 0; mapNumber < TOTAL_MAPS_IN_THE_WORLD; mapNumber++ )
	{
		while ( m_Maps[mapNumber] == nullptr )
		{
			m_Maps[mapNumber]= new Map( m_theGame , IntVec2( ( int ) ( INTITAL_MAP_SIZE_X + ( mapNumber * 10 ) ), ( int ) ( INTITAL_MAP_SIZE_Y + ( mapNumber * 10 ) ) ) , mapNumber );
		}
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
		Entitylist playerList = m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ];
		for ( int entityListIndex = 0; entityListIndex < ( int ) playerList.size(); entityListIndex++ )
		{
			if ( playerList[ entityListIndex ] && playerList[ entityListIndex ]->m_health >= 0 )
			{
				bool resultcheck = IsPointOnDisc2D( Disc2D( playerList[ entityListIndex ]->m_position , playerList[ entityListIndex ]->m_physicsRadius ) , m_currentMap->m_exitPosition );
				if ( resultcheck )
				{
					m_currentMapNumber++;
				}
				return resultcheck;
			}
		}
		return false;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------