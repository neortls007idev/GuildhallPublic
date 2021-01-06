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

	Camera* gameCamera		= g_theGame->GetWorldCamera();
	Vec2 cameraMins			= gameCamera->GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs			= gameCamera->GetOrthoMax().GetXYComponents();

	AABB2 cameraArea		= AABB2( cameraMins , cameraMaxs );
	Vec2 cameraDimensions	= cameraArea.GetDimensions();
		
	IntVec2 tileCoords = IntVec2::ZERO;

	Vec2 startpointatbottomleft( -845.f , -168.f );
	startpointatbottomleft.x += halfTileDimensions.x;
	const float xOffset = ( float )RoundDownToInt( ( ( cameraDimensions.x * 0.5f - map.m_leftWallCosmeticBounds.GetDimensions().x ) / map.m_dimensions.x ) + halfTileDimensions.x );
	const float xHalfOffset = xOffset * 0.5f;
	const float yOffset = ( cameraDimensions.y * 0.5f ) / map.m_dimensions.y;
	
	for ( tileCoords.y = 0; ( tileCoords.y < map.m_dimensions.y ) && ( tileCoords.y < m_mutationImage->GetDimensions().y ); tileCoords.y++ )
	{
		for ( tileCoords.x = 0;( tileCoords.x <  map.m_dimensions.x ) && ( tileCoords.x < m_mutationImage->GetDimensions().x ); tileCoords.x++ )
		{		
			TileDefinition* curTileTypeInImage = DoesRGBA8AtpixelCoordsMatchAnyTileDefinition( tileCoords );
			if ( curTileTypeInImage )
			{
				map.SpawnNewEntity( eEntityType::TILE ,
				                    startpointatbottomleft + Vec2( tileCoords.x * xOffset , tileCoords.y * yOffset ) ,
									Vec2::ZERO ,
				                    curTileTypeInImage );
				map.m_numAliveTiles++;
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
		if ( index->second->m_tileColor == colorAtPixelCoords )
		{
			return index->second;
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
