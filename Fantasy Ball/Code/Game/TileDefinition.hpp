#pragma once
#include <map>
#include <string>
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Primitives/AABB2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball;

//--------------------------------------------------------------------------------------------------------------------------------------------

class TileDefinition
{
public:

	TileDefinition( const tinyxml2::XMLElement& definitionXMLElement );
	static void				CreateTileDefinitions( const char* xmlFilePath );
	friend TileDefinition*	ParseXmlAttribute( const tinyxml2::XMLElement& element , const char* attributeName , TileDefinition* defaultValue );
	
private:

//--------------------------------------------------------------------------------------------------------------------------------------------

public:
	static std::map< std::string , TileDefinition* >		s_definitions;
	std::string												m_name						= "";
	int														m_health					= 1;
	int														m_solidAtHealth				= 0;
	int														m_visibleAtHealth			= 0;
	bool													m_isSolid					= false;
	bool													m_isVisible					= false;
	IntVec2													m_spriteCoords				= IntVec2::ZERO;
	IntVec2													m_VFXSpriteCoords			= IntVec2::ZERO;
	IntVec2													m_ballSpriteCoords			= IntVec2::ZERO;
	Rgba8													m_tileColor					= Rgba8( 0 , 0 , 0 , 0 );
	AABB2													m_spriteUVs					= AABB2::ZERO_TO_ONE;
	float													m_screenshakeMinProbablity	= 0.f;

private:

};
