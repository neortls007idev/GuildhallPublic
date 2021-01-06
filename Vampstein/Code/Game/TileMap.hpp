#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Primitives/AABB3.hpp"
#include "Game/Map.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class GPUMesh;
class MapRegion;
class MapMaterial;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Tile
{
public:
	friend class TileMap;
	bool				IsSolid() const																				{ return m_isSolid; }
	
public:
	IntVec2				m_tileCoords	= IntVec2::ZERO;
	AABB2				m_bounds		= AABB2::ZERO_TO_ONE;
	bool				m_isSolid		= false;
	const MapRegion*	m_region		= nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class TileMap : public Map
{
public:
	TileMap( char const* mapName , IntVec2 dimensions );
	TileMap( char const* mapName , XMLElement* rootElement );
	void					ParseLegend( XMLElement* legendElement , std::map<char , std::string>& legendMap );
	void					ParseMapRows( XMLElement* rowElement , std::map<char , std::string>& legendMap );
	static TileMap*			CreateTileMapFromXml( char const* mapName , char const* filepath );

	virtual ~TileMap() override;
	//--------------------------------------------------------------------------------------------------------------------------------------------

			void			PopulateTiles();
	virtual void			UpdateMeshes() override;
	virtual void			Render() const override;
	
			void			AddVertsForTile( std::vector< VertexMaster >& destinationVerts ,
							                  std::vector< uint >& destinationIndices , int tileIndex );
	
			void			AddVertsForSolidTile ( std::vector< VertexMaster >& destinationVerts ,
							                       std::vector< uint >& destinationIndices , int tileIndex ,
							                       Vec2 minUVs = Vec2::ZERO , Vec2 maxUvs = Vec2::ONE );
	
			void			AddVertsForNonSolidTile ( std::vector< VertexMaster >& destinationVerts ,
							                          std::vector< uint >& destinationIndices , int tileIndex );
	
			RayCastResult	RayCastToEntities( Vec2 start , Vec2 direction , float dist );
			RayCastResult	RayCastToTiles( Vec2 start , Vec2 direction , float distace );

			void			DebugRenderRaycasts( Vec2 start , Vec2 direction , float distance );
	
	virtual	void			ResolveCollisions() override;
			void			ResolveEntityVsTileCollisions( Entity* entity , IntVec2 currTileCoords );

			//--------------------------------------------------------------------------------------------------------------------------------------------
			//			UTILITY METHODS
			//--------------------------------------------------------------------------------------------------------------------------------------------
			IntVec2			GetTileCoordsForIndex( int index );
			int				GetIndexForTileCoords( IntVec2 tileCoords );
			Tile*			GetTileAtCoords( IntVec2 tileCoords );
			bool			IsTileSolid( IntVec2 tileCoords );
			AABB3			GetTileBounds( int tileIndex );
	
private:
			MapMaterial*	GetMaterialForLegend( const char glyph );
			
private:
	IntVec2									m_dimensions									= IntVec2::ZERO;
	std::vector< Tile >						m_tiles;
	GPUMesh*								m_worldMesh;
	GPUMesh*								m_entitesDebugMesh;
	GPUMesh*								m_raytraceDebugMesh;
	//std::map<char , std::string>			m_legendMap;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------