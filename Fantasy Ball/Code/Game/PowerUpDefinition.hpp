#pragma once
#include <map>
#include <string>
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Primitives/AABB2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

enum ePowerUpType : unsigned char;
class Ball;

//--------------------------------------------------------------------------------------------------------------------------------------------

class PowerUpDefinition
{
public:

	PowerUpDefinition( const tinyxml2::XMLElement& definitionXMLElement );
	static void					CreatePowerUpDefinitions( const char* xmlFilePath );
	friend PowerUpDefinition*	ParseXmlAttribute( const tinyxml2::XMLElement& element , const char* attributeName , PowerUpDefinition* defaultValue );
	static PowerUpDefinition*	GetPowerUpDefinitionForPowerUp( ePowerUpType type );
	
private:

//--------------------------------------------------------------------------------------------------------------------------------------------

public:
	static std::map< std::string , PowerUpDefinition* >		s_definitions;
	std::string												m_name							= "";
	IntVec2													m_spriteCoords					= IntVec2::ZERO;
	Rgba8													m_color							= Rgba8( 0 , 0 , 0 , 0 );
	AABB2													m_spriteUVs						= AABB2::ZERO_TO_ONE;
	float													m_spawnProbablityRequirement	= 0.95f;

private:

};
