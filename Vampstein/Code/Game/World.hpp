#pragma once
#include <map>
#include <string>
#include <vector>

#include "Engine/Core/NamedStrings.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Map;

//--------------------------------------------------------------------------------------------------------------------------------------------

class World
{
public:
	World( Game* owner , char const* name , char const* mapsFolderPath );
	~World();

	void	CreateMaps( std::string mapsFolderPath );
	void	Update();
	void	Render() const;
	
	Map*	GetMapByName( std::string mapName ) const;
	Map*	GetFirstMap() const;
	size_t	GetNumMaps() const															{ return m_maps.size(); }
	Strings	GetAllMapNames() const;
	
	Map*	GetMap( std::string mapName );
	
public:
	Game*								m_owner							= nullptr;
	Map* 								m_currentMap					= nullptr;
	
private:
	std::string							m_name;
	std::map< std::string ,  Map* >		m_maps;
};

//--------------------------------------------------------------------------------------------------------------------------------------------