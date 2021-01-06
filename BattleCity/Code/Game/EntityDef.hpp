#pragma once
#include <map>
#include <string>
#include "Engine/Core/Rgba8.hpp"
#include "ThirdParty/TinyXML2/tinyxml2.h"

class EntityDefinition
{
public:
	EntityDefinition( const tinyxml2::XMLElement& definitionXMLElement );
	~EntityDefinition() {};

	static void  CreateEntityDefinitions( const char* xmlFilePath );

private:

public:
	static std::map< std::string , EntityDefinition* >		s_definitions;
	std::string												m_name					= "Actor";
	std::string												m_entityType			= "INVALID";
	std::string												m_faction				= "INVALID";
	Rgba8													m_colorInImageFile;
	Vec2													m_velocity				= Vec2::ZERO;
	int														m_health				= 1;
	float													m_physicsRadius			= 0.f;
	float													m_cosmeticRadius		= 0.f;

	bool													m_pushesEntiites		= false;
	bool													m_isPushedByWalls		= false;
	bool													m_isPushedByEntities	= false;
	bool													m_isHitByBullets		= false;
};