#pragma once
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <map>

#include "BillBoard.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class SpriteSheet;
class SpriteAnimSet;
class SpriteAnimDefinition;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum ENTITYANIMS
{
	ENTITY_INVALID_ANIM ,
	ENTITY_WALK_ANIM ,
	ENTITY_ATTACK_ANIM ,
	ENTITY_PAIN_ANIM ,
	ENTITY_DEATH_ANIM ,

	NUM_TOTAL_ENTITY_ANIMS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class EntityDef
{
public:
	EntityDef( tinyxml2::XMLElement* xmlEntityDef );
	EntityDef();
	~EntityDef() {};

	static void		  CreateDefinitions( char const* dataFilePath );
	static void		  ParseAllActors( tinyxml2::XMLElement* root );
	static int		  GetIndexForAnimName( std::string animName );
	static void		  ParseEntityAnim( tinyxml2::XMLElement* animElement , EntityDef* newDef , int index , std::string animName );
	static void		  ParseAllEntitiesOfName( tinyxml2::XMLElement* root , std::string elementName );
	static void		  ParsePortalSpecificData( tinyxml2::XMLElement* elementRoot , EntityDef* definition );
		
	//--------------------------------------------------------------------------------------------------------------------------------------------

	static EntityDef* GetDefinition( std::string const& entityDefName );

public:
	static std::map<std::string , EntityDef*>		s_entityDefs;
	
	std::string										m_typeName;
	std::string										m_className;
	SpriteSheet*									m_spriteSheet											= nullptr;
	
	std::map<std::string , SpriteAnimDefinition*>	m_entityAnims[ NUM_TOTAL_ENTITY_ANIMS ];

	float											m_physicsRadius		= 0.0f;
	float											m_height			= 0.0f;
	float											m_eyeHeight			= 0.0f;
	Vec2											m_spriteSize		= Vec2::ONE;
	bool											m_pushesEntities	= false;
	bool											m_pushedByEntities	= false;
	bool											m_pushedByWalls		= false;
	float											m_mass				= 1.f;
	IntVec2											m_damage			= IntVec2::ZERO;
	float											m_speed				= 0.0f;
	eBillBoardType									m_billboard			= CAMERA_FACING_XY;

};

//--------------------------------------------------------------------------------------------------------------------------------------------