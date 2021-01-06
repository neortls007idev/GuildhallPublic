#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Primitives/AABB2.hpp"

class Texture;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eBillBoardType
{
	CAMERA_FACING_XY,
	CAMERA_OPPOSED_XY,
	CAMERA_OPPOSED_XYZ,					// Screen Aligned.
	CAMERA_FACING_XYZ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct BillBoard
{
	BillBoard( AABB2 bounds , Mat44 initialTransform  , Vec3 pos , Vec2 minUvs , Vec2 maxUvs , Texture* tex , eBillBoardType type = CAMERA_FACING_XY );
	void Update();
	void Render();

	void UpdateCameraFacingXY();
	void UpdateCameraOpposedXY();
	void UpdateCameraOpposedXYZ();
	void UpdateCameraFacingXYZ();
	
public:	   
	eBillBoardType				m_type;
	AABB2						m_bounds;
	Mat44						m_transform;
	Vec3						m_pos;
	Texture*					m_spriteTex;
	Vec2						m_minUVs;
	Vec2						m_maxUVs;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

