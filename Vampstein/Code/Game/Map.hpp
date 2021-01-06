#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Game/Entity.hpp"
#include <string>
#include "EntityDef.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

typedef std::vector< Entity* > Entitylist;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct RayCastResult
{
	bool didImpact = false;
	Vec2 startHitPoint;
	Vec2 endHitPoint;
	Vec2 impactNormal;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Map
{
public:
	Map( char const* mapName );
	Map( char const* mapName , XMLElement* rootElement );
	virtual ~Map();
	static  Map*		CreateNewMapOfType( std::string mapType , char const* mapName , XMLElement* rootElement );
			void		ParseAllMapPlayerStartEntities( tinyxml2::XMLElement* parentElement , std::string mapName );
			void		ParseAllMapEntitiesOfName( tinyxml2::XMLElement* parentElement , std::string elementName , std::string mapName );
			void		ParseAllMapEntities( tinyxml2::XMLElement* parentElement , std::string mapName );
			void		ParseAllPortalEntities( Entity* portalEntity , tinyxml2::XMLElement* parentElement , std::string mapName );

	virtual void		Update( float deltaSeconds );
	virtual void		UpdateMeshes() = 0;
	virtual void		Render() const = 0;
	
	virtual Entity*		SpawnNewEntityOfType( std::string const& entityDefName );
	virtual Entity*		SpawnNewPlayerEntity();
	virtual Entity*		SpawnNewEntityOfType( EntityDef const& entityDef );
			void		AddEntityToMap( Entity* entity );
			void		AddEntityToList( Entity* entity , Entitylist& eList );

	virtual	void		ResolveCollisions();
			void		ResolvePortalVsEntityCollisions();
			void		ResolveEntityVsEntityCollisions();
public:
	std::string			m_name;
	Vec3				m_playerStartPos;
	float				m_playerStartYawDegrees;
	bool				m_parsedSuccessfully;

	Entitylist			m_allEntities;
	Entitylist			m_players;
	Entitylist			m_projectiles;
	Entitylist			m_NPCs;
	Entitylist			m_Portals;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
