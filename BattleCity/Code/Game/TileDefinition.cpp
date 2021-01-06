#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------
//			STATIC VARIABLES DEFINITION
//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC std::map< std::string , TileDefinition* > TileDefinition::s_definitions;
STATIC SpriteSheet* TileDefinition::s_tileSS;

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition::TileDefinition( const tinyxml2::XMLElement& definitionXMLElement )
{
	m_name					= ParseXmlAttribute( definitionXMLElement , "name" , m_name );
	m_tint					= ParseXmlAttribute( definitionXMLElement , "colorInImageFile" , m_tint );
	IntVec2 m_spriteCoords	= ParseXmlAttribute( definitionXMLElement , "spriteCoords" , m_spriteCoords );
	m_isSolid				= ParseXmlAttribute( definitionXMLElement , "isSolid" , m_isSolid );

	const SpriteDefinition& currentTileSprite = s_tileSS->GetSpriteDefinition( m_spriteCoords );
	currentTileSprite.GetUVs( m_minUVs , m_maxUVs );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition::~TileDefinition()
{
	SAFE_RELEASE_POINTER( s_tileSS );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileDefinition::CreateTileDefinitions( const char* xmlFilePath )
{
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
		
	if ( ( tileSheetFileName == "" ) || ( tileSpriteSheetDimensions.x == 0 && tileSpriteSheetDimensions.y == 0 ) )
	{
		ERROR_AND_DIE( "You forgot To mention the filePath for the TileSpriteSheet or it's Grid Layout" );
	}
	
	const char* filePath = tileSheetFileName.c_str();
	
	if( s_tileSS == nullptr )
	{
		s_tileSS = new SpriteSheet( *( g_theRenderer->GetOrCreateTextureFromFile( filePath ) ) , tileSpriteSheetDimensions );
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

//--------------------------------------------------------------------------------------------------------------------------------------------