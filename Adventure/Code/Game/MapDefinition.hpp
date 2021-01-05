#pragma once
#include <map>
#include <string>
#include <vector>
#include "Engine/Core/NamedStrings.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapGenStep.hpp"
#include <unordered_map>

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapDefinition
{
public:

	MapDefinition( const tinyxml2::XMLElement& definitionXMLElement );
	static void  CreateMapDefinitions( const char* xmlFilePath );


private:

	//--------------------------------------------------------------------------------------------------------------------------------------------

public:
	static std::unordered_map< std::string , MapDefinition* >			s_definitions;
	//	NamedStrings											m_tileProperties;
	//	NamedStrings											m_spriteSheet;
	int														m_width;
	int														m_height;
	TileDefinition*											m_fillTile;
	TileDefinition*											m_edgeTile;
	std::string												m_name;
	std::vector<MapGenStep*>								m_generationSteps;
private:

};
