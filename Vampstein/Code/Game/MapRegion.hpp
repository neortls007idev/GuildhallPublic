#pragma once
#include "Engine/Core/NamedStrings.hpp"
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include <string>
#include <map>

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapMaterial;

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapRegion
{
public:
	static bool							LoadDefinitions( char const* regionDataFilePath );
	static MapRegion const*				GetDefinition( std::string const& name );

	bool								IsSolid() const											{ return m_isSolid; }
	MapMaterial*						GetSideMaterial() const									{ return m_sideMaterial; }
	MapMaterial*						GetFloorMaterial() const								{ return m_floorMaterial; }
	MapMaterial*						GetCelingMaterial() const								{ return m_ceilingMaterial; }

private:
	MapRegion( XMLElement* materialDefinition );

private:
	std::string										m_name;
	bool											m_isSolid							= true;
	MapMaterial*									m_sideMaterial						= nullptr;
	MapMaterial*									m_floorMaterial						= nullptr;
	MapMaterial*									m_ceilingMaterial					= nullptr;

public:
	static std::map<std::string , MapRegion*>		s_mapRegions;
};

//--------------------------------------------------------------------------------------------------------------------------------------------