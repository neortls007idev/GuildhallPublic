#pragma once
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Primitives/Capsule2D.hpp"
#include "Engine/Primitives/Polygon2D.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameCommon.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------
class GameObject;
class Polygon2D;
//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:

	Game();
	~Game(){};
	void InitialGameObjectsSpawner();

	void Update( float deltaSeconds );
	void EndFrame();
	void Render() const;
	void RenderUI() const;
	void RenderGravityUI() const;
	void PhysicsGravityUI() const;
	void RenderDrawMode() const;
	void RenderDrawFromPointCloudMode() const;
	void DebugRender() const;

	void DrawConvexgonMode();
	void PolygonDrawMode();
	void PolygonDrawPointCloudMode();
	bool IsPolygonPotentiallyConvex( Vec2 latestPoint );

	void UpdateCamera();
	void UpdateWorldBounds();
	void UpdateGameObject( float deltaSeconds );
	void UpdateGameObjects();

	void IsMouseInsideGameObject();
	void ResetCollisions();
	void AreObjectsColliding();
	void ChangeColorOnCollision();
	void ChangeAlphaByBounciness();

	void UpdateSimulationType( eSimulationMode* simMode );
	void UpdateGravity();
	void UpdateFriction();
	void UpdateMass();
	void UpdateDrag( float deltaSeconds );
	Vec2 GetMouseDragVelocity() const;
	void ResetMouseDragVelocity();

	void RandomizePointCloud( RandomNumberGenerator rng );

	void DrawMouseCurrentPosition( const Camera& camera ) const;
	GameObject* PickGameobject( Vec2 mousePos );
	void DrawGameObjectToolTip() const;
//--------------------------------------------------------------------------------------------------------------------------------------------

private:
	void DestroyGameObjectsOutOfWorldBounds();

	void UpdateFromUserInput( float deltaSeconds );
	void SelectGameObjectFormUserInput();
	void UpdateCameraFromUserInput( float deltaSeconds );
	void UpdateSelectedGameObjectBouncinessFromUserInput( float deltaSeconds );
	void UpdateSelectedGameObjectAngularMotionFromUserInput( float deltaSeconds );
	
//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	int m_controllerID = -1;
	float m_screenShakeIntensity = 0.f;
	float m_tilesInViewHorizontally = 16.0f;
	float m_tilesInViewVertically = 9.0f;
	Vec2 m_CameraCenter = Vec2( m_tilesInViewHorizontally / 2 , m_tilesInViewVertically / 2 );
	bool m_hasCursorChangedToOBB = false;
	eSimulationMode m_simMode = SIMULATIONMODE_STATIC;
//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	static RandomNumberGenerator	m_rng;
	mutable  Camera					m_worldCamera;
	mutable  Camera					m_UICamera;

		   Rgba8					m_fillColor					= Rgba8( 255 , 255 , 255 , 0 );
		   Rgba8					m_overlapColor				= Rgba8( 255 , 0 , 0 , 255 );
		   Rgba8					m_selectedObjectColor		= Rgba8( 0 , 110 , 0 , 255 );

		   Vec2						m_mousePosition				= Vec2::ZERO;
		   Vec2						m_rigidBodyMouseOffset		= Vec2::ZERO;
		   GameObject*				m_selectedGameObject		= nullptr;
		   GameObject*				m_tooltipObject				= nullptr;
		   bool						m_isDragOffsetSet			= false;
		   bool						m_isGameObjectSelected		= false;

		   Vec3						m_cameraDefaultPosition		= Vec3::ZERO;
		   Vec3						m_cameraCurrentPosition		= Vec3::ZERO;
		   Vec3						m_cameraMoveVelocity		= Vec3( MAX_CAMERA_MOVE_VELOCITY_X , MAX_CAMERA_MOVE_VELOCITY_Y , 0.f );
		   Vec2						m_currentCameraOutputSize	= Vec2( 1600.f , 800.f );

		   AABB2					m_worldBounds				= AABB2::ZERO_TO_ONE;
		   AABB2					m_worldBoundsInterior		= AABB2::ZERO_TO_ONE;
		   Vec2						m_worldBoundsOffset			= Vec2( 47.5f , 47.5f );
		   float					m_worldBoundsThickness		= 5.f;
		   GameObject*				m_floorObject				= nullptr;

		   float					m_minColliderRadius			= MIN_COLLIDER_RADIUS;
		   float					m_maxColliderRadius			= MAX_COLLIDER_RADIUS;
		   float					m_currentColliderRadius;
		   Polygon2D				m_testPolygon;
		   Polygon2D				m_testPolygon1;
		   
		   std::vector<GameObject*>	m_gameObjects;
		   std::vector<bool>		m_isMouseOnGameObject;
		   std::vector<Vec2>		m_pointCloud;

		   std::vector<Vec2>		m_drawModePoints;
		   bool						m_isDrawModeActive = false;
		   Vec2						m_invalidPoint;
		   bool						m_isLastPointInvalid = false;

		   Vec2						m_MouseDragFrames[ 10 ];
		   float					m_dragTime = 0.f;
		   int						m_frameCount = 0;
		   int						m_frameDelay = 0;
private:
	float							m_gameCameraCurrentHeight	= 1600.f;
};

//--------------------------------------------------------------------------------------------------------------------------------------------