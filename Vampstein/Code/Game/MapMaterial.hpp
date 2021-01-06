#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include <map>
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

class SpriteSheet;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eMapMaterialSheetTypes
{
	MM_DIFFUSE,

	NUM_TOTAL_MM_SHEET_TYPES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class MaterialSheet
{
private:
	MaterialSheet( XMLElement* element );
	static MaterialSheet* GetMaterialSheet( std::string sheetName );
	friend class MapMaterial;

public:
	SpriteSheet* GetSpriteSheet() const																{ return m_diffuseSpriteSheet; }

private:
	std::string										m_name;
	IntVec2											m_layout;
	SpriteSheet*									m_diffuseSpriteSheet			= nullptr;

	static std::map<std::string , MaterialSheet*>	s_materialSheets;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapMaterial
{
private:
	MapMaterial( XMLElement* element );
	
public:
	static bool			LoadDefinitions( const char* filePath );
	static MapMaterial* GetDefinition( std::string materialName );
	SpriteSheet*		GetDiffuseSpriteSheet();
	void				GetUVCoords( Vec2& outMins , Vec2& outMax );

private:
	IntVec2												m_spriteCoords;
	std::string											m_name;
	std::string											m_sheetName;

	static std::map<std::string , MapMaterial*>			s_mapMaterials;
};

//--------------------------------------------------------------------------------------------------------------------------------------------