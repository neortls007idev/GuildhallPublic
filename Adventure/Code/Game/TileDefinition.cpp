#include "Game/TileDefinition.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern SpriteSheet* g_tileSpriteSheet;

//--------------------------------------------------------------------------------------------------------------------------------------------

std::map< std::string , TileDefinition* > TileDefinition::s_definitions;

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition::TileDefinition( const tinyxml2::XMLElement& definitionXMLElement )
{
	//m_tileProperties.PopulateFromXmlElementAttributes( definitionXMLElement );
	m_name		   = ParseXmlAttribute( definitionXMLElement , "name" , m_name );
	m_allowsSight  = ParseXmlAttribute( definitionXMLElement , "allowsSight" , m_allowsSight	);
	m_allowsSwim   = ParseXmlAttribute( definitionXMLElement , "allowsSwim"  , m_allowsSwim		);
	m_allowsWalk   = ParseXmlAttribute( definitionXMLElement , "allowsWalk"  , m_allowsWalk		);
	m_allowsFly    = ParseXmlAttribute( definitionXMLElement , "allowsFly"   , m_allowsFly		);
	m_spriteCoords = ParseXmlAttribute( definitionXMLElement , "spriteCoords", m_spriteCoords	);
	m_tileColor    = ParseXmlAttribute( definitionXMLElement , "colorInImageFile"  , m_tileColor		);

	// TODO - below line of code needs further testing.
	int spriteSheetWidth = g_tileSpriteSheet->GetTexture().GetDimensions().x / g_tileSpriteSheet->GetSpriteDimension().x;
	int tileSpriteIndex = m_spriteCoords.x + ( spriteSheetWidth * m_spriteCoords.y );
	const SpriteDefinition& currentTileSprite = g_tileSpriteSheet->GetSpriteDefinition( tileSpriteIndex );
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

	std::string terrainTexturePath = "Data/Images/";

	//g_tileSpriteSheet
	 
	tinyxml2::XMLElement* tileDefinition = xmlDocument.RootElement();
	std::string terrainFileName = ParseXmlAttribute( *tileDefinition , "spriteSheet" , "" );
	IntVec2 terrainSpriteSheetDimensions = ParseXmlAttribute( *tileDefinition , "spriteLayout" , IntVec2::ZERO );
	
	if ( ( terrainFileName == "" ) || ( terrainSpriteSheetDimensions.x == 0 && terrainSpriteSheetDimensions.y == 0 ) )
	{
		ERROR_AND_DIE( "You forgot To mention the filePath for the TileSpreadSheet or it's Grid Layout" );
	}
	
	terrainTexturePath.append(terrainFileName);
	const char* filePath = terrainTexturePath.c_str();
	if ( g_tileSpriteSheet == NULL )
	{
		g_tileSpriteSheet = new SpriteSheet( *( g_theRenderer->GetOrCreateTextureFromFile( filePath ) ) , terrainSpriteSheetDimensions );
	}

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