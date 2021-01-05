#include "Game/ActorDefs.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

std::map< std::string , ActorDefinition* > ActorDefinition::s_definitions;

extern SpriteSheet*  g_characterSpriteSheet;
extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

ActorDefinition::ActorDefinition( const tinyxml2::XMLElement& definitionXMLElement )
{
	//m_actorDefs.PopulateFromXmlElementAttributes( definitionXMLElement );
	m_name				= ParseXmlAttribute( definitionXMLElement , "name" , "Actor" );
	m_faction			= ParseXmlAttribute( definitionXMLElement , "faction" , "INVALID" );
	m_physicsRadius		= ParseXmlAttribute( definitionXMLElement , "physicsRadius"	, 1.f );
	m_localDrawBounds	= ParseXmlAttribute( definitionXMLElement , "localDrawBounds" , AABB2::ZERO_TO_ONE );
	m_startPosition		= ParseXmlAttribute( definitionXMLElement , "startPosition" , Vec2::ZERO );
	m_defaultVelocity	= ParseXmlAttribute( definitionXMLElement , "defaultVelocity" , Vec2::ZERO );
	m_canWalk			= ParseXmlAttribute( definitionXMLElement , "canWalk"   , false );
	m_canSwim			= ParseXmlAttribute( definitionXMLElement , "canSwim"   , false );
	m_canFly			= ParseXmlAttribute( definitionXMLElement , "canFly"    , false );
	m_isDead			= ParseXmlAttribute( definitionXMLElement , "isDead"    , false );
	m_isGarbage			= ParseXmlAttribute( definitionXMLElement , "isGarbage" , false );
	m_spriteCoords		= ParseXmlAttribute( definitionXMLElement , "SpriteCoords" , IntVec2( 7 , 0 ) );

	int spriteSheetWidth = g_characterSpriteSheet->GetTexture().GetDimensions().x / g_characterSpriteSheet->GetSpriteDimension().x;
	int tileSpriteIndex = m_spriteCoords.x + ( spriteSheetWidth * m_spriteCoords.y );
	const SpriteDefinition& currentTileSprite = g_characterSpriteSheet->GetSpriteDefinition( tileSpriteIndex );
	currentTileSprite.GetUVs( m_spriteUVs.m_mins , m_spriteUVs.m_maxs );

	const XMLElement* animElement = definitionXMLElement.FirstChildElement("SpriteAnimSet");

	m_animSet = new SpriteAnimSet( *g_theRenderer , *animElement );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ActorDefinition::CreateActorDefinitions( const char* xmlFilePath )
{
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );

	if ( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		ERROR_AND_DIE( "XML FILE DID NOT LOAD" );
		return;
	}

	tinyxml2::XMLElement* actorDefinition = xmlDocument.RootElement();
	actorDefinition = actorDefinition->FirstChildElement();

	while ( actorDefinition )
	{
		ActorDefinition* currentActorDefiniton = new ActorDefinition( *actorDefinition );
		std::string key = ParseXmlAttribute( *actorDefinition , "name" , "Invalid Name" );
		s_definitions[ key ] = currentActorDefiniton;
		actorDefinition = actorDefinition->NextSiblingElement();
	}
}

