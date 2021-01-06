#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/MapMaterial.hpp"
#include "Game/MapRegion.hpp"

#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC std::map<std::string , MapRegion*>		MapRegion::s_mapRegions;

//--------------------------------------------------------------------------------------------------------------------------------------------

bool MapRegion::LoadDefinitions( char const* regionDataFilePath )
{
	tinyxml2::XMLDocument regionDataDocument;
	regionDataDocument.LoadFile( regionDataFilePath );

	XMLElement* root = regionDataDocument.RootElement();

	if ( nullptr == root )
	{
		std::string filePath = regionDataFilePath;
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "ERROR: Map Region Types file at path: %s not present." , filePath.c_str() );
		return false;
	}
		
	for( XMLElement* element = root->FirstChildElement( "RegionType" ); element != nullptr; element = element->NextSiblingElement( "RegionType" ) )
	{
		MapRegion* newMapRegion = new MapRegion( element );
		auto mapRegion = MapRegion::s_mapRegions.find( newMapRegion->m_name );

		if ( mapRegion != MapRegion::s_mapRegions.end() )
		{
			//error multiple definitions
			continue;
		}

		MapRegion::s_mapRegions[ newMapRegion->m_name ] = newMapRegion;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MapRegion const* MapRegion::GetDefinition( std::string const& name )
{
	auto mapRegion = s_mapRegions.find( name );

	if ( mapRegion != s_mapRegions.end() )
	{
		return mapRegion->second;
	}

	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MapRegion::MapRegion( XMLElement* materialDef )
{
	m_name = ParseXmlAttribute( *materialDef , "name" , "NULL" );

	if ( m_name == "NULL" )
	{
		//Error
	}
	
	std::string isSolid = ParseXmlAttribute( *materialDef , "isSolid" , "NULL" );

	if ( isSolid == "NULL" )
	{
		//Error
	}
	else if ( isSolid == "false" )
	{
		m_isSolid = false;
	}
	else
	{
		m_isSolid = true;
	}

	if ( m_isSolid )
	{
		tinyxml2::XMLElement* element = materialDef->FirstChildElement( "Side" );
		if ( element == nullptr )
		{
			//Error
		}
		else
		{
			std::string sideMaterial = ParseXmlAttribute( *element , "material" , "NULL" );
			m_sideMaterial = MapMaterial::GetDefinition( sideMaterial );
		}
	}
	else
	{
		tinyxml2::XMLElement* firstElement	= materialDef->FirstChildElement( "Floor" );
		tinyxml2::XMLElement* secondElement = materialDef->FirstChildElement( "Ceiling" );

		std::string floorMaterial = ParseXmlAttribute( *firstElement , "material" , "NULL" );
		m_floorMaterial = MapMaterial::GetDefinition( floorMaterial );

		std::string celilingMaterial = ParseXmlAttribute( *secondElement , "material" , "NULL" );
		m_ceilingMaterial = MapMaterial::GetDefinition( celilingMaterial );

	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------