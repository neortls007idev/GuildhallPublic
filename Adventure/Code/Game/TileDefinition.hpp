#pragma once
#include <map>
#include <string>
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Primitives/AABB2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class TileDefinition
{
public:

	TileDefinition( const tinyxml2::XMLElement& definitionXMLElement );
	static void  CreateTileDefinitions( const char* xmlFilePath );
	friend TileDefinition* ParseXmlAttribute( const tinyxml2::XMLElement& element , const char* attributeName , TileDefinition* defaultValue );


private:

//--------------------------------------------------------------------------------------------------------------------------------------------

public:
	static std::map< std::string , TileDefinition* >		s_definitions;
//	NamedStrings											m_tileProperties;
//	NamedStrings											m_spriteSheet;
	std::string												m_name					= "";
	bool													m_allowsSight			= false;
	bool													m_allowsWalk			= false;
	bool													m_allowsSwim			= false;
	bool													m_allowsFly				= false;
	IntVec2													m_spriteCoords			= IntVec2::ZERO;
	Rgba8													m_tileColor				= Rgba8( 0 , 0 , 0 , 0 );
	AABB2													m_spriteUVs				= AABB2::ZERO_TO_ONE;

private:

};
