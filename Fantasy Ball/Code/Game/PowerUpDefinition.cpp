#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Game.hpp"
#include "Game/PowerUpDefinition.hpp"
#include "Game/PowerUps.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern Game*			g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

std::map< std::string , PowerUpDefinition* > PowerUpDefinition::s_definitions;

//--------------------------------------------------------------------------------------------------------------------------------------------

PowerUpDefinition::PowerUpDefinition( const tinyxml2::XMLElement& definitionXMLElement )
{
	m_name							= ParseXmlAttribute( definitionXMLElement , "name"							, m_name );
	m_spriteCoords					= ParseXmlAttribute( definitionXMLElement , "spriteCoords"					, m_spriteCoords	);
	m_color							= ParseXmlAttribute( definitionXMLElement , "color"							, m_color		);
	m_spawnProbablityRequirement	= ParseXmlAttribute( definitionXMLElement , "spawnProbablityRequirement"		, m_spawnProbablityRequirement );

	const SpriteDefinition& currentTileSprite	= g_theGame->m_gameSS[ SS_POWERUPS ]->GetSpriteDefinition( m_spriteCoords );
	currentTileSprite.GetUVs( m_spriteUVs.m_mins , m_spriteUVs.m_maxs );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PowerUpDefinition::CreatePowerUpDefinitions( const char* xmlFilePath )
{
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );

	if ( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		ERROR_AND_DIE( "XML FILE DID NOT LOAD" );
		return;
	}
	 
	tinyxml2::XMLElement* powerupDefinition	= xmlDocument.RootElement();
	std::string powerUpSheetFileName		= ParseXmlAttribute( *powerupDefinition , "PowerUpSpriteSheet"	, "" );
	IntVec2 powerUpSpriteSheetDimensions	= ParseXmlAttribute( *powerupDefinition , "PowerUpSpriteLayout"	, IntVec2::ZERO );
		
	if ( ( powerUpSheetFileName == "" ) || ( powerUpSpriteSheetDimensions.x == 0 && powerUpSpriteSheetDimensions.y == 0 ) )
	{
		ERROR_AND_DIE( "You forgot To mention the filePath for the Powerup SpriteSheet or it's Grid Layout" );
	}
	
	const char* filePath = powerUpSheetFileName.c_str();
	
	if( g_theGame->m_gameSS[ SS_POWERUPS ] == nullptr )
	{
		g_theGame->m_gameSS[ SS_POWERUPS ] = new SpriteSheet( *( g_theRenderer->GetOrCreateTextureFromFile( filePath ) ) , powerUpSpriteSheetDimensions );
	}
	
	powerupDefinition = powerupDefinition->FirstChildElement( "PowerUpDefinition" );

	while ( powerupDefinition )
	{
		PowerUpDefinition* currentTileDefiniton = new PowerUpDefinition( *powerupDefinition );

		std::string key			= ParseXmlAttribute( *powerupDefinition , "name" , "Invalid Name" );
		s_definitions[ key ]	= currentTileDefiniton;
		powerupDefinition		= powerupDefinition->NextSiblingElement();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

PowerUpDefinition* ParseXmlAttribute( const tinyxml2::XMLElement& element , const char* attributeName , PowerUpDefinition* defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	PowerUpDefinition* value = defaultValue;

	if ( attributeValueText )
	{
		value = PowerUpDefinition::s_definitions[ attributeValueText ];
	}

	return value;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC PowerUpDefinition* PowerUpDefinition::GetPowerUpDefinitionForPowerUp( ePowerUpType type )
{
	PowerUpDefinition* definition = nullptr;

	switch ( type )
	{
		case PT_BALLX2				:
			{
				definition = s_definitions.at( "2xBall" );
				return definition;
			}break;

		case PT_BALLX3				:
			{
				definition = s_definitions.at( "3xBall" );
				return definition;
			}break;

		case PT_BALLX8				:
			{
				definition = s_definitions.at( "8xBall" );
				return definition;
			}break;
		
//		case PT_BALL_SIZE_UP		:
//			{
//				//definition = s_definitions.at( "2xBall" );
//				return definition;
//			}break;
//		
//		case PT_BALL_SIZE_DOWN		:
//			{
//				//definition = s_definitions.at( "2xBall" );
//				return definition;
//			}break;
//		
//		case PT_BALL_SIZE_MEGA		:
//			{
//				//definition = s_definitions.at( "2xBall" );
//				return definition;
//			}break;
//		
//		case PT_BALL_SIZE_SHRINK	:
//			{
//				//definition = s_definitions.at( "2xBall" );
//				return definition;
//			}break;
		
		case PT_BRICK_THROUGH		:
			{
				definition = s_definitions.at( "BrickThrough" );
				return definition;
			}break;

		case PT_BRICK_ALL_VISIBLE:
			{
				definition = s_definitions.at( "AllVisible" );
				return definition;
			}break;

		case PT_BRICK_ALL_SOFT:
			{
				definition = s_definitions.at( "Allsoft" );
				return definition;
			}break;

		case PT_EXTRA_LIFE:
			{
				definition = s_definitions.at( "ExtraLife" );
				return definition;
			}break;

		case PT_DEATH:
			{
				definition = s_definitions.at( "Death" );
				return definition;
			}break;
	}

	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------