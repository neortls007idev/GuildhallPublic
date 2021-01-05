#include "Game/MapDefinition.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

std::unordered_map< std::string , MapDefinition* > MapDefinition::s_definitions;

//--------------------------------------------------------------------------------------------------------------------------------------------

MapDefinition::MapDefinition( const tinyxml2::XMLElement& definitionXMLElement )
{
	m_name   = ParseXmlAttribute( definitionXMLElement , "name" , "Invalid Name" );
	m_width  = ParseXmlAttribute( definitionXMLElement , "width"  , 0 );
	m_height = ParseXmlAttribute( definitionXMLElement , "height" , 0 );

	std::string defaultTile = ParseXmlAttribute( definitionXMLElement , "fillTile" , "Grass" );
	m_fillTile = TileDefinition::s_definitions[ defaultTile ];

	std::string edgeTile = ParseXmlAttribute( definitionXMLElement , "edgeTile" , "Grass" );
	m_edgeTile = TileDefinition::s_definitions[ edgeTile ];

	const XMLElement* mapGenrationStep = definitionXMLElement.FirstChildElement();

	if ( mapGenrationStep )
	{
		std::string mapGenElementName = mapGenrationStep->Name();
	
		if ( mapGenElementName.compare("GenerationSteps") )
		{
			return;
		}
		
		mapGenrationStep  = mapGenrationStep->FirstChildElement();
		//int currentStepNumber = 0;
		while ( mapGenrationStep )
		{
			m_generationSteps.push_back( MapGenStep::CreateMapGenStep( *mapGenrationStep ) );
			mapGenrationStep = mapGenrationStep->NextSiblingElement();
			//currentStepNumber++;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapDefinition::CreateMapDefinitions( const char* xmlFilePath )
{
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );

	if ( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		ERROR_AND_DIE( "XML FILE DID NOT LOAD" );
		return;
	}

	tinyxml2::XMLElement* mapDefinition = xmlDocument.RootElement();
	mapDefinition = mapDefinition->FirstChildElement( "MapDefinition" );

	while ( mapDefinition )
	{
		MapDefinition* currentTileDefiniton = new MapDefinition( *mapDefinition );
		std::string key = ParseXmlAttribute( *mapDefinition , "name" , "Invalid Name" );
		s_definitions[ key ] = currentTileDefiniton;
		mapDefinition = mapDefinition->NextSiblingElement();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

