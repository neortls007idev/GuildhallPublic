#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern Game*			g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

std::map< std::string , TileDefinition* > TileDefinition::s_definitions;

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition::TileDefinition( const tinyxml2::XMLElement& definitionXMLElement )
{
	m_name				= ParseXmlAttribute( definitionXMLElement , "name"											, m_name );
	m_health			= ParseXmlAttribute( definitionXMLElement , "health"											, m_health	);
	m_solidAtHealth		= ParseXmlAttribute( definitionXMLElement , "healthToSolid"									, m_solidAtHealth);
	m_isSolid			= ParseXmlAttribute( definitionXMLElement , "isSolid"										, m_isSolid		);
	m_isVisible			= ParseXmlAttribute( definitionXMLElement , "isVisible"										, m_isVisible		);
	m_visibleAtHealth	= ParseXmlAttribute( definitionXMLElement , "visibleAtHealth"								, m_visibleAtHealth );
	m_spriteCoords		= ParseXmlAttribute( definitionXMLElement , "spriteCoords"									, m_spriteCoords	);
	m_VFXSpriteCoords	= ParseXmlAttribute( definitionXMLElement , "VFXSpriteCoords"								, m_VFXSpriteCoords );
	m_ballSpriteCoords	= ParseXmlAttribute( definitionXMLElement , "BallSpriteCoords"								, m_ballSpriteCoords );
	m_tileColor			= ParseXmlAttribute( definitionXMLElement , "colorInImageFile"								, m_tileColor		);
	m_screenshakeMinProbablity = ParseXmlAttribute( definitionXMLElement , "screenshakeMinProbablity"				, m_screenshakeMinProbablity );
	
	const SpriteDefinition& currentTileSprite	= g_theGame->m_gameSS[ SS_BRICKS ]->GetSpriteDefinition( m_spriteCoords );
	currentTileSprite.GetUVs( m_spriteUVs.m_mins , m_spriteUVs.m_maxs );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileDefinition::CreateTileDefinitions( const char* xmlFilePath )
{
	//m_spriteSheet.PopulateFromXmlElementAttributes( "Data/Definitions/Tiles.xml" );

	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );

	if ( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		ERROR_AND_DIE( "XML FILE DID NOT LOAD" );
		return;
	}
	 
	tinyxml2::XMLElement* tileDefinition	= xmlDocument.RootElement();
	std::string tileSheetFileName			= ParseXmlAttribute( *tileDefinition , "tileSpriteSheet" , "" );
	IntVec2 tileSpriteSheetDimensions		= ParseXmlAttribute( *tileDefinition , "tileSpriteLayout" , IntVec2::ZERO );
	
	std::string tileVFXSheetFileName		= ParseXmlAttribute( *tileDefinition , "tileVFXSpriteSheet" , "" );
	IntVec2 tileVFXSpriteSheetDimensions	= ParseXmlAttribute( *tileDefinition , "tileVFXSpriteLayout" , IntVec2::ZERO );

	std::string ballSheetFileName			= ParseXmlAttribute( *tileDefinition , "BallSpriteSheet" , "" );
	IntVec2 ballSpriteSheetDimensions		= ParseXmlAttribute( *tileDefinition , "BallSpriteLayout" , IntVec2::ZERO );
	IntVec2 defaultBallSpriteCoOrds			= ParseXmlAttribute( *tileDefinition , "BallDefaultSpriteCoords" , -IntVec2::ONE );
	
	if ( ( tileSheetFileName == "" ) || ( tileSpriteSheetDimensions.x == 0 && tileSpriteSheetDimensions.y == 0 ) )
	{
		ERROR_AND_DIE( "You forgot To mention the filePath for the TileSpriteSheet or it's Grid Layout" );
	}
	
	const char* filePath = tileSheetFileName.c_str();
	
	if( g_theGame->m_gameSS[ SS_BRICKS ] == nullptr )
	{
		g_theGame->m_gameSS[ SS_BRICKS ] = new SpriteSheet( *( g_theRenderer->GetOrCreateTextureFromFile( filePath ) ) , tileSpriteSheetDimensions );
	}

	if( ( tileVFXSheetFileName == "" ) || ( tileVFXSpriteSheetDimensions.x == 0 && tileVFXSpriteSheetDimensions.y == 0 ) )
	{
		ERROR_AND_DIE( "You forgot To mention the filePath for the TileVFXSpriteSheet or it's Grid Layout" );
	}

			filePath = tileVFXSheetFileName.c_str();

	if( g_theGame->m_gameSS[ SS_VFX_FLARE ] == nullptr )
	{
		g_theGame->m_gameSS[ SS_VFX_FLARE ] = new SpriteSheet( *( g_theRenderer->GetOrCreateTextureFromFile( filePath ) ) , tileVFXSpriteSheetDimensions );
	}

	if( ( ballSheetFileName == "" ) || ( ballSpriteSheetDimensions.x == 0 && ballSpriteSheetDimensions.y == 0 ) )
	{
		ERROR_AND_DIE( "You forgot To mention the filePath for the BallSpriteSheet or it's Grid Layout" );
	}

	filePath = ballSheetFileName.c_str();

	if( g_theGame->m_gameSS[ SS_BALL ] == nullptr )
	{
		g_theGame->m_gameSS[ SS_BALL ] = new SpriteSheet( *( g_theRenderer->GetOrCreateTextureFromFile( filePath ) ) , ballSpriteSheetDimensions );
	}

	if( defaultBallSpriteCoOrds.x == -1 && defaultBallSpriteCoOrds.y == -1 )
	{
		ERROR_AND_DIE( "You forgot To mention the default Ball Sprite." );
	}
	
	g_theGame->m_ballDefaultSpriteCoords = defaultBallSpriteCoOrds;
	
	tileDefinition = tileDefinition->FirstChildElement( "TileDefinition" );

	while ( tileDefinition )
	{
		TileDefinition* currentTileDefiniton = new TileDefinition( *tileDefinition );
		std::string key = ParseXmlAttribute( *tileDefinition , "name" , "Invalid Name" );
		s_definitions[ key ] = currentTileDefiniton;
		tileDefinition = tileDefinition->NextSiblingElement();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition* ParseXmlAttribute( const tinyxml2::XMLElement& element , const char* attributeName , TileDefinition* defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	TileDefinition* value = defaultValue;

	if ( attributeValueText )
	{
		value = TileDefinition::s_definitions[ attributeValueText ];
	}

	return value;
}

//--------------------------------------------------------------------------------------------------------------------------------------------