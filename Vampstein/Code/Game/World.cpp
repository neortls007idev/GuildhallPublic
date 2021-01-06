#include "Game/World.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "TileMap.hpp"
#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

World::World( Game* owner , char const* name , char const* mapsFolderPath ) :
																		m_owner( owner ) ,
																		m_name( name )
{
	CreateMaps( mapsFolderPath );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

World::~World()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::CreateMaps( std::string mapsFolderPath )
{
	Strings mapFileNames = GetFileNamesInfolder( mapsFolderPath , "*.xml" );

	for ( size_t index = 0 ; index < mapFileNames.size() ; index++ )
	{
		std::string mapFilePath = mapsFolderPath + "/" + mapFileNames[ index ];

		tinyxml2::XMLDocument mapDocument;
		mapDocument.LoadFile( mapFilePath.c_str() );

		XMLElement* mapRoot = mapDocument.RootElement();

		if( nullptr == mapRoot )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_ERROR ,
										  "ERROR: MAP Root Element in file %s is invalid; Not Parsing this Map" , mapFilePath.c_str() );
			continue;
		}
		
		std::string mapType = ParseXmlAttribute( *mapRoot , "type" , "NULL" );
		Map* temp =  Map::CreateNewMapOfType( mapType , mapFileNames[ index ].c_str() , mapRoot );
		//Map* temp = new Map( mapFileNames[ index ].c_str() , mapRoot );
		
		if( nullptr == temp )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_ERROR ,
										  "ERROR: MAP file %s is ill formed; Parsing this Map Failed" , mapFilePath.c_str() );
			continue;
		}
		
		if ( nullptr != temp )
		{
			m_maps[ mapFileNames[ index ] ] = temp;
		}	
	}

	if ( m_maps.size() == 0 )
	{
		g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_ERROR , "ERROR: No maps! were loaded cannot start Game" );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::Update()
{
	if( nullptr != m_currentMap )
	{
		m_currentMap->UpdateMeshes();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void World::Render() const
{
	if ( nullptr != m_currentMap )
	{
		m_currentMap->Render();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map* World::GetMapByName( std::string mapName ) const
{
	std::string mapFileName = mapName + ".xml";
	for ( auto it = m_maps.begin(); it != m_maps.end() ; ++it )
	{
		if ( it->first == mapFileName )
		{
			return it->second;
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map* World::GetFirstMap() const
{
	if ( m_maps.size() > 0 )
	{
		return m_maps.begin()->second;
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Strings World::GetAllMapNames() const
{
	Strings allMapNames;

	for( auto it = m_maps.begin(); it != m_maps.end(); ++it )
	{
		std::string name = it->first;
		name.erase( name.length() - 4 , 4 );
		allMapNames.push_back( name );
	}

	return allMapNames;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map* World::GetMap( std::string mapName )
{
	std::string mapNameCopy = mapName + ".xml";

	auto found = m_maps.find( mapNameCopy );
	if( found != m_maps.end() )
	{
		//g_theAudio->PlaySound( m_sound , false , 1.f );
		return found->second;
	}
	return nullptr;
}