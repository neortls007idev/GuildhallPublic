#include "Game/GameCommon.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC RandomNumberGenerator Game::m_rng;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 1600.f , 800.f ) );
	
	Vec2 cameraBottomLeft = m_worldCamera.GetOrthoMin().GetXYComponents();
	Vec2 cameraTopRight = m_worldCamera.GetOrthoMax().GetXYComponents();
	
	m_rng.manuallyIncrementPosition();
	RandomizeAABB2(		  cameraBottomLeft , cameraTopRight , m_rng );
	m_rng.manuallyIncrementPosition();
	RandomizeOBB2(		  cameraBottomLeft , cameraTopRight , m_rng );
	m_rng.manuallyIncrementPosition();
	RandomizeLineSegment( cameraBottomLeft , cameraTopRight , m_rng );
	m_rng.manuallyIncrementPosition();
	RandomizeCapsule(	  cameraBottomLeft , cameraTopRight , m_rng );
	m_rng.manuallyIncrementPosition();
	RandomizeDisc(		  cameraBottomLeft , cameraTopRight , m_rng );
	m_rng.manuallyIncrementPosition();
	RandomizePolygon( cameraBottomLeft , cameraTopRight , m_rng.RollRandomIntInRange( 3 , 13 ) , m_rng );

	m_color.RollRandomColor( m_rng );
	m_color.a = 100;

	m_mousePosition = g_theInput->GetMouseNormalizedClientPosition() * ( cameraTopRight - cameraBottomLeft );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RandomizeAABB2( Vec2 mins, Vec2 maxs , RandomNumberGenerator rng )
{
	m_aabb2D = AABB2( rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) ,
		rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RandomizeOBB2( Vec2 mins , Vec2 maxs , RandomNumberGenerator rng )
{
	m_Obb2D = OBB2( Vec2( rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) ) ,
		Vec2( rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) ) ,
		rng.RollRandomFloatInRange( 0.f , 360.f ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RandomizeLineSegment( Vec2 mins , Vec2 maxs , RandomNumberGenerator rng )
{
	m_lineSegment2D = LineSegment2D( rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) ,
		rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RandomizeCapsule( Vec2 mins , Vec2 maxs, RandomNumberGenerator rng )
{
	m_capsule2D = Capsule2D( rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) ,
		rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) ,
		rng.RollRandomFloatInRange( 20.f , 50.f ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RandomizeDisc( Vec2 cameraBottomLeft , Vec2 cameraTopRight , RandomNumberGenerator rng )
{
	m_disc2D = Disc2D( Vec2( rng.RollRandomFloatInRange( cameraBottomLeft.x , cameraTopRight.x ) , rng.RollRandomFloatInRange( cameraBottomLeft.y , cameraTopRight.y ) ),
		rng.RollRandomFloatInRange( 20.f , 50.f ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RandomizePolygon( Vec2 mins , Vec2 maxs , unsigned int count , RandomNumberGenerator rng )
{
	m_polygon.clear();

// 	for ( unsigned int pointIndex = 0; pointIndex < 4/*count*/ ; pointIndex++ )
// 	{
// 		//rng.manuallyIncrementPosition();
// 		Vec2 tempPoint( rng.RollRandomFloatInRange( mins.x , maxs.x ) , rng.RollRandomFloatInRange( mins.y , maxs.y ) );
// 		m_polygon.push_back( tempPoint );
// 	}

	m_polygon.push_back( Vec2( 10 , 10 ) );
	m_polygon.push_back( Vec2( 10 , 100 ) );
	m_polygon.push_back( Vec2( 50 , 150 ) );

	m_polygon.push_back( Vec2( 10 , 10 ) );
	m_polygon.push_back( Vec2( 50 , 150 ) );
	m_polygon.push_back( Vec2( 100 , 100 ) );

	m_polygon.push_back( Vec2( 10 , 10 ) );
	m_polygon.push_back( Vec2( 100 , 100 ) );
	m_polygon.push_back( Vec2( 100 , 10 ) );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RandomizeShapes( Vec2 mins , Vec2 maxs , RandomNumberGenerator* rng )
{
	rng->manuallyIncrementPosition();
	RandomizeAABB2(		  mins , maxs , *rng );
	rng->manuallyIncrementPosition();
	RandomizeOBB2(		  mins , maxs , *rng );
	rng->manuallyIncrementPosition();
	RandomizeLineSegment( mins , maxs , *rng );
	rng->manuallyIncrementPosition();
	RandomizeCapsule(	  mins , maxs , *rng );
	rng->manuallyIncrementPosition();
	RandomizeDisc(		  mins , maxs , *rng );
	rng->manuallyIncrementPosition();
	RandomizePolygon( mins , maxs , rng->RollRandomIntInRange( 3 , 15 ) , *rng );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	Vec2 cameraBottomLeft = m_worldCamera.GetOrthoMin().GetXYComponents();
	Vec2 cameraTopRight	  = m_worldCamera.GetOrthoMax().GetXYComponents();
	UpdateFromKeyBoard();
	m_mousePosition = g_theInput->GetMouseNormalizedClientPosition() * ( cameraTopRight - cameraBottomLeft );

	UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BindTexture( nullptr );

	Vec2 nearestPointOnAABB2D		  = GetNearestPointOnAABB2D( m_mousePosition , m_aabb2D );
	Vec2 nearestPointOnOBB2D		  = GetNearestPointOnOBB2D( m_mousePosition , m_Obb2D );
	Vec2 nearestPointOnCapsule2D	  = GetNearestPointOnCapsule2D( m_mousePosition , m_capsule2D.m_capsuleMidStart , m_capsule2D.m_capsuleMidEnd , m_capsule2D.m_radius );
	Vec2 nearestPointOnDisc2D		  = GetNearestPointOnDisc2D( m_mousePosition , m_disc2D.m_center, m_disc2D.m_radius );
	Vec2 nearestPointOnLineSegment2D  = GetNearestPointOnLineSegment2D( m_mousePosition  , m_lineSegment2D.m_start, m_lineSegment2D.m_end );

	Rgba8 nearestPointColor				= WHITE;
	Rgba8 pointIsInsideObjectColor		= RED;
	Rgba8 objectColorWhenPointIsInside  = m_color;
	objectColorWhenPointIsInside.a		= 225;
	
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnAABB2D			, WHITE , 2.f );
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnOBB2D			, WHITE , 2.f );
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnCapsule2D	    , WHITE , 2.f );
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnLineSegment2D  , WHITE , 2.f );
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnDisc2D         , WHITE , 2.f );
	
	OBB2 mouseCursor = OBB2( m_mousePosition , Vec2( 30 , 50 ) , Vec2( .6f , .8f ) );
	
	if ( m_hasCursorChangedToOBB )
	{
		g_theRenderer->DrawOBB2( mouseCursor , Rgba8( 255 , 0 , 0 , 100 ) );
	}

	if ( IsPointInsideAABB2D( m_mousePosition , m_aabb2D ) )
	{
		g_theRenderer->DrawAABB2( m_aabb2D , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnAABB2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawAABB2( m_aabb2D , m_color );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnAABB2D , 2.f ) , nearestPointColor );
	}

	if ( IsPointInsideOBB2D( m_mousePosition , m_Obb2D ) )
	{
		
		g_theRenderer->DrawOBB2( m_Obb2D , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnOBB2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawOBB2( m_Obb2D , m_color );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnOBB2D , 2.f ) , nearestPointColor );
	}


	if ( IsPointInsideCapsule2D( m_mousePosition , m_capsule2D.m_capsuleMidStart , m_capsule2D.m_capsuleMidEnd , m_capsule2D.m_radius ) )
	{
		g_theRenderer->DrawCapsule2D( m_capsule2D.m_capsuleMidStart , m_capsule2D.m_capsuleMidEnd , m_capsule2D.m_radius , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnCapsule2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawCapsule2D( m_capsule2D.m_capsuleMidStart , m_capsule2D.m_capsuleMidEnd , m_capsule2D.m_radius , m_color );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnCapsule2D , 2.f ) , nearestPointColor );
	}
	
	if ( IsPointOnDisc2D( m_disc2D , m_mousePosition ) )
	{
		g_theRenderer->DrawDisc( m_disc2D , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnDisc2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawDisc( m_disc2D , m_color );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnDisc2D , 2.f ) , nearestPointColor );
	}

	if ( m_hasCursorChangedToOBB && DoOBBsOverlap( mouseCursor , m_Obb2D ) )
	{
		g_theRenderer->DrawOBB2( m_Obb2D , Rgba8( 255 , 255 , 0 , 255 ) );
		g_theRenderer->DrawOBB2( mouseCursor , Rgba8( 255 , 0 , 0 , 255 ) );
	}

	g_theRenderer->DrawLine( m_lineSegment2D.m_start, m_lineSegment2D.m_end , m_color , 5.f );

	g_theRenderer->DrawPolygon( &m_polygon[ 0 ] , ( uint ) m_polygon.size() , WHITE );

	DrawMouseCurrentPosition( m_worldCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DrawMouseCurrentPosition( const Camera& camera ) const
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedClientPosition();
	AABB2 orthoBounds = AABB2( camera.GetOrthoMin().GetXYComponents() , camera.GetOrthoMax().GetXYComponents() );
	Vec2 mouseDrawPos = orthoBounds.GetPointForUV( mouseNormalizedPos );
	//g_theRenderer->DrawRing( mouseDrawPos , 2.f , RED , 2.f );
	g_theRenderer->DrawDisc( Disc2D( mouseDrawPos , 2.5f ) , RED );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard()
{
	if ( g_theInput->WasKeyJustPressed( KEY_F8 ) )
	{
		Vec2 cameraBottomLeft = m_worldCamera.GetOrthoMin().GetXYComponents();
		Vec2 cameraTopRight = m_worldCamera.GetOrthoMax().GetXYComponents();
		RandomizeShapes( cameraBottomLeft , cameraTopRight , &m_rng );
		m_color.RollRandomColor( m_rng );
		m_color.a = 100;
	}

	if ( g_theInput->WasKeyJustPressed( 'C' ) )
	{
		m_hasCursorChangedToOBB = !m_hasCursorChangedToOBB;
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------