#include "Game/Map.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/player.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/NpcTank.hpp"
#include "Game/Bullet.hpp"
#include "Game/Boulder.hpp"
#include "Game/NpcTurret.hpp"
#include "Game/Explosion.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( Game* theGame , IntVec2 size , int MapNumber ) : m_theGame( theGame ) , m_size( size ) , m_thisMapNumber( MapNumber )
{

	m_thisMapNumber++;
	m_exitPosition = Vec2( m_size.x - 2 , m_size.y - 2 ) + Vec2( 0.5f , 0.5f ); // adding in the offset for center point

//--------------------------------------------------------------------------------------------------------------------------------------------
//						INITIALIZING ALL TILES TO GRASSTILES
//--------------------------------------------------------------------------------------------------------------------------------------------

	for ( int verticalIndex = 0; verticalIndex < m_size.y; verticalIndex ++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < m_size.x; horizontalIndex ++ )
		{
			m_tiles.push_back( Tile( IntVec2( horizontalIndex , verticalIndex ) , TILE_TYPE_DEFAULT ) );
		}
	}
		
//--------------------------------------------------------------------------------------------------------------------------------------------
//						INITIALIZING ALL TILES AND ENTITIES AS NECESSARY
//--------------------------------------------------------------------------------------------------------------------------------------------

	RandomizeStoneTiles();
	RandomizeMudTiles();
// 	if ( MapNumber >= 1 && MapNumber != 2)
// 	{
// 		SpawnWorms( MAX_NUMBER_OF_LAVA_WORMS , TILE_TYPE_LAVA );
// 	}
// 	if ( MapNumber >= 2 )
// 	{
// 		SpawnWorms( MAX_NUMBER_OF_WATER_WORMS , TILE_TYPE_WATER );
// 	}
	SafeZones();
	OuterBoundaryWalls();
	SafeZonesBoundary();
	IntVec2 startPosition = IntVec2( 1 , 1 );
	bool mapvalidity = CheckMapValidityUsingFloodFill( startPosition , IntVec2( m_exitPosition ) );
	SetUnreachedTilesToStone();
	if ( !mapvalidity )
	{
		delete this;
		return;
	}
	SafeZones();
	OuterBoundaryWalls();
	SafeZonesBoundary();

	InitializeTileVertices();
	SpawnNewEntity( PLAYERTANK_ENTITY , FACTION_ALLY , Vec2( 1.5f , 1.5f ) , 45.f );
	InitialNPCSpawner();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Update( float deltaSeconds )
{
	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES; Entitytype++ )
	{
		Entitylist& currentList = m_entityListsByType[ Entitytype ];	
		for ( int entityIndex = 0; entityIndex < (int)m_entityListsByType[ Entitytype ].size(); entityIndex++ )
		{
			if ( currentList[ entityIndex ] )
			{
				currentList[ entityIndex ]->Update( deltaSeconds );
			}
			
		}
	}
	CheckNoClipping();
	CheckCollisions();
	GarbageCollection();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Render() const
{
	g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_TILE_TERRAIN_8x8 ] );
	g_theRenderer->DrawVertexArray( m_tileVerts );
	g_theRenderer->BindTexture( nullptr );

	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES; Entitytype++ )
	{
		Entitylist currentList = m_entityListsByType[ Entitytype ];

		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
		{
			if ( currentList[ entityIndex ] )
			{
				currentList[ entityIndex ]->Render();
				//m_entityListsByType[ Entitytype ][ entityIndex ]->Render();
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::GarbageCollection()
{
	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES; Entitytype++ )
	{
		Entitylist& currentList = m_entityListsByType[ Entitytype ];

		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
		{
			if ( /*currentList[ entityIndex ]->GetHealth() <=0 ||*/ currentList[ entityIndex ] != nullptr && currentList[entityIndex]->IsGarbage() )
			{
				currentList[ entityIndex ]->m_isDead =  true ;
				//currentList[ entityIndex ]->m_isGarbage = true;
				delete currentList[ entityIndex ];
				currentList[ entityIndex ] = nullptr;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnNewEntity( EntityType type , Faction faction , const Vec2& position, const float& orientation, const float blastRadius , const float animationDuration )
{
	Entity* newEntity = nullptr;

	switch ( type )
	{
	case INVALID_ENTITY:
		break;
	case PLAYERTANK_ENTITY:
		newEntity = new Player( m_theGame , position , Vec2::ZERO , orientation, type, faction );
		break;
	case NPCTANK_ENTITY:
		newEntity = new NpcTank( m_theGame , position , Vec2::ZERO , orientation , type , faction );
		break;
	case NPCTURRET_ENTITY:
		newEntity = new NpcTurret( m_theGame , position , Vec2::ZERO , orientation , type , faction );
		break;
	case NPCBOULDER_ENTITY:
		newEntity = new Boulder( m_theGame , position , Vec2::ZERO , orientation , type , faction );
		break;
	case GOOD_BULLET_ENTITY :
		newEntity = new Bullet( m_theGame , position , Vec2::ZERO , orientation , type , faction );
		break;
	case EVIL_BULLET_ENTITY:
		newEntity = new Bullet( m_theGame , position , Vec2::ZERO , orientation , type , faction );
		break;
	case EXPLOSION_ENTITY:
		newEntity = new Explosion( m_theGame , position , orientation , blastRadius , animationDuration );
		break;
	case NUM_ENTITY_TYPES:
		break;
	default:
		break;
	}

	AddEntityToMap( newEntity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToMap( Entity* entity )
{
	Entitylist& currentList = m_entityListsByType[ entity->m_entityType ];
	AddEntityToList( currentList, entity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToList( Entitylist& entityList, Entity* entity )
{
	int listIndex = 0;
	for ( listIndex = 0; listIndex < ( int ) entityList.size(); listIndex++ )
	{
		if ( entityList[ listIndex ] == nullptr )
		{
			entityList[ listIndex ] = entity;
			return;
		}
	}
	entityList.push_back( entity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveEntityEntityCollision( Entity& first , Entity& second )
{
		if ( first.m_pushesEntiites && second.m_isPushedByEntities && first.m_isPushedByEntities && second.m_isPushedByEntities)
		{
			PushMobileDiscOutOfEachOther( first.m_position, first.m_physicsRadius , second.m_position, second.m_physicsRadius );
		}
		if ( first.m_pushesEntiites && !second.m_isPushedByEntities && second.m_pushesEntiites )
		{
			PushMobileDiscOutOfFixedDisc( first.m_position , first.m_physicsRadius , second.m_position , second.m_physicsRadius );
		}
		if ( !first.m_pushesEntiites && first.m_isPushedByEntities && !second.m_isPushedByEntities && second.m_pushesEntiites )
		{
			PushMobileDiscOutOfFixedDisc( first.m_position , first.m_physicsRadius , second.m_position , second.m_physicsRadius );
		}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveEntityWorldCollision( Entity* entity )
{
	if ( !entity )
	{
		return;
	}
	Vec2 entityOriginalPosition = entity->m_position;
	if ( entity->m_isPushedByWalls )
	{
		Vec2 entityPosition = entity->m_position;

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
		int		topRighttIndex = GetTileIndexforTileCoords( topRightCoords );
		int		bottomRightIndex = GetTileIndexforTileCoords( bottomRightCoords );

		float entityPhysicsRadius = entity->m_physicsRadius;
			
		if ( IsTileTypeSolid( m_tiles[ leftIndex ].m_type ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , leftAABB );
		}
		if ( IsTileTypeSolid( m_tiles[ rightIndex ].m_type ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , rightAABB );
		}
		if ( IsTileTypeSolid( m_tiles[ upIndex ].m_type ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , upAABB );
		}
		if ( IsTileTypeSolid( m_tiles[ downIndex ].m_type ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , downAABB );
		}
		if ( IsTileTypeSolid( m_tiles[ topRighttIndex ].m_type ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , topRightAABB );
		}
		if ( IsTileTypeSolid( m_tiles[ topLeftIndex ].m_type ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , topLeftAABB );
		}
		if ( IsTileTypeSolid( m_tiles[ bottomRightIndex ].m_type ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , bottomRightAABB );
		}
		if ( IsTileTypeSolid( m_tiles[ bottomLeftIndex ].m_type ) )
		{
			PushDiscOutOfAABB( entityPosition , entityPhysicsRadius , bottomLeftAABB );
		}
		
		
//--------------------------------------------------------------------------------------------------------------------------------------------

		Vec2 entityOriginalVelocity = entity->m_velocity;
				
		int currentTileIndex = GetTileIndexforTileCoords( currentTileCoords );
		AABB2 currentAABB = AABB2( currentTileCoords.x , currentTileCoords.y , currentTileCoords.x + 1 , currentTileCoords.y + 1 );
							   
		if ( m_tiles[ leftIndex ].m_type == TILE_TYPE_MUD && DoDiscAndAABBOverlap( entityPosition , entityPhysicsRadius , leftAABB ) )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else if ( m_tiles[ rightIndex ].m_type == TILE_TYPE_MUD && DoDiscAndAABBOverlap( entityPosition , entityPhysicsRadius , rightAABB ) )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else if ( m_tiles[ upIndex ].m_type == TILE_TYPE_MUD && DoDiscAndAABBOverlap( entityPosition , entityPhysicsRadius , upAABB ) )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else if ( m_tiles[ downIndex ].m_type == TILE_TYPE_MUD && DoDiscAndAABBOverlap( entityPosition , entityPhysicsRadius , downAABB ) )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else if ( m_tiles[ topRighttIndex ].m_type == TILE_TYPE_MUD && DoDiscAndAABBOverlap( entityPosition , entityPhysicsRadius , topRightAABB ) )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else if ( m_tiles[ topLeftIndex ].m_type == TILE_TYPE_MUD && DoDiscAndAABBOverlap( entityPosition , entityPhysicsRadius , topLeftAABB ) )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else if ( m_tiles[ bottomRightIndex ].m_type == TILE_TYPE_MUD && DoDiscAndAABBOverlap( entityPosition , entityPhysicsRadius , bottomRightAABB ) )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else if ( m_tiles[ bottomLeftIndex ].m_type == TILE_TYPE_MUD && DoDiscAndAABBOverlap( entityPosition , entityPhysicsRadius , bottomLeftAABB ) )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else if ( m_tiles[ currentTileIndex ].m_type == TILE_TYPE_MUD )
		{
			entity->m_velocity = entity->m_velocity * 0.5f;
		}
		else
		{
			entity->m_velocity = entityOriginalVelocity;
		}

	entity->m_position = entityPosition;
	}

	if ( !entity->IsGarbage() && ( entity->m_entityType == GOOD_BULLET_ENTITY || entity->m_entityType == EVIL_BULLET_ENTITY ) )
	{
		for ( int tileIndex = 0; tileIndex < ( int ) m_tiles.size(); tileIndex++ )
		{
			if ( IsTileTypeSolid( m_tiles[ tileIndex ].m_type ) )
			{
				AABB2 box = AABB2( ( float ) m_tiles[ tileIndex ].m_tileCoords.x , ( float ) m_tiles[ tileIndex ].m_tileCoords.y ,
									( float ) m_tiles[ tileIndex ].m_tileCoords.x + 1 , ( float ) m_tiles[ tileIndex ].m_tileCoords.y + 1 );
				if ( DoDiscAndAABBOverlap( entity->m_position , entity->m_physicsRadius , box ) )
				{
					entity->m_isGarbage = true;
					SpawnNewEntity( EXPLOSION_ENTITY , FACTION_ALLY , entity->m_position , g_RNG->RollRandomFloatLessThan( 360.f ) , 0.5f , 0.5f );
				}
				
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveEntityBulletCollision( Entity& entity )
{

	if ( entity.m_isHitByBullets && ( entity.m_health > 0 ) )
	{
		entity.TakeDamage();
		if (entity.m_health<=0)
		{
			entity.m_isDead = true;
			entity.m_isGarbage = true;
		}
		if ( entity.m_entityType == PLAYERTANK_ENTITY  && entity.m_health > 0)
		{
			g_theAudioSystem->PlaySound( SOUND_PLAYER_DAMAGED );
		}
		if ( entity.m_entityType == PLAYERTANK_ENTITY && entity.m_health <= 0 )
		{
			g_theAudioSystem->PlaySound( SOUND_PLAYER_DIED );
			SpawnNewEntity( EXPLOSION_ENTITY , FACTION_ALLY , entity.m_position , g_RNG->RollRandomFloatLessThan( 360.f ), 2.f, 3.f );
		}
		if ( (entity.m_entityType == NPCTANK_ENTITY || entity.m_entityType == NPCTURRET_ENTITY )&& entity.m_health > 0 )
		{
			g_theAudioSystem->PlaySound( SOUND_ENEMY_DAMAGED );
		}
		if ( ( entity.m_entityType == NPCTANK_ENTITY || entity.m_entityType == NPCTURRET_ENTITY ) && entity.m_health <= 0 )
		{
			g_theAudioSystem->PlaySound( SOUND_ENEMY_DIED );
			if ( entity.m_entityType == NPCTANK_ENTITY )
			{
				SpawnNewEntity( EXPLOSION_ENTITY , FACTION_ALLY , entity.m_position , g_RNG->RollRandomFloatLessThan( 360.f ) , 1.5f , 2.f );
			}
			if ( entity.m_entityType == NPCTURRET_ENTITY )
			{
				SpawnNewEntity( EXPLOSION_ENTITY , FACTION_ALLY , entity.m_position , g_RNG->RollRandomFloatLessThan( 360.f ) , 1.f , 1.5f );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::CheckCollisions()
{
	//Entitylist& currentList = m_entityListsByType[ PLAYERTANK_ENTITY ];

	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES-1; Entitytype++ )
	{
		Entitylist& currentList = m_entityListsByType[ Entitytype ];
		
		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
		{
			if (currentList[entityIndex]!= nullptr)
			{
				ResolveEntityWorldCollision( currentList[ entityIndex ] );
			}
		}
	}
	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES-1; Entitytype++ )
	{
		for ( int Entitytype2 = 0; Entitytype2 < NUM_ENTITY_TYPES-1; Entitytype2++ )
		{
			for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
			{
				for ( int entityIndex1 = 0; entityIndex1 < ( int ) m_entityListsByType[ Entitytype2 ].size(); entityIndex1++ )
				{
					if ( m_entityListsByType[ Entitytype ][ entityIndex ] != nullptr && m_entityListsByType[ Entitytype2 ][ entityIndex1 ] != nullptr && m_entityListsByType[ Entitytype ][ entityIndex ] != m_entityListsByType[ Entitytype2 ][ entityIndex1 ] )
					{
  						ResolveEntityEntityCollision( *m_entityListsByType[ Entitytype ][ entityIndex ] , *m_entityListsByType[ Entitytype2 ][ entityIndex1 ] );

					}
				}

			}

		}
	}
	for ( int Entitytype = GOOD_BULLET_ENTITY; Entitytype < NUM_ENTITY_TYPES - 1; Entitytype++ )
	{
		for ( int EntitytypeTwo = 0; EntitytypeTwo < NUM_ENTITY_TYPES-1; EntitytypeTwo++ )
		{
			for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
			{
				for ( int entityIndex1 = 0; entityIndex1 < ( int ) m_entityListsByType[ EntitytypeTwo ].size(); entityIndex1++ )
				{
						Entity* firstEntity = m_entityListsByType[ Entitytype ][ entityIndex ];
						Entity* secondEntity = m_entityListsByType[ EntitytypeTwo ][ entityIndex1 ];
					if ( firstEntity != nullptr && secondEntity != nullptr )
					{ 
						if ( firstEntity->m_faction != secondEntity->m_faction && secondEntity->m_entityType != NPCBOULDER_ENTITY )
						{
							if ( DoDiscsOverlap( firstEntity->m_position , firstEntity->m_physicsRadius , secondEntity->m_position , secondEntity->m_physicsRadius ) )
							{
								ResolveEntityBulletCollision( *secondEntity );
								SpawnNewEntity( EXPLOSION_ENTITY , FACTION_ALLY , firstEntity->m_position , g_RNG->RollRandomFloatLessThan( 360.f ) , 0.5f , 0.5f );
								firstEntity->TakeDamage();
								firstEntity->m_isDead = true;
								firstEntity->m_isGarbage = true;
							}
						}
						if (  secondEntity->m_entityType == NPCBOULDER_ENTITY )
						{
							if ( DoDiscsOverlap( firstEntity->m_position , firstEntity->m_physicsRadius , secondEntity->m_position , secondEntity->m_physicsRadius ) )
							{
								firstEntity->m_velocity.Reflect( (secondEntity->m_position- firstEntity->m_position).GetNormalized() );
								firstEntity->m_orientationDegrees = firstEntity->m_velocity.GetAngleDegrees();
								PushMobileDiscOutOfFixedDisc( firstEntity->m_position , firstEntity->m_physicsRadius , secondEntity->m_position , secondEntity->m_physicsRadius );
								firstEntity->m_velocity = Vec2::MakeFromPolarDegrees( firstEntity->m_orientationDegrees , 10.f );
								g_theAudioSystem->PlaySound( m_theGame->m_sounds[ SOUND_BOULDER_MOVED ] ); // Needs TO be moved to more appropriate place
							}
						}
					}
				}
			}

		}

	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

RayCastResult Map::RayCast( Vec2 startPosition , Vec2 forwardDirection , float maxDistance ) const
{
	RayCastResult rayCastResult;
	float correctionScale = 1.f / 100.f;
	float currentDistance = 0.f;
	Vec2  endPosition = startPosition;

	while ( currentDistance <= maxDistance )
	{
		endPosition += forwardDirection * correctionScale;
		currentDistance += correctionScale;
		IntVec2 currentTileCoords = IntVec2( RoundDownToInt( endPosition.x ) , RoundDownToInt( endPosition.y ) );
		int index = ( currentTileCoords.y * m_size.x ) + currentTileCoords.x;

		if ( IsTileTypeSolid(m_tiles[index].m_type) )
		{
			rayCastResult.m_didImpact = true;
			rayCastResult.m_impactDistance = currentDistance;
			rayCastResult.m_impactFraction = currentDistance * ( 1.f / maxDistance );
			rayCastResult.m_impactPosition = endPosition;
			break;
		}
	}

	if ( currentDistance >= maxDistance )
	{
		rayCastResult.m_didImpact = false;
		rayCastResult.m_impactDistance = maxDistance;
		rayCastResult.m_impactPosition = endPosition;
		rayCastResult.m_impactFraction = 1.f;
	}

	return rayCastResult;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Map::InLineOfSight( Vec2& endPosition , Vec2& startPosition , float maxDistance )
{
	Vec2 forwardDirection = endPosition - startPosition;
	float distanceBetweenEndAndStart = forwardDirection.GetLength();
	forwardDirection.Normalize();

	if (distanceBetweenEndAndStart> maxDistance)
	{
		return false;
	}
	RayCastResult raycastResult = RayCast( startPosition, forwardDirection , distanceBetweenEndAndStart );

 	if ( !raycastResult.m_didImpact )
 	{
 		return true;
 	}
 	return false;	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Map::IsTileSolid( const Tile& tile )const
{
	return IsTileTypeSolid( tile.GetTileType() );	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::CheckNoClipping()
{
	if ( m_theGame->m_isClipping )
	{
		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ PLAYERTANK_ENTITY ].size(); entityIndex++ )
		{
			if ( m_entityListsByType[ PLAYERTANK_ENTITY ][ entityIndex ] )
			{
				m_entityListsByType[ PLAYERTANK_ENTITY ][ entityIndex ]->m_isPushedByWalls = false;
			}
		}
	}
	else
	{
		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ PLAYERTANK_ENTITY ].size(); entityIndex++ )
		{
			if ( m_entityListsByType[ PLAYERTANK_ENTITY ][ entityIndex ] )
			{
				m_entityListsByType[ PLAYERTANK_ENTITY ][ entityIndex ]->m_isPushedByWalls = true;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Map::GetTileCenter( IntVec2 tileCoords ) const
{
	return Vec2( tileCoords ) + Vec2( 0.5f , 0.5f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::OuterBoundaryWalls()
{
	for ( int horizontalIndex = 0; horizontalIndex < m_size.x; horizontalIndex++ )
	{
		m_tiles[ horizontalIndex ].SetTileType( TILE_TYPE_EDGETILE );
	}
	for ( int verticalIndex = 1; verticalIndex < m_size.y; verticalIndex++ )
	{
		int index =  verticalIndex * m_size.x ;
		int index2 = index - 1;
		m_tiles[ index2 ].SetTileType( TILE_TYPE_EDGETILE );
		m_tiles[ index ].SetTileType( TILE_TYPE_EDGETILE );
	}
	
	for ( int horizontalIndex = 0; horizontalIndex < m_size.x; horizontalIndex++ )
	{
		int index = ((m_size.y-1)*m_size.x) + horizontalIndex;
		m_tiles[ index ].SetTileType( TILE_TYPE_EDGETILE );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnWorms( int numberOfWormsToSpawn , Tiletype wormType )
{
	for ( int currentWormNum = 0; currentWormNum < numberOfWormsToSpawn * ( m_thisMapNumber + 1 ); currentWormNum++ )
	{

		IntVec2 tileCoords;
		tileCoords.x = g_RNG->RollRandomIntInRange( 1 , m_size.x - 2 );
		tileCoords.y = g_RNG->RollRandomIntInRange( 1 , m_size.y - 2 );
		int wormLength = g_RNG->RollRandomIntInRange( MIN_WORM_LENGTH , MAX_WORM_LENGTH );
		//int wormCurrentLength = 0;
		int tileIndex = ( tileCoords.y * m_size.x ) + tileCoords.x;
		for ( int wormCurrentLength = 0; wormCurrentLength < wormLength; wormCurrentLength++ )
		{

			IntVec2 nextTile = tileCoords;
			int nextTileMovement = g_RNG->RollRandomIntInRange( 0 , 1 );
			int nextTileVariation = g_RNG->RollRandomIntInRange( -1 , 1 );

			if ( nextTileMovement == 0 )
			{ tileCoords.x += nextTileVariation; }
			else
			{ tileCoords.y += nextTileVariation; }

			tileIndex = ( tileCoords.y * m_size.x ) + tileCoords.x;

			int currentTileIndex = GetTileIndexforTileCoords( tileCoords );
			m_tiles[ currentTileIndex ].SetTileType( wormType );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::RandomizeStoneTiles()
{
	SpawnWorms( MAX_NUMBER_OF_STONE_WORMS , TILE_TYPE_STONE );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::RandomizeMudTiles()
{
	SpawnWorms( MAX_NUMBER_OF_MUD_WORMS , TILE_TYPE_MUD );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SafeZones()
{
//--------------------------------------------------------------------------------------------------------------------------------------------
//									SPAWN SAFEZONE
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	for (int verticalIndex = 1; verticalIndex < 6; verticalIndex++)
	{
		for ( int horizontalIndex = 1; horizontalIndex < 6; horizontalIndex++ )
		{
			int index = ( verticalIndex * m_size.x ) + horizontalIndex;
			m_tiles[ index ].SetTileType( TILE_TYPE_STARTAREA_TILE );
		}
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//									FINISH SAFEZONE
//--------------------------------------------------------------------------------------------------------------------------------------------

	for ( int verticalIndex = m_size.y - 1; verticalIndex > m_size.y - 7; verticalIndex-- )
	{
		for ( int horizontalIndex = m_size.x - 1; horizontalIndex > m_size.x - 7; horizontalIndex-- )
		{
			int index = ( verticalIndex * m_size.x ) + horizontalIndex;
			m_tiles[ index ].SetTileType( TILE_TYPE_EXITAREA_TILE );
		}
	}
}

bool Map::CheckMapValidityUsingFloodFill( IntVec2 startTile , IntVec2 endTile )
{
	bool hasAnythingChanged = true;
	//bool hasReached
	int startTileIndex = GetTileIndexforTileCoords( startTile );
	m_tiles[ startTileIndex ].m_hasBeenSeen = true;
	int tilesTraversed = 0;
	while ( hasAnythingChanged )
	{
		hasAnythingChanged = false;
	
		for ( int currentTileIndex = startTileIndex + 1; currentTileIndex < ( int ) m_tiles.size(); currentTileIndex++ )
		{
			IntVec2 currentTile = GetTileCoordsforTileIndex( currentTileIndex );
			IntVec2 northTile = IntVec2( currentTile.x , currentTile.y + 1 );
			IntVec2 southTile = IntVec2( currentTile.x , currentTile.y - 1 );
			IntVec2 eastTile = IntVec2( currentTile.x + 1 , currentTile.y );
			IntVec2 westTile = IntVec2( currentTile.x - 1 , currentTile.y );

			int northTileIndex = GetTileIndexforTileCoords( northTile );
			int southTileIndex = GetTileIndexforTileCoords( southTile );
			int eastTileIndex = GetTileIndexforTileCoords( eastTile );
			int westTileIndex = GetTileIndexforTileCoords( westTile );
			

			if (  !IsTileSolid( m_tiles[ currentTileIndex ] )  &&  
				( m_tiles[ northTileIndex ].m_hasBeenSeen || m_tiles[ southTileIndex ].m_hasBeenSeen || 
				  m_tiles[ eastTileIndex ].m_hasBeenSeen || m_tiles[westTileIndex].m_hasBeenSeen )
				)
			{
				if ( !m_tiles[currentTileIndex].m_hasBeenSeen)
				{
					m_tiles[ currentTileIndex ].m_hasBeenSeen = true;
					tilesTraversed++;
					hasAnythingChanged = true;
				}
			}

			if ( m_tiles[ GetTileIndexforTileCoords( endTile ) ].m_hasBeenSeen )
			{
				return true;
			}
		}
	}
	if (  m_tiles[GetTileIndexforTileCoords(endTile)].m_hasBeenSeen )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SafeZonesBoundary()
{
//--------------------------------------------------------------------------------------------------------------------------------------------
//									 SPAWN SAFE ZONE BOUNDARY
//--------------------------------------------------------------------------------------------------------------------------------------------

	int index = ( int ) ( ( 2 * m_size.x ) + 4 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( 3 * m_size.x ) + 4 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( 4 * m_size.x ) + 2 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( 4 * m_size.x ) + 3 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( 4 * m_size.x ) + 4 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

//--------------------------------------------------------------------------------------------------------------------------------------------
//									FINISH LINE SAFE ZONE BOUNDARY
//--------------------------------------------------------------------------------------------------------------------------------------------

	index = ( int ) ( ( ( m_size.y - 3 ) * m_size.x ) + m_size.x - 5 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( m_size.y - 4 ) * m_size.x ) + m_size.x - 5 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( m_size.y - 5 ) * m_size.x ) + m_size.x - 5 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	//index = ( int ) ( ( ( m_size.y - 6 ) * m_size.x ) + m_size.x - 5 );
	//m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( m_size.y - 5 ) * m_size.x ) + m_size.x - 4 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

	index = ( int ) ( ( ( m_size.y - 5 ) * m_size.x ) + m_size.x - 3 );
	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

//--------------------------------------------------------------------------------------------------------------------------------------------
// FLOODFILL TESTING
//--------------------------------------------------------------------------------------------------------------------------------------------

//   	index = ( int ) ( ( ( m_size.y - 2 ) * m_size.x ) + m_size.x - 5 );
//   	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );
//   
//  	index = ( int ) ( ( ( m_size.y - 6 ) * m_size.x ) + m_size.x - 2 );
//  	m_tiles[ index ].SetTileType( TILE_TYPE_STONE );

//--------------------------------------------------------------------------------------------------------------------------------------------

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::InitializeTileVertices()
{
	IntVec2 exitPosition = IntVec2(m_exitPosition);
	int exitTileIndex = ( exitPosition.y * m_size.x ) + exitPosition.x;
	m_tiles[ exitTileIndex ].m_type = TILE_TYPE_EXIT;
	for ( int verticalIndex = 0; verticalIndex < m_size.y; verticalIndex++ )
	{
		for ( int horizontalIndex = 0; horizontalIndex < m_size.x; horizontalIndex++ )
		{
			int currentTileIndex = ( verticalIndex * m_size.x ) + horizontalIndex;
			AABB2 box = AABB2( Vec2( horizontalIndex , verticalIndex ) , Vec2( horizontalIndex + 1 , verticalIndex + 1 ) );
			Vec2 minUVs;
			Vec2 maxUVs;
			Tiletype tempType = m_tiles[ currentTileIndex ].GetTileType();
			TileDefinition::s_definitions[ tempType ].m_spriteDfinition.GetUVs( minUVs , maxUVs );
			AppendVertsForAABB2( m_tileVerts , box , GetColorForTileType( tempType ) , minUVs, maxUVs );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::InitialNPCSpawner()
{
	int npcTanksToSpawn	   = INTITAL_MAP_MAX_NUM_NPC_TANKS   + ( 2 * m_thisMapNumber );
	int npcTurretsToSpawn  = INTITAL_MAP_MAX_NUM_NPC_TURRETS + ( 2 * m_thisMapNumber );
	int npcBouldersToSpawn = INTITAL_MAP_MAX_NUM_BOULDERS    + ( 2 * m_thisMapNumber );

	for ( int totalNPCEntitiesToSpawn = 16 + ( 6 * m_thisMapNumber ); totalNPCEntitiesToSpawn > 0; )
	{
		IntVec2 tileCoords;
		tileCoords.x = g_RNG->RollRandomIntInRange( 1 , m_size.x - 2 );
		tileCoords.y = g_RNG->RollRandomIntInRange( 1 , m_size.y - 2 );
		Vec2 randomPosition = GetTileCenter( tileCoords );
		int tileIndex = ( tileCoords.y * m_size.x ) + tileCoords.x;

//--------------------------------------------------------------------------------------------------------------------------------------------
//			TO AVOID SPAWNING ENTITES IN SAFE ZONE
//--------------------------------------------------------------------------------------------------------------------------------------------

		if ( ( tileCoords.x <= 5 && tileCoords.y <= 5 ) || ( ( tileCoords.x >= m_size.x - 5 && tileCoords.y >= m_size.y - 5 ) ) )
		{
			continue;
		}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			TO AVOID SPAWNING ENTITES ON SOLID TILES
//--------------------------------------------------------------------------------------------------------------------------------------------

		if ( IsTileSolid( m_tiles[tileIndex]) )
		{
			continue;
		}

		float randomOrientaion = g_RNG->RollRandomFloatInRange( 0.f , 360.f );
		if ( npcTanksToSpawn > 0 )
		{
			SpawnNewEntity( NPCTANK_ENTITY , FACTION_ENEMY , randomPosition , randomOrientaion );
			npcTanksToSpawn--;
			totalNPCEntitiesToSpawn--;
			continue;
		}
		if ( npcTurretsToSpawn > 0 )
		{
			SpawnNewEntity( NPCTURRET_ENTITY , FACTION_ENEMY , randomPosition , randomOrientaion );
			npcTurretsToSpawn--;
			totalNPCEntitiesToSpawn--;
			continue;
		}
		if ( npcBouldersToSpawn > 0 )
		{
			SpawnNewEntity( NPCBOULDER_ENTITY , FACTION_NEUTRAL , randomPosition , 0.f );
			npcBouldersToSpawn--;
			totalNPCEntitiesToSpawn--;
			continue;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Map::GetTileIndexforTileCoords( const IntVec2& tileCoords ) const
{
	if ( tileCoords.x < 1 || tileCoords.y<1 || tileCoords.x >m_size.x - 2 || tileCoords.y > m_size.y - 2 )
	{
		return 0;
	}
	return ( tileCoords.x ) + ( m_size.x * tileCoords.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

IntVec2 Map::GetTileCoordsforTileIndex( int tileIndex ) const
{
	int tileY = tileIndex / m_size.x;
	int tileX = tileIndex - ( tileY * m_size.x );
	return( IntVec2( tileX , tileY ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SetUnreachedTilesToStone()
{
	for ( int tileIndex = 0; tileIndex < ( int ) m_tiles.size(); tileIndex++ )
	{
		if ( !IsTileTypeSolid(m_tiles[tileIndex].GetTileType()) && !m_tiles[tileIndex].m_hasBeenSeen )
		{
			m_tiles[ tileIndex ].SetTileType( TILE_TYPE_STONE );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::~Map()
{
	for ( int  entitylistIndex = 0; entitylistIndex < NUM_ENTITY_TYPES; entitylistIndex++ )
	{
		Entitylist& currentList = m_entityListsByType[ entitylistIndex ];
		for ( int listIndex = 0; listIndex < ( int ) currentList.size(); listIndex++ )
		{
				delete currentList[ listIndex ];
				currentList[ listIndex ] = nullptr;
		}
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------
