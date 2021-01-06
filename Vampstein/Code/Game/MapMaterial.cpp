#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameCommon.hpp"
#include "Game/MapMaterial.hpp"

#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC std::map<std::string , MaterialSheet*>	MaterialSheet::s_materialSheets;
STATIC std::map<std::string , MapMaterial*>		MapMaterial::s_mapMaterials;

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapMaterial::GetUVCoords( Vec2& outMins , Vec2& outMax )
{
	SpriteSheet* sheet = GetDiffuseSpriteSheet();

	IntVec2 spriteSheetLayout = sheet->GetSpriteDimension();
	int spriteIndex = m_spriteCoords.y * spriteSheetLayout.x + m_spriteCoords.x;

	sheet->GetSpriteUVs( outMins , outMax , spriteIndex );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MaterialSheet::MaterialSheet( XMLElement* element )
{
	m_name								= ParseXmlAttribute( *element , "name" , "NULL" );
	
	if ( m_name == "NULL" )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
									  "ERROR: MaterialsSheet name : %s; is not present or invalid." ,
									  m_name.c_str() );
	}

	g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG ,"MaterialsSheet name : %s was found." , m_name.c_str() );
	
	m_layout							= ParseXmlAttribute( *element , "layout" , -IntVec2::ONE );
	
	if ( m_layout == -IntVec2::ONE )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
									  "ERROR: MaterialsSheet layout : ( %d , %d ) is not present or invalid." ,
									  m_layout.x , m_layout.y );
	}

	g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG ,"MaterialsSheet layout : ( %d , %d ) was found." , m_layout.x , m_layout.y );

	XMLElement* diffuseElement = element->FirstChildElement( "Diffuse" );

	if ( diffuseElement != nullptr )
	{
		std::string diffuseSSFilePath	= ParseXmlAttribute( *diffuseElement ,"image", "NULL" );
		
		if ( diffuseSSFilePath == "NULL" )
		{
			g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
										  "ERROR: Diffuse sprite sheet name : %s; is not present or invalid." ,
										  diffuseSSFilePath.c_str() );
		}
		else
		{
			Texture* diffuseSSTexture	= g_theRenderer->GetOrCreateTextureFromFile( diffuseSSFilePath.c_str() );
			m_diffuseSpriteSheet		= new SpriteSheet( *diffuseSSTexture , m_layout );

			g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Diffuse Materials Sheet : %s was found." , diffuseSSFilePath.c_str() );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MaterialSheet* MaterialSheet::GetMaterialSheet( std::string sheetName )
{
	auto materialSheet = MaterialSheet::s_materialSheets.find( sheetName );
	
	if ( materialSheet != MaterialSheet::s_materialSheets.end() )
	{
		return materialSheet->second;
	}

	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MapMaterial::MapMaterial( XMLElement* element )
{
	m_name	= ParseXmlAttribute( *element , "name" , "NULL" );
	
	if ( m_name == "NULL" )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
									  "ERROR: Material name : %s; is not present or invalid." ,
									  m_name.c_str() );
	}

	m_sheetName = ParseXmlAttribute( *element , "sheet" , "NULL" );
	
	if ( m_sheetName == "NULL" )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
									  "ERROR: Material Sheet name : %s; is not present or invalid." ,
									  m_sheetName.c_str() );
	}

	m_spriteCoords = ParseXmlAttribute( *element , "spriteCoords" , -IntVec2::ONE );
	
	if ( m_spriteCoords == -IntVec2::ONE )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
									  "ERROR: Material sprite coords : ( %d , %d ); is not present or invalid." ,
									  m_spriteCoords.x , m_spriteCoords.y );
	}	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool MapMaterial::LoadDefinitions( const char* filePath )
{
	tinyxml2::XMLDocument doc;
	bool loadFile = doc.LoadFile( filePath );

	std::string materialfilePath = filePath;
	if( loadFile != tinyxml2::XMLError::XML_SUCCESS )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "ERROR: Map Material Types file at path: %s not present." ,
		                              materialfilePath.c_str() );
		return false;
	}
	
	XMLElement* root = doc.RootElement();
	std::string rootElementName = root->Name();
	if( rootElementName != "MapMaterialTypes" )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
		                              "ERROR: Map Material Types at path: %s is not using MapMaterialTypes as Root Element." ,
		                              materialfilePath.c_str() );
		return false;
	}

	g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG ,
								  "LOG: Map Material Types at path: %s now parsing ..." ,
								  materialfilePath.c_str() );
	
	for( XMLElement* element = root->FirstChildElement( "MaterialsSheet" ); element != nullptr; element = element->NextSiblingElement( "MaterialSheet" ) )
	{
		MaterialSheet* newSheet				= new MaterialSheet( element );

		auto materialSheet	= MaterialSheet::s_materialSheets.find( newSheet->m_name );

		if ( materialSheet != MaterialSheet::s_materialSheets.end() )
		{
			g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
										  "ERROR: Map Material Sheet : %s has multiple Definitions." ,
										  newSheet->m_name.c_str() );
			g_theDevConsole->PrintString( DEVCONSOLE_WARNING ,
										  "WARNING: (Using the First Definition)." ,
										  newSheet->m_name.c_str() );
			continue;
		}

		MaterialSheet::s_materialSheets[ newSheet->m_name ] = newSheet;
	}

	for( XMLElement* element = root->FirstChildElement( "MaterialType" ); element != nullptr; element = element->NextSiblingElement( "MaterialType" ) )
	{
		MapMaterial* newMaterial			= new MapMaterial( element );
		
		auto mapMaterial	= MapMaterial::s_mapMaterials.find( newMaterial->m_name );

		if ( mapMaterial != MapMaterial::s_mapMaterials.end() )
		{
			g_theDevConsole->PrintString( DEVCONSOLE_ERROR ,
										  "ERROR: Map Material : %s has multiple Definitions." ,
										  newMaterial->m_name.c_str() );
			g_theDevConsole->PrintString( DEVCONSOLE_WARNING ,
										  "WARNING: (Using the First Definition)." ,
										  newMaterial->m_name.c_str() );
			continue;
		}

		MapMaterial::s_mapMaterials[ newMaterial->m_name ] = newMaterial;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

MapMaterial* MapMaterial::GetDefinition( std::string materialName )
{
	auto mapMaterial = MapMaterial::s_mapMaterials.find( materialName );
	
	if ( mapMaterial != MapMaterial::s_mapMaterials.end() )
	{
		return mapMaterial->second;
	}

	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

SpriteSheet* MapMaterial::GetDiffuseSpriteSheet()
{
	SpriteSheet* sheet = nullptr;

	sheet = MaterialSheet::GetMaterialSheet( m_sheetName )->GetSpriteSheet();
	return sheet;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
