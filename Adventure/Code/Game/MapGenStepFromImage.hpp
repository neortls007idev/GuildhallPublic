#pragma once
#include "Game/MapGenStep.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Image;

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapGenStepFromImage : public MapGenStep
{
public:
	MapGenStepFromImage( const XMLElement& element );
	~MapGenStepFromImage() {};

	virtual void RunStepOnce( Map& map ) override;
	TileDefinition* DoesRGBA8AtpixelCoordsMatchAnyTileDefinition( IntVec2 pixelCoords );

private:
	Image*   m_mutationImage = nullptr;
	int		 m_rotations     = 0;
	Vec2     m_alignment	 = ALIGN_BOTTOM_LEFT;
};
//--------------------------------------------------------------------------------------------------------------------------------------------