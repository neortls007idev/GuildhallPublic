#include "Game/Actor.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Map.hpp"
#include "Game/World.hpp"

#include "Game/Game.hpp"
#include "Game/TileMap.hpp"
#include "Game/Portal.hpp"
#include "Game/Projectile.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( char const* mapName ) :
									m_name( mapName )
{
	m_parsedSuccessfully = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( char const* mapName , XMLElement* element )
{
	m_parsedSuccessfully = false;
	XMLElement* root = element;
	std::string mapType = element->Attribute( "type"  "NULL" );
	CreateNewMapOfType( mapType , mapName , root );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC Map* Map::CreateNewMapOfType( std::string mapType , char const* mapName , XMLElement* rootElement )
{
	if( _stricmp( mapType.c_str() , "TileMap" ) == 0 )
	{
		Map* newMap = new TileMap( mapName , rootElement );
				
		if( newMap->m_parsedSuccessfully == false )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_ERROR ,
										  "ERROR: %s of TYPE: %s FAILED to parse successfully" , mapName , mapType.c_str() );
			newMap = nullptr;
		}

		return newMap;
	}
	
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ParseAllMapPlayerStartEntities( tinyxml2::XMLElement* parentElement , std::string mapName )
{
	bool parseResult = false;
	
	tinyxml2::XMLElement* entityType = parentElement->FirstChildElement( "PlayerStart" );

	if( entityType != nullptr )
	{
		Entity* newPlayerEntity = SpawnNewPlayerEntity();
		
		Vec2 pos = ParseXmlAttribute( *entityType , "pos" , Vec2::ZERO );
		newPlayerEntity->m_pos = Vec3( pos , 0.65f );
		m_playerStartPos = newPlayerEntity->m_pos;
		
		if( m_playerStartPos.GetLengthSquared() < 0.1f )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
										  "WARNING: MAP: %s has Player start position - 0 , 0 , 0 )" , mapName.c_str() );
		}

		float startYaw = ParseXmlAttribute( *entityType , "yaw" , -1.f );
		newPlayerEntity->m_startYawDegrees = startYaw;
		m_playerStartYawDegrees = newPlayerEntity->m_startYawDegrees;
		
		if( startYaw <= -0.98f && startYaw >= -1.01 )
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
										  "WARNING: MAP: %s has Player start yaw = %.2f )" , mapName.c_str() , startYaw );
		}
		else
		{
			g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_SYTEMLOG ,
										  "MAP: %s has Player start yaw = %.2f )" , mapName.c_str() , startYaw );
		}
	}
	parseResult = true;
	m_parsedSuccessfully &= parseResult;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ParseAllMapEntitiesOfName( tinyxml2::XMLElement* parentElement , std::string elementName , std::string mapName )
{
	bool parseResult = false;

	for( tinyxml2::XMLElement* element = parentElement->FirstChildElement( elementName.c_str() ); element != nullptr; element = element->NextSiblingElement( elementName.c_str() ) )
	{
		if( element != nullptr )
		{
			std::string entityType = ParseXmlAttribute( *element , "type" , "invalid" );
			
			Entity* newEntity = SpawnNewEntityOfType( entityType );

			if ( nullptr == newEntity )
			{
				g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
											  "WARNING: MAP: %s has INVALID Entity : %s )" , mapName.c_str() , elementName.c_str() );
				continue;
			}

			Vec2 pos = ParseXmlAttribute( *element , "pos" , Vec2::ZERO );
			newEntity->m_pos = Vec3( pos , 0.0f );

			if( newEntity->m_pos.GetLengthSquared() < 0.1f )
			{
				g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
											  "WARNING: MAP: %s has %s Entity start position - 0 , 0 , 0 )" , mapName.c_str() , elementName.c_str() );
			}

			float startYaw = ParseXmlAttribute( *element , "yaw" , -1.f );
			newEntity->m_startYawDegrees = startYaw;

			if( startYaw <= -0.98f && startYaw >= -1.01 )
			{
				g_theDevConsole->PrintString( eDevConsoleMessageType::DEVCONSOLE_WARNING ,
											  "WARNING: MAP: %s has %s Entity start yaw = %.2f )" , mapName.c_str() , elementName.c_str() , startYaw );
			}

			newEntity->ConstructEntityBillboard();

			if ( elementName == "Portal" )
			{
				ParseAllPortalEntities( newEntity , element , mapName );
			}
		}
	}
	parseResult = true;
	m_parsedSuccessfully &= parseResult;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ParseAllMapEntities( tinyxml2::XMLElement* rootElement , std::string mapName )
{
	XMLElement* entityElement = rootElement->FirstChildElement( "Entities" );
	std::vector<std::string> entityTypes;
	entityTypes.push_back( "PlayerStart" );

	ParseAllMapPlayerStartEntities( entityElement , mapName );
	ParseAllMapEntitiesOfName( entityElement , "Actor" , mapName );
	ParseAllMapEntitiesOfName( entityElement , "Portal" , mapName );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ParseAllPortalEntities( Entity* portalEntity , tinyxml2::XMLElement* parentElement , std::string mapName )
{

	Portal* entityCopy = ( Portal* ) portalEntity;
	
	std::string destMapName = ParseXmlAttribute( *parentElement , "destMap" , "null" );

	if( destMapName != "null" )
	{
		entityCopy->m_destMap	= destMapName;
	}
	
	Vec2 destPos				= ParseXmlAttribute( *parentElement , "destPos" , Vec2( 0.f , 0.f ) );
	entityCopy->m_desPos		= destPos;
	
	float yawOffset				= ParseXmlAttribute( *parentElement , "destYawOffset" , 0.f );
	entityCopy->m_desYawOffset = yawOffset;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Update( float deltaSeconds )
{
	for( size_t index = 0 ; index < m_allEntities.size() ; index++ )
	{
		if( m_allEntities[ index ] != nullptr )
		{
			m_allEntities[ index ]->Update( deltaSeconds );
		}
	}

	ResolveCollisions();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity* Map::SpawnNewEntityOfType( EntityDef const& entityDef )
{
	Entity* newEntity = nullptr;

	if ( entityDef.m_className == "Actor" )
	{
		newEntity = new Actor( entityDef , this );
		newEntity->m_entityType = NPC;
	}
	else if( entityDef.m_className == "Projectile" )
	{
		newEntity = new Projectile( entityDef , this );
		newEntity->m_entityType = PROJECTILE;
	}
	else if( entityDef.m_className == "Portal" )
	{
		newEntity = new Portal( entityDef , this );
		newEntity->m_entityType = PORTAL;
	}
	else if( entityDef.m_className == "Entity" )
	{
		newEntity = new Entity( entityDef , this );
	}

	AddEntityToMap( newEntity );
	return newEntity;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity* Map::SpawnNewEntityOfType( std::string const& entityDefName )
{
	EntityDef const* entityDef = EntityDef::GetDefinition( entityDefName );
	if ( entityDef )
	{
		return SpawnNewEntityOfType( *entityDef );
	}
	else
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Failed to create Entity of type : %s" , entityDefName.c_str() );
		return nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity* Map::SpawnNewPlayerEntity()
{
	Entity* newEntity = new Entity();
	newEntity->m_entityType = PLAYER;

	AddEntityToMap( newEntity );
	return newEntity;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToMap( Entity* entity )
{
	if ( nullptr == entity )
	{
		return;
	}

	AddEntityToList( entity , m_allEntities );

	if ( entity->IsPlayer() )
	{
		AddEntityToList( entity , m_players );
	}
	else if( entity->IsProjectile() )
	{
		AddEntityToList( entity , m_projectiles );
	}
	else if( entity->IsNPC() )
	{
		AddEntityToList( entity , m_NPCs );
	}
	else if( entity->IsPortal() )
	{
		AddEntityToList( entity , m_Portals );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToList( Entity* entity , Entitylist& eList )
{
	EmplaceBackAtEmptySpace( eList , entity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveCollisions()
{
	ResolveEntityVsEntityCollisions();
	ResolvePortalVsEntityCollisions();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolvePortalVsEntityCollisions()
{
	for ( size_t portalEntityIndex = 0; portalEntityIndex < m_Portals.size() ; portalEntityIndex++  )
	{
		for( size_t entityIndex = 0 ; entityIndex < m_allEntities.size() ; entityIndex++ )
		{
			if( nullptr != m_allEntities[ entityIndex ] && nullptr != m_allEntities[ portalEntityIndex ] )
			{
				if( ( m_allEntities[ entityIndex ] != m_Portals[ portalEntityIndex ] ) && !m_allEntities[ entityIndex ]->IsPortal() )
				{
					Entity* entity = m_allEntities[ entityIndex ];
					Portal* portalEntity = ( Portal* ) m_Portals[ portalEntityIndex ];

					Vec2	firstPos = entity->m_pos.GetXYComponents();
					float	firstRadius = entity->m_physicsRadius;

					Vec2	secondPos = portalEntity->m_pos.GetXYComponents();
					float	secondRadius = portalEntity->m_physicsRadius;

					if( DoDiscsOverlap( firstPos , firstRadius , secondPos , secondRadius ) )
					{
						entity->m_pos = Vec3( portalEntity->m_desPos , portalEntity->m_pos.z );
						entity->m_startYawDegrees += portalEntity->m_desYawOffset;

						if( portalEntity->m_destMap != "" )
						{
							Map* destMap = g_theGame->s_world->GetMap( portalEntity->m_destMap );

							if ( entity == g_theGame->m_player )
							{
								g_theGame->s_world->m_currentMap = destMap;
								g_theGame->m_yaw += portalEntity->m_desYawOffset;
							}
							
							if ( destMap != nullptr )
							{
								destMap->AddEntityToMap( entity );
								m_allEntities[ entityIndex ] = nullptr;
							}
						}
						return;
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveEntityVsEntityCollisions()
{
	for ( size_t firstEntityIndex = 0 ; firstEntityIndex < m_allEntities.size() ; firstEntityIndex ++ )
	{
		for ( size_t secondEntityIndex = firstEntityIndex ; secondEntityIndex < m_allEntities.size() ; secondEntityIndex++ )
		{
			if( nullptr != m_allEntities[ firstEntityIndex ] && nullptr != m_allEntities[ secondEntityIndex ] )
			{
				Entity* firstEntity		= m_allEntities[ firstEntityIndex ];
				Entity* secondEntity	= m_allEntities[ secondEntityIndex ];

				Vec2	firstPos		= firstEntity->m_pos.GetXYComponents();
				float	firstRadius		= firstEntity->m_physicsRadius;

				Vec2	secondPos		= secondEntity->m_pos.GetXYComponents();
				float	secondRadius	= secondEntity->m_physicsRadius;
				
				if( DoDiscsOverlap( firstPos , firstRadius , secondPos , secondRadius ) )
				{
					if( firstEntity->m_canPushEntities && secondEntity->m_canBePushedByEntities &&
						firstEntity->m_canBePushedByEntities && secondEntity->m_canPushEntities )
					{
						float firstEntityMass		= firstEntity->m_mass;
						float secondEntityMass		= secondEntity->m_mass;

						float	massRatioFirst		= secondEntityMass / ( firstEntityMass + secondEntityMass );
						float	massRatioSecond		= firstEntityMass / ( firstEntityMass + secondEntityMass );

						Vec2	displacementBetweenCenters = Vec2( firstPos - secondPos );
						float	distanceBetweenCenters = displacementBetweenCenters.GetLength();

						float	overlap		= firstRadius + secondRadius - distanceBetweenCenters;
						Vec2	normal		= ( firstPos - secondPos ).GetNormalized();
						Vec2	firstDisp	= overlap * massRatioFirst * normal;
						Vec2	secondDisp	= overlap * massRatioSecond * -normal;
						
						firstEntity->m_pos += Vec3( firstDisp , 0.f );
						secondEntity->m_pos += Vec3( secondDisp , 0.f );
					}

					if( firstEntity->m_canPushEntities && !secondEntity->m_canBePushedByEntities &&
						firstEntity->m_canBePushedByEntities && !secondEntity->m_canPushEntities )
					{
						Vec2	displacementBetweenCenters = Vec2( firstPos - secondPos );
						float	distanceBetweenCenters = displacementBetweenCenters.GetLength();

						float	overlap = firstRadius + secondRadius - distanceBetweenCenters;
						Vec2	normal = ( firstPos - secondPos ).GetNormalized();
						Vec2	firstDisp = overlap * normal;
						
						firstEntity->m_pos += Vec3( firstDisp , 0.f );
					}

					if( !firstEntity->m_canPushEntities && secondEntity->m_canBePushedByEntities &&
						!firstEntity->m_canBePushedByEntities && secondEntity->m_canPushEntities )
					{
						float firstEntityMass = firstEntity->m_mass;
						float secondEntityMass = secondEntity->m_mass;

						float	massRatioFirst = 1.f / ( firstEntityMass + secondEntityMass );
						
						Vec2	displacementBetweenCenters = Vec2( firstPos - secondPos );
						float	distanceBetweenCenters = displacementBetweenCenters.GetLength();

						float	overlap = firstRadius + secondRadius - distanceBetweenCenters;
						Vec2	normal = ( firstPos - secondPos ).GetNormalized();
						Vec2	firstDisp = overlap * massRatioFirst * normal;
						Vec2	secondDisp = overlap * -normal;

						secondEntity->m_pos += Vec3( secondDisp , 0.f );
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::~Map()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------