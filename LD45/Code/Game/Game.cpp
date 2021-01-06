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


extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;
//extern Camera* g_Camera;

Game::Game()
{
	m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( WORLD_SIZE_X , WORLD_SIZE_Y ) );
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_uiCamera.SetClearMode( CLEAR_NONE , BLACK );
	m_world1 = new World(this);
	m_tankBaseTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase1.png" );
	//m_tankBaseTexture = g_theRenderer->CreateTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	m_Waldo = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/stewie1.png" );

}

void Game::Update(float deltaSeconds)
{
	//UNUSED( deltaSeconds );
	UpdateCamera();
	
	m_world1->Update( deltaSeconds );
	GarbageCollection();
}

void Game::KeyboardKeyPressEvents()
{
	
}

void Game::Render() const
{
	g_theRenderer->BindTexture( m_tankBaseTexture );
	//g_theRenderer->DrawAABB2( m_aabbtest , m_testingColor );
	//g_theRenderer->BindTexture( nullptr );
	m_world1->Render();
}

void Game::RenderUI() const
{
	g_theRenderer->BeginCamera(m_uiCamera);

	g_theRenderer->EndCamera(m_uiCamera);
}

void Game::UpdateCamera()
{
	static RandomNumberGenerator rng;
	
	m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	m_screenShakeIntensity = ClampZeroToOne(m_screenShakeIntensity);

	float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast<float> (rng.RollRandomIntInRange((int)-maxShakeDist, (int)maxShakeDist));
	rng.manuallyIncrementPosition();
	float cameraShakeY = static_cast<float> (rng.RollRandomIntInRange((int)-maxShakeDist, (int)maxShakeDist));
	rng.manuallyIncrementPosition();
	
	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	m_worldCamera.Translate2D(Vec2(cameraShakeX, cameraShakeY));
	

	//-----------------------------------------------------------------------------------------------------------------
	//Updating UI Camera
	//-----------------------------------------------------------------------------------------------------------------

// 	for(int healthIcons = m_Ship->m_health; healthIcons > 0; healthIcons--)
// 	{
// 		RenderUI();
// 	}

}

void Game::AddScreenShakeIntensity(float deltaShakeIntensity)
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
