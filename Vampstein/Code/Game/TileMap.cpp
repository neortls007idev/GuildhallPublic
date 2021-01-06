#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Primitives/AABB3.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game.hpp"
#include "Game/MapMaterial.hpp"
#include "Game/MapRegion.hpp"
#include "Game/TheApp.hpp"
#include "Game/TileMap.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern DevConsole*			g_theDevConsole;
extern AudioSystem*			g_theAudioSystem;
extern TheApp*				g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------
//			UTILITY FUNCTION FORWARD DECLARATIONS - DEFINED AT THE BOTTOM OF FILE
//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateTopFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
								const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateBottomFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
								const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateNorth_FrontFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
						const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateSouth_BackFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
						const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateWest_LeftFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
						const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateEast_RightFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
						const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );


//--------------------------------------------------------------------------------------------------------------------------------------------
RandomNumberGenerator rng;

//--------------------------------------------------------------------------------------------------------------------------------------------

TileMap::TileMap( char const* mapName , IntVec2 dimensions ) :
																Map( mapName ) ,
																m_dimensions( dimensions )
{
	m_worldMesh			= new GPUMesh( g_theRenderer );
	m_entitesDebugMesh	= new GPUMesh( g_theRenderer );
	m_raytraceDebugMesh = new GPUMesh( g_theRenderer );
	PopulateTiles();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileMap::TileMap( char const* mapName , XMLElement* rootElement ) :
																	Map( mapName ) 
{
	
	XMLElement* root = rootElement;

	m_dimensions = ParseXmlAttribute( *root , "dimensions" , IntVec2( -1 , -1 ) );

	if( m_dimensions == IntVec2( -1 , -1 ) )
	{
		g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_ERROR ,
									  "ERROR: MAP: %s has INVALID dimensions )" );
	}
	else
	{
		g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_SYTEMLOG ,
									  "LOG: MAP: %s has %d rows , %d columns" , mapName , m_dimensions.x , m_dimensions.y );
	}
	PopulateTiles();
	
	XMLElement* legendElement = root->FirstChildElement( "Legend" );
	XMLElement* mapRowElement = root->FirstChildElement( "MapRows" );

	std::map<char , std::string>	legendMap;
	
	if( legendElement != nullptr )
	{
		ParseLegend( legendElement , legendMap );
	}

	if( mapRowElement != nullptr )
	{
		ParseMapRows( mapRowElement , legendMap );
	}
	
	m_worldMesh = new GPUMesh( g_theRenderer );
	m_entitesDebugMesh = new GPUMesh( g_theRenderer );
	m_raytraceDebugMesh = new GPUMesh( g_theRenderer );
	m_parsedSuccessfully = true;

	m_Portals.clear();
	ParseAllMapEntities( rootElement , mapName );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::ParseLegend( XMLElement* legendElement , std::map<char , std::string>& legendMap )
{	
	for( XMLElement* element = legendElement->FirstChildElement( "Tile" ); element != nullptr; element = element->NextSiblingElement( "Tile" ) )
	{
		const char* glyph = element->Attribute( "glyph" );
		std::string regionName = ParseXmlAttribute( *element , "regionType" , "NULL" );
		auto found = legendMap.find( *glyph );

		if( found == legendMap.end() )
		{
			legendMap[ *glyph ] = regionName;
		}
		else
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_ERROR ,
										  "ERROR: GLYPH : %c Was not found" , glyph );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::ParseMapRows( XMLElement* rowElement , std::map<char , std::string>& legendMap )
{
	int numRows = 0;

	for( XMLElement* element = rowElement->LastChildElement( "MapRow" ); element != nullptr; element = element->PreviousSiblingElement( "MapRow" ) )
	{
		std::string rowString = ParseXmlAttribute( *element , "tiles" , "NULL" );

		if( rowString == "NULL" )
		{
			//error cant parse 
		}

		if( rowString.size() != m_dimensions.x )
		{
			//error not enough tiles
		}
		
		for( int mapRowIndex = 0; mapRowIndex < rowString.size(); mapRowIndex++ )
		{
			char rowCharacter = rowString[ mapRowIndex ];
			auto region = legendMap.find( rowCharacter );
			std::string regionName = "";

			if( region != legendMap.end() )
			{
				regionName = region->second;
			}

			const MapRegion* mapRegion = MapRegion::GetDefinition( regionName );
			m_tiles[ numRows ].m_region = mapRegion;
			m_tiles[ numRows ].m_isSolid = mapRegion->IsSolid();
			numRows++;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileMap* TileMap:: CreateTileMapFromXml( char const* mapName , char const* filepath )
{
	tinyxml2::XMLDocument mapDocument;
	mapDocument.LoadFile( filepath );

	XMLElement* root = mapDocument.RootElement();

	IntVec2 dimensions = ParseXmlAttribute( *root , "dimensions" , IntVec2( -1 , -1 ) );

	if( dimensions == IntVec2( -1 , -1 ) )
	{
		//error 
	}

	TileMap* newTileMap = new TileMap( mapName , dimensions );

	XMLElement* legendElement = root->FirstChildElement( "Legend" );
	XMLElement* mapRowElement = root->FirstChildElement( "MapRows" );

	std::map<char , std::string>	legendMap;

	if( legendElement != nullptr )
	{
		newTileMap->ParseLegend( legendElement , legendMap );
	}

	if( mapRowElement != nullptr )
	{
		newTileMap->ParseMapRows( mapRowElement , legendMap );
	}

	return newTileMap;

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::PopulateTiles()
{
	for( int tileYIndex = 0 ; tileYIndex < m_dimensions.y ; tileYIndex++ )
	{
		for( int tileXIndex = 0 ; tileXIndex < m_dimensions.x ; tileXIndex++ )
		{
			Tile newTile = Tile();
			newTile.m_tileCoords = IntVec2( tileXIndex , tileYIndex );
			newTile.m_bounds = AABB2( tileXIndex , tileYIndex , tileXIndex + 1 , tileYIndex + 1 );
			m_tiles.push_back( newTile );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileMap::~TileMap()
{
	delete m_worldMesh;
	m_worldMesh				= nullptr;

	delete m_entitesDebugMesh;
	m_entitesDebugMesh = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::UpdateMeshes()
{
	std::vector< Vertex_PCU >		WorldMeshVerts;
	
	std::vector< VertexMaster >		tempWorldMeshVerts;
	std::vector< uint >				worldMeshIndices;
	
	for ( int tileYIndex = 0 ; tileYIndex < m_dimensions.y ; tileYIndex++ )
	{
		for ( int tileXIndex = 0 ; tileXIndex < m_dimensions.x ; tileXIndex++ )
		{
			int tileIndex = ( m_dimensions.x *  tileYIndex ) + tileXIndex;
			AddVertsForTile( tempWorldMeshVerts , worldMeshIndices , tileIndex );
		}
	}

	VertexMaster::ConvertVertexMasterToVertexPCU( WorldMeshVerts , tempWorldMeshVerts );

	m_worldMesh->UpdateVertices( WorldMeshVerts );
	m_worldMesh->UpdateIndices( worldMeshIndices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::Render() const
{
	Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->DrawMesh( m_worldMesh );
	g_theRenderer->BindTexture( nullptr );

	if ( g_theGame->m_debugDraw )
	{
		for( size_t index = 0 ; index < m_tiles.size() ; index++ )
		{
			if( m_tiles[index].IsSolid() )
			{
				g_theRenderer->DrawUnfilledAABB2( m_tiles[ index ].m_bounds , MAGENTA , 0.1f );
			}
			else
			{
				g_theRenderer->DrawUnfilledAABB2( m_tiles[ index ].m_bounds , GREEN , 0.1f );
			}
		}
	}
	
	std::vector<Vertex_PCU> debug3DMeshVerts;
	std::vector<uint>		debug3DIndices;
	for( size_t index = 0 ; index < m_allEntities.size() ; index++ )
	{
		if( m_allEntities[ index ] != nullptr )
		{
			Entity* entity = m_allEntities[ index ];
			entity->Render();
				
			if( g_theGame->m_debugDraw )
			{
				if( entity != g_theGame->m_player )
				{
					std::vector<Vertex_PCU> line3DMeshVerts;
					std::vector<uint>		line3DIndices;
					CreateCylinder( line3DMeshVerts , line3DIndices , entity->m_physicsRadius , entity->m_pos ,
									entity->m_pos + Vec3( 0.f , 0.f , entity->m_height ) , CYAN , CYAN );

					AppendIndexedVerts( line3DMeshVerts , line3DIndices , debug3DMeshVerts , debug3DIndices );
				}
			}
		}
		if( g_theGame->m_debugDraw )
		{
			g_theRenderer->SetRasterState( WIREFRAME );
			m_entitesDebugMesh->UpdateVertices( debug3DMeshVerts );
			m_entitesDebugMesh->UpdateIndices( debug3DIndices );
			g_theRenderer->DrawMesh( m_entitesDebugMesh );
			g_theRenderer->SetRasterState( FILL_SOLID );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::DebugRenderRaycasts( Vec2 start , Vec2 direction , float distance )
{
	RayCastResult resultEntity  = RayCastToEntities( start , direction , distance );
	Vec2 end = start + direction * distance;

	std::vector<Vertex_PCU> raytraceVerts;
	std::vector<uint>		raytraceIndices;
	CreateCylinder( raytraceVerts , raytraceIndices , 0.005f , Vec3( start , 0.5f ) ,
					Vec3( end , 0.5f ) , HALF_ALPHA_YELLOW , HALF_ALPHA_YELLOW );
	
	if( resultEntity .didImpact )
	{
		std::vector<Vertex_PCU> penetrationVerts;
		std::vector<uint>		penetrationIndices;
		CreateCylinder( penetrationVerts , penetrationIndices , 0.015f , Vec3( resultEntity.startHitPoint , 0.5f ) ,
		                Vec3( resultEntity.endHitPoint , 0.5f ) , MAGENTA , MAGENTA );
		AppendIndexedVerts( penetrationVerts , penetrationIndices , raytraceVerts , raytraceIndices );
	}
	
	RayCastResult resultTiles = RayCastToTiles( start , direction , distance );
	std::vector<Vertex_PCU> resultTilesVerts;
	std::vector<uint>		resultTilesIndices;
	CreateCylinder( resultTilesVerts , resultTilesIndices , 0.005f , Vec3( start , 0.7f ) ,
					Vec3( end , 0.7f ) , HALF_ALPHA_YELLOW , HALF_ALPHA_YELLOW );
	AppendIndexedVerts( resultTilesVerts , resultTilesIndices , raytraceVerts , raytraceIndices );
	
	if( resultTiles.didImpact )
	{
		std::vector<Vertex_PCU> tileImpactPointVerts;
		std::vector<uint>		tileImpactPointIndices;
		CreateUVSphere( 8 , 16 , tileImpactPointVerts , tileImpactPointIndices , 0.1f , Vec3( resultTiles.startHitPoint , 0.7f ) , RED );
		AppendIndexedVerts( tileImpactPointVerts , tileImpactPointIndices , raytraceVerts , raytraceIndices );
	}
	m_raytraceDebugMesh->UpdateVertices( raytraceVerts );
	m_raytraceDebugMesh->UpdateIndices( raytraceIndices );
	
	g_theRenderer->DrawMesh( m_raytraceDebugMesh );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::ResolveCollisions()
{
	Map::ResolveCollisions();
	
	for( size_t index = 0 ; index < m_allEntities.size() ; index++ )
	{
		Entity* entity = m_allEntities[ index ];

		if( nullptr == entity )
		{
			continue;
		}
		
		if( entity->m_canBePushedByWalls )
		{
			IntVec2 currTileCoords = IntVec2( RoundDownToInt( entity->m_pos.x ) , RoundDownToInt( entity->m_pos.y ) );
			ResolveEntityVsTileCollisions( entity , currTileCoords );
		}
	}	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::ResolveEntityVsTileCollisions( Entity* entity , IntVec2 currTileCoords )
{
	IntVec2 northTileCoords( 1 , 0 );
	IntVec2 southTileCoords( -1 , 0 );
	IntVec2 westTileCoords( 0 , 1 );
	IntVec2 eastTileCoords( 0 , -1 );
	IntVec2 southEastTileCoords( -1 , -1 );
	IntVec2 southWestTileCoords( -1 , 1 );
	IntVec2 northEastTileCoords( 1 , -1 );
	IntVec2 northWestTileCoords( 1 , 1 );

	Vec2	entityPos	= entity->m_pos.GetXYComponents();
	float	radius		= entity->m_physicsRadius;
	Tile* tile			= GetTileAtCoords( currTileCoords );

	if ( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}
	
	tile = GetTileAtCoords( currTileCoords + northTileCoords );

	if( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords + northTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}

	tile = GetTileAtCoords( currTileCoords + southTileCoords );

	if( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords + southTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}

	tile = GetTileAtCoords( currTileCoords + westTileCoords );

	if( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords + westTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}
	
	tile = GetTileAtCoords( currTileCoords + eastTileCoords );

	if ( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords + eastTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}

	//----------------------------------------------------------------------------------------------------------

	tile = GetTileAtCoords( currTileCoords + southEastTileCoords );

	if( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords + southEastTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}

	tile = GetTileAtCoords( currTileCoords + southWestTileCoords );

	if( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords + southWestTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}

	tile = GetTileAtCoords( currTileCoords + northEastTileCoords );

	if( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords + northEastTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}

	tile = GetTileAtCoords( currTileCoords + northWestTileCoords );

	if( tile != nullptr )
	{
		if( IsTileSolid( currTileCoords + northWestTileCoords ) )
		{
			PushDiscOutOfAABB( entityPos , radius , tile->m_bounds );
		}
	}

	entity->m_pos = Vec3( entityPos , entity->m_pos.z );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::AddVertsForTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex )
{
	IntVec2 tileCoords = GetTileCoordsForIndex( tileIndex );
	Tile* tile = GetTileAtCoords( tileCoords );
	
	if ( tile->IsSolid() )
	{
		Vec2 minUVs;
		Vec2 maxUVs;
		
		const MapRegion* sideRegion = tile->m_region;
		sideRegion->GetSideMaterial()->GetUVCoords( minUVs , maxUVs );
		
		AddVertsForSolidTile( destinationVerts , destinationIndices , tileIndex , minUVs , maxUVs );
	}
	else
	{
		AddVertsForNonSolidTile( destinationVerts , destinationIndices , tileIndex );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::AddVertsForSolidTile ( std::vector< VertexMaster >& destinationVerts ,
                                     std::vector< uint >& destinationIndices , int tileIndex ,
                                     Vec2 minUVs /*= Vec2::ZERO*/ , Vec2 maxUvs /*= Vec2::ONE*/ )
{
	AABB3 bounds = GetTileBounds( tileIndex );

	std::vector< VertexMaster >		tileMeshVerts;
	std::vector< uint >				tileMeshIndices;

	IntVec2 currTileCoords	= GetTileCoordsForIndex( tileIndex );
	IntVec2 northTileCoords( -1 , 0 );
	IntVec2 southTileCoords( 1 , 0 );
	IntVec2 eastTileCoords( 0 , -1 );
	IntVec2 westTileCoords( 0 , 1 );


	if ( !IsTileSolid( currTileCoords + northTileCoords ) )
	{
		CreateNorth_FrontFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , minUVs , maxUvs );

		AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
							destinationVerts , destinationIndices );
	}

	tileMeshVerts.clear();
	tileMeshIndices.clear();

	if( !IsTileSolid( currTileCoords + southTileCoords ) )
	{
		CreateSouth_BackFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , minUVs , maxUvs );
		
		AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
							destinationVerts , destinationIndices );
	}
		tileMeshVerts.clear();
		tileMeshIndices.clear();

	if( !IsTileSolid( currTileCoords + eastTileCoords ) )
	{
		CreateEast_RightFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , minUVs , maxUvs );

		AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
							destinationVerts , destinationIndices );
	}
		tileMeshVerts.clear();
		tileMeshIndices.clear();
	
	if( !IsTileSolid( currTileCoords + westTileCoords ) )
	{
		CreateWest_LeftFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , minUVs , maxUvs );

		AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
							destinationVerts , destinationIndices );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::AddVertsForNonSolidTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex )
{
	AABB3 bounds = GetTileBounds( tileIndex );
	//m_tiles[ tileIndex ].m_bounds = bounds;
	std::vector< VertexMaster >		tileMeshVerts;
	std::vector< uint >				tileMeshIndices;

	IntVec2 curTileCoords	= GetTileCoordsForIndex( tileIndex );
	Tile* tile				= GetTileAtCoords( curTileCoords );

	Vec2 ceilingMinUVS;
	Vec2 ceilingMaxUVS;

	const MapRegion* ceilingRegion = tile->m_region;
	ceilingRegion->GetCelingMaterial()->GetUVCoords( ceilingMinUVS , ceilingMaxUVS );
	
	CreateTopFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , ceilingMinUVS , ceilingMaxUVS );

	AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
						destinationVerts , destinationIndices );
	
	tileMeshVerts.clear();
	tileMeshIndices.clear();

	Vec2 floorMinUVS;
	Vec2 floorMaxUVS;

	const MapRegion* floorRegion = tile->m_region;
	floorRegion->GetFloorMaterial()->GetUVCoords( floorMinUVS , floorMaxUVS );
	
	CreateBottomFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , floorMinUVS , floorMaxUVS );

	AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
						destinationVerts , destinationIndices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

RayCastResult TileMap::RayCastToEntities( Vec2 start , Vec2 direction , float dist )
{
	std::vector<RayCastResult> results;
	RayCastResult toReturn;
	
	for( size_t index = 0; index < m_allEntities.size(); index++ )
	{
		if( m_allEntities[ index ] != nullptr )
		{
			Vec2 neartstPoint	= GetNearestPointOnLineSegment2D( m_allEntities[ index ]->m_pos.GetXYComponents() , start , start + ( direction * dist ) );
			float distance		= ( neartstPoint - m_allEntities[ index ]->m_pos.GetXYComponents() ).GetLength();

			if ( m_allEntities[index] == g_theGame->m_player )
			{
				continue;
			}
			
			if( distance > m_allEntities[ index ]->m_physicsRadius )
			{
				continue;
			}
			
			float distanceAlongRay = sqrtf( ( m_allEntities[ index ]->m_physicsRadius * m_allEntities[ index ]->m_physicsRadius ) - ( distance * distance ) );

			Vec2 startPoint = m_allEntities[ index ]->m_pos.GetXYComponents() + ( neartstPoint - m_allEntities[ index ]->m_pos.GetXYComponents() ).GetNormalized() * distance;
			RayCastResult result;

			result.didImpact		= true;
			result.startHitPoint	= startPoint - ( direction * distanceAlongRay );
			result.endHitPoint		= startPoint + ( direction * distanceAlongRay );
			result.impactNormal		= ( result.startHitPoint - m_allEntities[ index ]->m_pos.GetXYComponents() ).GetRotated90Degrees().GetNormalized();
			results.push_back( result );
		}
	}

	if( results.size() > 0 )
	{
		if( results.size() > 1 )
		{
			float minDistanceSq = INFINITY;
			size_t resultIndex	= 0;
			
			for ( size_t index = 0 ; index < results.size() ; index++  )
			{
				float distanceSq = ( start - results[ index ].startHitPoint ).GetLengthSquared();

				if ( distanceSq < minDistanceSq )
				{
					resultIndex = index;
					minDistanceSq = distanceSq;
				}
			}
			return results[ resultIndex ];
		}
		else
		{
			return results[ 0 ];
		}
	}
	return toReturn;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

RayCastResult TileMap::RayCastToTiles( Vec2 start , Vec2 direction , float distace )
{
	RayCastResult toReturn;
	IntVec2 currentTileCoords = IntVec2( ( int ) ( floor( start.x ) ) , ( int ) ( floor( start.y ) ) );

	if( IsTileSolid( currentTileCoords ) )
	{
		toReturn.didImpact		= true;
		toReturn.startHitPoint	= start;
		toReturn.endHitPoint	= start;
		toReturn.impactNormal	= -direction;
		return toReturn;
	}
	
	Vec2	displacement				= direction * distace;
	float	xDeltaT						= 1 / abs( displacement.x );
	int		tileStepX					= direction.x > 0.f ? 1 : -1;
	int		offsetToLeadingEdgeX		= ( tileStepX + 1 ) / 2;
	float	firstVerticalIntersectionX	= ( float ) ( currentTileCoords.x + offsetToLeadingEdgeX );
	float	tOfNextXCrossing			= abs( firstVerticalIntersectionX - start.x ) * xDeltaT;
	float	yDeltaT						= 1 / abs( displacement.y );
	int		tileStepY					= direction.y > 0.f ? 1 : -1;
	int		offsetToLeadingEdgeY		= ( tileStepY + 1 ) / 2;
	float	firstVerticalIntersectionY	= ( float ) ( currentTileCoords.y + offsetToLeadingEdgeY );
	float	tOfNextYCrossing			= abs( firstVerticalIntersectionY - start.y ) * yDeltaT;
	int		tileX						= currentTileCoords.x;
	int		tileY						= currentTileCoords.y;
	
	while( true )
	{
		if( tOfNextXCrossing < tOfNextYCrossing )
		{
			if( tOfNextXCrossing > 1.f )
			{
				toReturn.didImpact		= false;
				toReturn.startHitPoint	= start;
				toReturn.endHitPoint	= start + direction * distace;
				return toReturn;
			}
			
			tileX += tileStepX;

			if( IsTileSolid( IntVec2( tileX , tileY ) ) )
			{
				toReturn.didImpact		= true;
				toReturn.startHitPoint	= start + ( displacement * tOfNextXCrossing );
				toReturn.endHitPoint	= toReturn.startHitPoint;
				toReturn.impactNormal	= Vec2( ( float ) -tileStepX , 0.f );
				return toReturn;
			}

			tOfNextXCrossing += xDeltaT;
		}
		else
		{
			if( tOfNextYCrossing > 1.f )
			{
				toReturn.didImpact		= false;
				toReturn.startHitPoint	= start;
				toReturn.endHitPoint	= start + direction * distace;
				return toReturn;
			}

			tileY += tileStepY;
			if( IsTileSolid( IntVec2( tileX , tileY ) ) )
			{
				toReturn.didImpact		= true;
				toReturn.startHitPoint	= start + ( displacement * tOfNextYCrossing );
				toReturn.endHitPoint	= toReturn.startHitPoint;
				toReturn.impactNormal	= Vec2( 0.f , ( float ) -tileStepY );
				return toReturn;
			}
			tOfNextYCrossing += yDeltaT;
		}
	}
	return toReturn;
}


//--------------------------------------------------------------------------------------------------------------------------------------------

AABB3 TileMap::GetTileBounds( int tileIndex )
{
	AABB3 box( Vec3( -0.5f , -0.5f , -0.5f ) , Vec3( 0.5f , 0.5f , 0.5f ) );
	Vec2 tileCoords = Vec2( IntVec2( GetTileCoordsForIndex( tileIndex ) ) );
	Vec2 offset( 0.5f , 0.5f );
	Vec3 translateBy = Vec3( tileCoords + offset , 0.5f );
	box.Translate( translateBy );

	return box;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

IntVec2 TileMap::GetTileCoordsForIndex( int index )
{
	//IntVec2 coords;
	//coords.x = RoundDownToInt( ( float ) index / m_dimensions.y );
	//coords.y = index - ( coords.x * m_dimensions.y );
	//return coords;
	return m_tiles[ index ].m_tileCoords;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int TileMap::GetIndexForTileCoords( IntVec2 tileCoords )
{
	int tileIndex = ( m_dimensions.x * tileCoords.y ) + tileCoords.x;
	return tileIndex;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* TileMap::GetTileAtCoords( IntVec2 tileCoords )
{
	int index = GetIndexForTileCoords( tileCoords );
	Tile* tile = nullptr;

	if ( index >= 0 &&  index < m_tiles.size() )
	{
		tile = &m_tiles[ index ];
	}
	
	return tile;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TileMap::IsTileSolid( IntVec2 tileCoords )
{
	Tile* tile = GetTileAtCoords( tileCoords );
	if( tile == nullptr )
	{
		return true;
	}

	return tile->IsSolid();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			UTILITY FUNCTION - MAP CREATION
//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateTopFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
							  const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );
	
	Vertex_PCU faceVerts[ 4 ] = {
		
		// TOP FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 topNormal = CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 topTangent = faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position;
		
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = topNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( topTangent , 1.f );
	}

	uint FaceIndicesValues[ 6 ] = {
								// TOP FACE INDICES
									0,2,1,
									3,2,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateBottomFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
							  const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {

		// BOTTOM FACE VERTS
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 bottomNormal = - CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 bottomTangent = -( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position );
	
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = bottomNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( bottomTangent , 1.f );
	}
	

	uint FaceIndicesValues[ 6 ] = {
								// BOTTOM FACE INDICES
									0,2,1,
									3,2,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateNorth_FrontFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
					   const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {
						// FRONT FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 frontNormal = CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 frontTangent = faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position;

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = frontNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( frontTangent , 1.f );
	}

	uint FaceIndicesValues[ 6 ] = {
								// NORTH FACE INDICES
									0,1,2,
									2,3,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateSouth_BackFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
					   const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {
						// BACK FACE VERTS
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 backNormal = -CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 backTangent = -( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = backNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( backTangent , 1.f );
	}

	uint FaceIndicesValues[ 6 ] = {
								// SOUTH FACE INDICES
									0,1,2,
									2,3,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateEast_RightFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
					   const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {
						// RIGHT FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 rightNormal = -CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 rightTangent = -( faceVerts[ 0 ].m_position - faceVerts[ 3 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = rightNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( rightTangent , 1.f );
	}

	uint FaceIndicesValues[ 6 ] = {
								// EAST FACE INDICES
									0,1,2,
									2,3,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateWest_LeftFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
					   const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {
									// LEFT FACE VERTS
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 leftNormal = CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 leftTangent = ( faceVerts[ 0 ].m_position - faceVerts[ 3 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = leftNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( leftTangent , 1.f );
	}


	uint FaceIndicesValues[ 6 ] = {
								// WEST FACE INDICES
									0,1,2,
									2,3,0,
									};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

Vertex_PCU CubeVerts[ 24 ] = {





};
