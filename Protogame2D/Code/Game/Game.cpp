#include "Game/GameCommon.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


#include "Game/TheApp.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Game.hpp"
#include <algorithm>


extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;


Game::Game()
{
	m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y ) );
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_world1 = new World( this );
	m_tankBaseTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_tankTurretTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankTop.png" );
	
}


void Game::Update( float deltaSeconds )
{
	UpdateFromKeyBoard();
	if ( !g_theApp->m_debugCamera )
	{
		UpdateCamera();
	}
	m_world1->Update( deltaSeconds );
	if ( !m_isClipping )
	{ CheckCollision();	}
//	PushMobileDiscOutOfFixedDisc( m_world1->m_currentMap->m_entities[ 0 ]->m_position , m_entities[ 0 ]->m_physicsRadius , m_entities[ 1 ]->m_position , m_entities[ 1 ]->m_physicsRadius );
	GarbageCollection();
}

void Game::Render() const
{
	g_theRenderer->BindTexture( nullptr );
	m_world1->Render();
	
}




void Game::RenderUI() const
{
	g_theRenderer->BeginCamera( m_uiCamera );

	g_theRenderer->EndCamera( m_uiCamera );
}

void Game::CheckCollision()
{
	Vec2 playerPosition = m_world1->m_currentMap->m_entities[ 0 ]->m_position;

	IntVec2 currentTileCoords = IntVec2( RoundDownToInt( playerPosition.x ) , RoundDownToInt( playerPosition.y ) );

	IntVec2 leftCoords        = IntVec2( currentTileCoords.x - 1 , currentTileCoords.y );
	IntVec2 rightCoords       = IntVec2( currentTileCoords.x + 1 , currentTileCoords.y );
	IntVec2 upCoords	      = IntVec2( currentTileCoords.x , currentTileCoords.y + 1 );
	IntVec2 downCoords		  = IntVec2( currentTileCoords.x , currentTileCoords.y - 1 );
	IntVec2 topLeftCoords     = IntVec2( currentTileCoords.x - 1 , currentTileCoords.y + 1 );
	IntVec2 bottomLeftCoords  = IntVec2( currentTileCoords.x - 1 , currentTileCoords.y - 1 );
	IntVec2 topRightCoords    = IntVec2( currentTileCoords.x + 1 , currentTileCoords.y + 1 );
	IntVec2 bottomRightCoords = IntVec2( currentTileCoords.x + 1 , currentTileCoords.y - 1 );
	AABB2 leftAABB			  = AABB2( leftCoords.x , leftCoords.y , leftCoords.x + 1 , leftCoords.y + 1 );
	AABB2 rightAABB			  = AABB2( rightCoords.x , rightCoords.y , rightCoords.x + 1 , rightCoords.y + 1 );
	AABB2 upAABB			  = AABB2( upCoords.x , upCoords.y , upCoords.x + 1 , upCoords.y + 1 );
	AABB2 downAABB			  = AABB2( downCoords.x , downCoords.y , downCoords.x + 1 , downCoords.y + 1 );
	AABB2 topLeftAABB		  = AABB2( topLeftCoords.x , topLeftCoords.y , topLeftCoords.x + 1 , topLeftCoords.y + 1 );
	AABB2 bottomRightAABB	  = AABB2( bottomRightCoords.x , bottomRightCoords.y , bottomRightCoords.x + 1 , bottomRightCoords.y + 1 );
	AABB2 topRightAABB		  = AABB2( topRightCoords.x , topRightCoords.y , topRightCoords.x + 1 , topRightCoords.y + 1 );
	AABB2 bottomLeftAABB	  = AABB2( bottomLeftCoords.x , bottomLeftCoords.y , bottomLeftCoords.x + 1 , bottomLeftCoords.y + 1 );
	int leftIndex			  = m_world1->m_currentMap->GetTileIndexforTileCoords( leftCoords );
	int rightIndex		      = m_world1->m_currentMap->GetTileIndexforTileCoords( rightCoords );
	int upIndex				  = m_world1->m_currentMap->GetTileIndexforTileCoords( upCoords );
	int downIndex			  = m_world1->m_currentMap->GetTileIndexforTileCoords( downCoords );
	int topLeftIndex		  = m_world1->m_currentMap->GetTileIndexforTileCoords( topLeftCoords );
	int bottomLeftIndex		  = m_world1->m_currentMap->GetTileIndexforTileCoords( bottomLeftCoords );
	int topRighttIndex		  = m_world1->m_currentMap->GetTileIndexforTileCoords( topRightCoords );
	int bottomRightIndex	  = m_world1->m_currentMap->GetTileIndexforTileCoords( bottomRightCoords );

	float tankCosmeticRadius = m_world1->m_currentMap->m_entities[ 0 ]->m_physicsRadius;

	if ( m_world1->m_currentMap->m_tiles[ leftIndex ].m_type == TILE_TYPE_STONE )
	{
		PushDiscOutOfAABB( playerPosition , tankCosmeticRadius , leftAABB );
	}
	if ( m_world1->m_currentMap->m_tiles[ rightIndex ].m_type == TILE_TYPE_STONE )
	{
		PushDiscOutOfAABB( playerPosition , tankCosmeticRadius , rightAABB );
	}
	if ( m_world1->m_currentMap->m_tiles[ upIndex ].m_type == TILE_TYPE_STONE )
	{
		PushDiscOutOfAABB( playerPosition , tankCosmeticRadius , upAABB );
	}
	if ( m_world1->m_currentMap->m_tiles[ downIndex ].m_type == TILE_TYPE_STONE )
	{
		PushDiscOutOfAABB( playerPosition , tankCosmeticRadius , downAABB );
	}
	if ( m_world1->m_currentMap->m_tiles[ topRighttIndex ].m_type == TILE_TYPE_STONE )
	{
		PushDiscOutOfAABB( playerPosition , tankCosmeticRadius , topRightAABB );
	}
	if ( m_world1->m_currentMap->m_tiles[ topLeftIndex ].m_type == TILE_TYPE_STONE )
	{
		PushDiscOutOfAABB( playerPosition , tankCosmeticRadius , topLeftAABB );
	}
	if ( m_world1->m_currentMap->m_tiles[ bottomRightIndex ].m_type == TILE_TYPE_STONE )
	{
		PushDiscOutOfAABB( playerPosition , tankCosmeticRadius , bottomRightAABB );
	}
	if ( m_world1->m_currentMap->m_tiles[ bottomLeftIndex ].m_type == TILE_TYPE_STONE )
	{
		PushDiscOutOfAABB( playerPosition , tankCosmeticRadius , bottomLeftAABB );
	}
	m_world1->m_currentMap->m_entities[ 0 ]->m_position = playerPosition;
	
}

void Game::UpdateCamera()
{
	//-----------------------------------------------------------------------------------------------------------------
	// Clamping the Camera
	//-----------------------------------------------------------------------------------------------------------------

	Vec2 tankPosition = m_world1->m_currentMap->m_entities[ 0 ]->m_position;
	Vec2 cameraCoords;
	
	cameraCoords.x = Clamp( tankPosition.x , ( m_tilesInViewHorizontally / 2 ) , WORLD_SIZE_X - ( m_tilesInViewHorizontally / 2 ) );
	cameraCoords.y = Clamp( tankPosition.y , ( m_tilesInViewVertically / 2 ) , WORLD_SIZE_Y - ( m_tilesInViewVertically / 2 ) );

	m_worldCamera.SetOrthoView( cameraCoords - m_CameraCenter , cameraCoords + m_CameraCenter );

	m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	m_screenShakeIntensity = ClampZeroToOne( m_screenShakeIntensity );

	float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	float cameraShakeY = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );

	
	m_worldCamera.Translate2D( Vec2( cameraShakeX , cameraShakeY ) );


	//-----------------------------------------------------------------------------------------------------------------
	//Updating UI Camera
	//-----------------------------------------------------------------------------------------------------------------

// 	for(int healthIcons = m_Ship->m_health; healthIcons > 0; healthIcons--)
// 	{
// 		RenderUI();
// 	}

}

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}





void Game::GarbageCollection()
{

	GarbageDeletion();

}

void Game::GarbageDeletion()
{

}


void Game::Die()
{

}

void Game::UpdateFromKeyBoard()
{
	if ( g_theInput->GetButtonState( KEY_F1 ).WasJustPressed() )
	{ m_debugDraw = !m_debugDraw; }
	
	if ( g_theInput->GetButtonState( KEY_F3 ).WasJustPressed() ) 
	{ m_isClipping = !m_isClipping; }
	
}