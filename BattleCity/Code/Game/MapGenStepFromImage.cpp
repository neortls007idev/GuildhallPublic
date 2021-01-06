#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/MapGenStepFromImage.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

extern Game* g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

MapGenStepFromImage::MapGenStepFromImage( const XMLElement& element ) : 
																		MapGenStep( element )
{
	std::string imageFilePath = "Data/MapGenImages/";
	std::string imageFileName = ParseXmlAttribute( element , "ImageFileName" , "" );
	if ( imageFileName == "" )
	{
		ERROR_AND_DIE("You forgot to give the image file path correctly");
	}
	imageFilePath.append( imageFileName );
	m_mutationImage = new Image( imageFilePath.c_str() );
	m_mutationImage = m_mutationImage->GetOrCreateImageFromFile( imageFilePath.c_str() );
	
	m_rotations = ParseXmlAttribute( element , "Rotations" , m_rotations );
	m_alignment = ParseXmlAttribute( element , "Alignment" , m_alignment );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MapGenStepFromImage::RunStepOnce( Map& map )
{
	IntVec2 tileCoords = IntVec2::ZERO;
		
	for ( tileCoords.y = 0; ( tileCoords.y < map.m_size.y ) && ( tileCoords.y < m_mutationImage->GetDimensions().y ); tileCoords.y++ )
	{
		for ( tileCoords.x = 0;( tileCoords.x <  map.m_size.x ) && ( tileCoords.x < m_mutationImage->GetDimensions().x ); tileCoords.x++ )
		{	
			Tile& tile = *map.GetTileAtTileCoords( tileCoords );
			TileDefinition* curTileTypeInImage = DoesRGBA8AtpixelCoordsMatchAnyTileDefinition( tileCoords );
			if ( curTileTypeInImage )
			{
				tile.m_tileDef = curTileTypeInImage;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileDefinition* MapGenStepFromImage::DoesRGBA8AtpixelCoordsMatchAnyTileDefinition( IntVec2 pixelCoords )
{
	Rgba8 colorAtPixelCoords = m_mutationImage->GetTexelColor( pixelCoords );

	for ( auto index = TileDefinition::s_definitions.cbegin(); index != TileDefinition::s_definitions.cend(); index++ )
	{
		if ( index->second->m_tint == colorAtPixelCoords )
		{
			return index->second;
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
