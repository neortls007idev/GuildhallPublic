#include "Game/Map.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/MapDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( Game* game , MapDefinition* mapDefinition , std::string mapName ) :
															m_theGame( game ),
															m_name( mapName ) , 
															m_mapDefinition( mapDefinition )
{
	//m_name = m_mapDefinition->m_name;
	m_dimensions.x = m_mapDefinition->m_width;
	m_dimensions.y = m_mapDefinition->m_height;

	for ( int verticalIndex = 0; verticalIndex < m_dimensions.y; verticalIndex++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < m_dimensions.x; horizontalIndex++ )
		{
			m_tiles.push_back( Tile( IntVec2( horizontalIndex , verticalIndex ) , m_mapDefinition->m_fillTile ) );
		}
	}

	RandomNumberGenerator rng;

	for ( int mapGenSteps = 0; mapGenSteps < m_mapDefinition->m_generationSteps.size(); mapGenSteps++ )
	{
		FloatRange* chanceToRun = &m_mapDefinition->m_generationSteps[ mapGenSteps ]->m_chanceToRun;
		float chanceToRunStep = chanceToRun->GetRandomInRange( rng ); 
		
		if ( rng.RollPercentChance( chanceToRunStep ) )
		{
			m_mapDefinition->m_generationSteps[ mapGenSteps ]->RunStep( *this );
		}
	}

	//SpawnWorms( 30 , TileDefinition::s_definitions[ "Stone" ] );
	OuterBoundaryWalls();
	InitializeTileVertices();

	m_player = new Actor( m_theGame , Vec2::ONE , 0.f , ActorDefinition::s_definitions[ "Player" ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Update( float deltaSeconds )
{
	m_player->Update( deltaSeconds );
	CheckCollisions();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Render() const
{
	g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_TERRAIN_32x32 ] );
	g_theRenderer->DrawVertexArray( m_tileVerts );
	g_theRenderer->BindTexture( nullptr );

	m_player->Render();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::GarbageCollection()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::~Map()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::OuterBoundaryWalls()
{
	for ( int horizontalIndex = 0; horizontalIndex < m_dimensions.x; horizontalIndex++ )
	{
		m_tiles[ horizontalIndex ].SetTileType( m_mapDefinition->m_edgeTile );
	}
	for ( int verticalIndex = 1; verticalIndex < m_dimensions.y; verticalIndex++ )
	{
		int index = verticalIndex * m_dimensions.x;
		int index2 = index - 1;
		m_tiles[ index2 ].SetTileType( m_mapDefinition->m_edgeTile );
		m_tiles[ index ].SetTileType( m_mapDefinition->m_edgeTile );
	}

	for ( int horizontalIndex = 0; horizontalIndex < m_dimensions.x; horizontalIndex++ )
	{
		int index = ( ( m_dimensions.y - 1 ) * m_dimensions.x ) + horizontalIndex;
		m_tiles[ index ].SetTileType( m_mapDefinition->m_edgeTile );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::InitializeTileVertices()
{
	for ( int verticalIndex = 0; verticalIndex < m_dimensions.y; verticalIndex++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < m_dimensions.x; horizontalIndex++ )
		{
			int currentTileIndex = ( verticalIndex * m_dimensions.x ) + horizontalIndex;
 			AABB2 box = AABB2( Vec2( horizontalIndex , verticalIndex ) , Vec2( horizontalIndex + 1 , verticalIndex + 1 ) );
			TileDefinition* currentTileDef = m_tiles[ currentTileIndex ].GetTileType();
			AppendVertsForAABB2( m_tileVerts , box , WHITE , currentTileDef->m_spriteUVs.m_mins , currentTileDef->m_spriteUVs.m_maxs );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnWorms( int numberOfWormsToSpawn , TileDefinition* wormType )
{
	for ( int currentWormNum = 0; currentWormNum < numberOfWormsToSpawn; currentWormNum++ )
	{

		IntVec2 tileCoords;
		tileCoords.x = g_RNG->RollRandomIntInRange( 1 , m_dimensions.x - 2 );
		tileCoords.y = g_RNG->RollRandomIntInRange( 1 , m_dimensions.y - 2 );
		int wormLength = g_RNG->RollRandomIntInRange( MIN_WORM_LENGTH , MAX_WORM_LENGTH );
		//int wormCurrentLength = 0;
		int tileIndex = ( tileCoords.y * m_dimensions.x ) + tileCoords.x;
		for ( int wormCurrentLength = 0; wormCurrentLength < wormLength; wormCurrentLength++ )
		{

			IntVec2 nextTile = tileCoords;
			int nextTileMovement = g_RNG->RollRandomIntInRange( 0 , 1 );
			int nextTileVariation = g_RNG->RollRandomIntInRange( -1 , 1 );

			if ( nextTileMovement == 0 )
			{
				tileCoords.x += nextTileVariation;
			}
			else
			{
				tileCoords.y += nextTileVariation;
			}

			tileIndex = ( tileCoords.y * m_dimensions.x ) + tileCoords.x;

			int currentTileIndex = GetTileIndexforTileCoords( tileCoords );
			m_tiles[ currentTileIndex ].SetTileType( wormType );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::CheckCollisions()
{
	ResolveEntityWorldCollision( m_player );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveEntityWorldCollision( Entity* entity )
{
	if ( !entity )
	{
		return;
	}
	Vec2 entityOriginalPosition = entity->m_position;
	Vec2& entityPosition = entity->m_position;

	IntVec2 currentTileCoords = IntVec2( RoundDownToInt( entityPosition.x ) , RoundDownToInt( entityPosition.y ) );
	IntVec2 leftCoords = IntVec2( currentTileCoords.x - 1 , currentTileCoords.y );
	IntVec2 rightCoords = IntVec2( currentTileCoords.x + 1 , currentTileCoords.y );
	IntVec2 upCoords = IntVec2( currentTileCoords.x , currentTileCoords.y + 1 );
	IntVec2 downCoords = IntVec2( currentTileCoords.x , currentTileCoords.y - 1 );
	IntVec2 topLeftCoords = IntVec2( currentTileCoords.x - 1 , currentTileCoords.y + 1 );
	IntVec2 bottomLeftCoords = IntVec2( currentTileCoords.x - 1 , currentTileCoords.y - 1 );
	IntVec2 topRightCoords = IntVec2( currentTileCoords.x + 1 , currentTileCoords.y + 1 );
	IntVec2 bottomRightCoords = IntVec2( currentTileCoords.x + 1 , currentTileCoords.y - 1 );
	AABB2	leftAABB = AABB2( leftCoords.x , leftCoords.y , leftCoords.x + 1 , leftCoords.y + 1 );
	AABB2	rightAABB = AABB2( rightCoords.x , rightCoords.y , rightCoords.x + 1 , rightCoords.y + 1 );
	AABB2	upAABB = AABB2( upCoords.x , upCoords.y , upCoords.x + 1 , upCoords.y + 1 );
	AABB2	downAABB = AABB2( downCoords.x , downCoords.y , downCoords.x + 1 , downCoords.y + 1 );
	AABB2	topLeftAABB = AABB2( topLeftCoords.x , topLeftCoords.y , topLeftCoords.x + 1 , topLeftCoords.y + 1 );
	AABB2	bottomRightAABB = AABB2( bottomRightCoords.x , bottomRightCoords.y , bottomRightCoords.x + 1 , bottomRightCoords.y + 1 );
	AABB2	topRightAABB = AABB2( topRightCoords.x , topRightCoords.y , topRightCoords.x + 1 , topRightCoords.y + 1 );
	AABB2	bottomLeftAABB = AABB2( bottomLeftCoords.x , bottomLeftCoords.y , bottomLeftCoords.x + 1 , bottomLeftCoords.y + 1 );
	int		leftIndex = GetTileIndexforTileCoords( leftCoords );
	int		rightIndex = GetTileIndexforTileCoords( rightCoords );
	int		upIndex = GetTileIndexforTileCoords( upCoords );
	int		downIndex = GetTileIndexforTileCoords( downCoords );
	int		topLeftIndex = GetTileIndexforTileCoords( topLeftCoords );
	int		bottomLeftIndex = GetTileIndexforTileCoords( bottomLeftCoords );
	int		topRightIndex = GetTileIndexforTileCoords( topRightCoords );
	int		bottomRightIndex = GetTileIndexforTileCoords( bottomRightCoords );

		float entityPhysicsRadius = entity->m_physicsRadius;

		if  ( !CanEntityMoveInTile(entity, &m_tiles[leftIndex]) )	
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , leftAABB );
		}
		if  ( !CanEntityMoveInTile( entity , &m_tiles[ rightIndex ] ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , rightAABB );
		}
		if  ( !CanEntityMoveInTile( entity , &m_tiles[ upIndex ] ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , upAABB );
		}
		if  ( !CanEntityMoveInTile( entity , &m_tiles[ downIndex ] ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , downAABB );
		}
		
		if  ( !CanEntityMoveInTile( entity , &m_tiles[ topRightIndex ] ) )			
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , topRightAABB );
		}
		if  ( !CanEntityMoveInTile( entity , &m_tiles[ topLeftIndex ] ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , topLeftAABB );
		}
		if ( !CanEntityMoveInTile( entity , &m_tiles[ bottomRightIndex ] ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , bottomRightAABB );
		}
		if ( !CanEntityMoveInTile( entity , &m_tiles[ bottomLeftIndex ] ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , bottomLeftAABB );
		}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Map::GetTileIndexforTileCoords( const IntVec2& tileCoords ) const
{
	if ( tileCoords.x < 0 || tileCoords.y < 0 || tileCoords.x > m_dimensions.x - 2 || tileCoords.y > m_dimensions.y - 2 )
	{
		return 0;
	}
	return ( tileCoords.x ) + ( m_dimensions.x * tileCoords.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

IntVec2 Map::GetTileCoordsforTileIndex( int tileIndex ) const
{
	int tileY = tileIndex / m_dimensions.x;
	int tileX = tileIndex - ( tileY * m_dimensions.x );
	return( IntVec2( tileX , tileY ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetTileAtTileCoords( const IntVec2& tileCoords ) 
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetNorthTileForTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	tileIndex += m_dimensions.x ;
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetSouthTileForTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	tileIndex -= m_dimensions.x;
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetEastTileForTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	tileIndex += 1;
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetWestTileForTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	tileIndex -= 1;
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetNorthEastTileForTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	tileIndex += ( m_dimensions.x ) + 1 ;
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetNorthWestTileForTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	tileIndex += ( m_dimensions.x ) - 1 ;
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetSouthEastTileForTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	tileIndex -= ( ( m_dimensions.x ) + 1 );
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* Map::GetSouthWestTileForTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexforTileCoords( tileCoords );
	tileIndex -= ( ( m_dimensions.x ) - 1 );
	return &m_tiles[ tileIndex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Map::CanEntityMoveInTile( const Entity* entity , const Tile* tile ) const
{
	bool entityCanMove = false;

	if ( entity->m_canWalk && tile->GetTileType()->m_allowsWalk )
	{
		entityCanMove = true;
	}
	
	if ( entity->m_canSwim && tile->GetTileType()->m_allowsSwim )
	{
		entityCanMove = true;
	}

	if ( entity->m_canFly && tile->GetTileType()->m_allowsFly )
	{
		entityCanMove = true;
	}

	return entityCanMove;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string Map::GetTileInfoForTileCoords( IntVec2 tileCoords )
{
	std::string tileInfo = "Tile Type : ";
	if ( tileCoords.x < 0 || tileCoords.y < 0 || tileCoords.x >( m_dimensions.x - 1 ) || tileCoords.y >( m_dimensions.y - 1 ) )
	{
		return "Not Pointing Inside Map";
	}
	else
	{
		int tileIndex = GetTileIndexforTileCoords( tileCoords );
		tileInfo += m_tiles[ tileIndex ].GetTileType()->m_name;
		return tileInfo;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string Map::GetMapInfo()
{
	return "Map Name : " + m_name;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Map::IsMousePointingActor( Vec2 mousePosition , Actor* actor )
{

	if ( ( RoundDownToInt( mousePosition.x ) == RoundDownToInt( m_player->m_position.x ) ) && ( RoundDownToInt( mousePosition.y ) == RoundDownToInt( m_player->m_position.y ) ) )
	{
		// TODO:- ADD in a for loop to check for all actors;
		actor = m_player;
		return true;
	}

	if ( actor == nullptr )
	{
		return false;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------