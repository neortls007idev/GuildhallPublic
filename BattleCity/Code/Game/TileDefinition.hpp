#pragma once
#include "Game/Tile.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
//#include <vector>
#include <map>
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

class TileDefinition
{
public:

	TileDefinition( const tinyxml2::XMLElement& definitionXMLElement );
	static void  CreateTileDefinitions( const char* xmlFilePath );
	friend TileDefinition* ParseXmlAttribute( const tinyxml2::XMLElement& element , const char* attributeName , TileDefinition* defaultValue );
	~TileDefinition();

public:
	
	static std::map< std::string , TileDefinition* >		s_definitions;
	static SpriteSheet*										s_tileSS;
	std::string												m_name				= "";
	bool													m_isSolid			= false;
	Vec2													m_minUVs;
	Vec2													m_maxUVs;
	Rgba8													m_tint;

private:

};
