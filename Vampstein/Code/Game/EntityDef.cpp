#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/EntityDef.hpp"

#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*					  g_theRenderer;
STATIC std::map<std::string , EntityDef*> EntityDef::s_entityDefs;

//--------------------------------------------------------------------------------------------------------------------------------------------

EntityDef::EntityDef()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC void EntityDef::CreateDefinitions( char const* dataFilePath )
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( dataFilePath );

	tinyxml2::XMLElement* root = doc.RootElement();

	ParseAllActors( root );

	//tinyxml2::XMLElement* entityElement = root->FirstChildElement();
	ParseAllEntitiesOfName( root , "Projectile" );
	ParseAllEntitiesOfName( root , "Portal" );
	ParseAllEntitiesOfName( root , "Entity" );


	//--------------------------------------------------------------------------------
	//			NOTES
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// DO NOT OVER RESOLVE ERRORS LIKE THIS THAT CAN BE CREATED BY LDs WHILE USING THE XML FILES
	//--------------------------------------------------------------------------------
	
	/*std::vector<std::string> entityTypes;
	entityTypes.push_back( "Actor" );
	
	while ( nullptr != entityElement )
	{
		std::string entityName = entityElement->Name();

		size_t index = 0;
		for ( index = 0 ; index < entityTypes.size() ; index++ )
		{
			if( entityName == entityTypes[ index ] )
			{
				entityElement = entityElement->NextSiblingElement();
				break;
			}
		}
		if ( index == entityTypes.size() )
		{
			ParseAllEntitiesOfName( root , entityName );
			entityTypes.push_back( entityName );
		}
	}*/
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC void EntityDef::ParseAllEntitiesOfName( tinyxml2::XMLElement* root , std::string elementName )
{
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			PARSING ALL ENTITIES WITH NAME
	//--------------------------------------------------------------------------------------------------------------------------------------------

	for( tinyxml2::XMLElement* element = root->FirstChildElement( elementName.c_str() ); element != nullptr; element = element->NextSiblingElement( elementName.c_str() ) )
	{
		std::string name			= ParseXmlAttribute( *element , "name" , "invalid" );

		if( name == "invalid" )
		{
			g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
										  "ERROR: Enttiy name attribute missing, skipping this Entity." );
			continue;
		}

		EntityDef* newDef = new EntityDef();
		tinyxml2::XMLElement* physicsEle = element->FirstChildElement( "Physics" );

		if( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.0f );
			newDef->m_height		= ParseXmlAttribute( *physicsEle , "height" , 0.0f );
			newDef->m_speed			= ParseXmlAttribute( *physicsEle , "speed"  , 0.f );
		}

		tinyxml2::XMLElement* gamePlayEle = element->FirstChildElement( "Gameplay" );

		if( gamePlayEle != nullptr )
		{
			newDef->m_damage		= ParseXmlAttribute( *gamePlayEle , "damage" , IntVec2::ZERO );
		}

		std::string portalElement = element->Name();
		if ( portalElement == "Portal" )
		{
			ParsePortalSpecificData( element , newDef );
		}

		newDef->m_className			= elementName;
		newDef->m_typeName			= name;

		s_entityDefs[ name ]		= newDef;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void EntityDef::ParsePortalSpecificData( tinyxml2::XMLElement* elementRoot , EntityDef* definition )
{
	tinyxml2::XMLElement* appereanceEle = elementRoot->FirstChildElement( "Appearance" );
	if( appereanceEle != nullptr )
	{
		std::string spritePath = ParseXmlAttribute( *appereanceEle , "spriteSheet" , "null" );
		IntVec2 spriteLayout = ParseXmlAttribute( *appereanceEle , "layout" , IntVec2( 0 , 0 ) );

		if( spritePath != "null" && spriteLayout != IntVec2( 0 , 0 ) )
		{
			Texture* spriteTex = g_theRenderer->GetOrCreateTextureFromFile( spritePath.c_str() );
			definition->m_spriteSheet = new SpriteSheet( *spriteTex , spriteLayout );
		}
		//std::string texturePath = ParseXmlAttribute( *appereanceEle , "Texture" , "null" );
		//
		//if( texturePath != "null" )
		//{
		//	Texture* spriteTex = g_theRenderer->GetOrCreateTextureFromFile( texturePath.c_str() );
		//	definition->m_texture = spriteTex;
		//}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC void EntityDef::ParseAllActors( tinyxml2::XMLElement* root )
{
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			PARSING ALL ACTORS
	//--------------------------------------------------------------------------------------------------------------------------------------------

	for( tinyxml2::XMLElement* element = root->FirstChildElement( "Actor" ); element != nullptr; element = element->NextSiblingElement( "Actor" ) )
	{
		std::string name = ParseXmlAttribute( *element , "name" , "invalid" );

		if( name == "invalid" )
		{
			//error
			continue;
		}

		EntityDef* newDef = new EntityDef();

		tinyxml2::XMLElement* physicsEle = element->FirstChildElement( "Physics" );

		if( physicsEle != nullptr )
		{
			newDef->m_physicsRadius = ParseXmlAttribute( *physicsEle , "radius" , 0.f );
			newDef->m_height = ParseXmlAttribute( *physicsEle , "height" , 0.f );
			newDef->m_eyeHeight = ParseXmlAttribute( *physicsEle , "eyeHeight" , 0.f );
			newDef->m_speed = ParseXmlAttribute( *physicsEle , "walkSpeed" , 0.0f );
		}

		tinyxml2::XMLElement* appereanceElement = element->FirstChildElement( "Appearance" );

		if( appereanceElement != nullptr )
		{
			newDef->m_spriteSize = ParseXmlAttribute( *appereanceElement , "size" , Vec2( 0.f , 0.f ) );
			std::string billboardType = ParseXmlAttribute( *appereanceElement , "billboard" , "invalid" );

			if( billboardType != "invalid" )
			{
				if( billboardType == "CameraFacingXY" )
				{
					newDef->m_billboard = CAMERA_FACING_XY;
				}
				else if( billboardType == "CameraOpposedXY" )
				{
					newDef->m_billboard = CAMERA_OPPOSED_XY;
				}
				else if( billboardType == "CameraFacingXYZ" )
				{
					newDef->m_billboard = CAMERA_FACING_XYZ;
				}
				else if( billboardType == "CameraOpposedXYZ" )
				{
					newDef->m_billboard = CAMERA_OPPOSED_XYZ;
				}
			}

			std::string spritePath = ParseXmlAttribute( *appereanceElement , "spriteSheet" , "invalid" );
			IntVec2		spriteLayout = ParseXmlAttribute( *appereanceElement , "layout" , IntVec2::ZERO );

			if( spritePath != "invalid" && spriteLayout != IntVec2( 0 , 0 ) )
			{
				Texture* spriteTex = g_theRenderer->GetOrCreateTextureFromFile( spritePath.c_str() );
				newDef->m_spriteSheet = new SpriteSheet( *spriteTex , spriteLayout );
			}

			if( newDef->m_spriteSheet != nullptr )
			{
				int index = 0;
				
				for( tinyxml2::XMLElement* animElement = appereanceElement->FirstChildElement() ; animElement != nullptr ; animElement = animElement->NextSiblingElement() )
				{
					if( animElement == nullptr )
					{
						continue;
					}
					std::string animName = animElement->Name();
					index = GetIndexForAnimName( animName );
					ParseEntityAnim( animElement , newDef , index , "front" );
					ParseEntityAnim( animElement , newDef , index , "back" );
					ParseEntityAnim( animElement , newDef , index , "left" );
					ParseEntityAnim( animElement , newDef , index , "right" );
					ParseEntityAnim( animElement , newDef , index , "frontLeft" );
					ParseEntityAnim( animElement , newDef , index , "frontRight" );
					ParseEntityAnim( animElement , newDef , index , "backLeft" );
					ParseEntityAnim( animElement , newDef , index , "backRight" );
				}
			}
		}

		newDef->m_className = "Actor";
		newDef->m_typeName = name;

		s_entityDefs[ name ] = newDef;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC void EntityDef::ParseEntityAnim( tinyxml2::XMLElement* animElement , EntityDef* newDef , int index , std::string animName )
{
	std::string animationName = ParseXmlAttribute( *animElement , animName.c_str() , "invalid" );

	if( animationName != "invalid" )
	{
		Strings nums = SplitStringAtGivenDelimiter( animationName , ',' );

		newDef->m_entityAnims[ index ][ animName ] = new SpriteAnimDefinition(*( newDef->m_spriteSheet ) ,
																   StringConvertToValue( nums[ 0 ].c_str() , 0 ) ,
																	 StringConvertToValue( nums[ nums.size() - 1 ].c_str() , 0 ) ,
																	1.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC int EntityDef::GetIndexForAnimName( std::string animName )
{
	if ( animName == "Walk" )																{ return ( int ) ENTITY_WALK_ANIM; }
	if ( animName == "Attack" )																{ return ( int ) ENTITY_ATTACK_ANIM; }
	if ( animName == "Pain" )																{ return ( int ) ENTITY_PAIN_ANIM; }
	if ( animName == "Death" )																{ return ( int ) ENTITY_DEATH_ANIM; }

	g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
								  "ERROR: Parsing animset %s | PLEASE USE - Walk,Attack,Pain or-and Death Only" , animName.c_str() );
	return (int) ENTITY_INVALID_ANIM;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC EntityDef* EntityDef::GetDefinition( std::string const& entityDefName )
{
	EntityDef* entityDef = s_entityDefs[ entityDefName ];
	
	if ( nullptr != entityDef )
	{
		return s_entityDefs[ entityDefName ];
	}
	
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
