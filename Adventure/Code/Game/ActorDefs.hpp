#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Renderer/SpriteAnimeSet.hpp"
#include <string>


//--------------------------------------------------------------------------------------------------------------------------------------------

class ActorDefinition
{
public:
	ActorDefinition( const tinyxml2::XMLElement& definitionXMLElement );
	~ActorDefinition() {};

	static void  CreateActorDefinitions( const char* xmlFilePath );

private:
	
public:
	static std::map< std::string , ActorDefinition* >		s_definitions;
	std::string	   m_name				= "Actor";
	std::string	   m_faction			= "INVALID";
	float		   m_physicsRadius		= 0.4f;
	AABB2		   m_localDrawBounds	= AABB2( 0.f , 0.f , .75f , 1.f );
	Vec2		   m_startPosition		= Vec2::ZERO;
	Vec2		   m_defaultVelocity	= Vec2::ZERO;
	IntVec2		   m_spriteCoords;
	AABB2		   m_spriteUVs;
	SpriteAnimSet* m_animSet			= nullptr;
	bool		   m_canWalk			= false;
	bool		   m_canSwim			= false;
	bool		   m_canFly				= false;
	bool		   m_isDead				= false;
	bool		   m_isGarbage			= false;
};