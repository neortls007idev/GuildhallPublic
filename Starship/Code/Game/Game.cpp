#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/DebrisColors.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern TheApp*				g_theApp;
//extern Camera*			g_Camera;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game ()
{
	m_uiCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_uiCamera->SetClearMode( CLEAR_NONE , BLACK );
	SpawnPlayerShip();
	SpawnInitialEnemies();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	delete m_uiCamera;
	m_uiCamera = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Levels ()
{
	if ( m_waveNumber < 5 && m_totalEnemies == 0 )
	{
		m_AsteroidSpawnCount = m_waveNumber + 6;
		m_BettleSpawnCount = m_waveNumber;
		m_WaspSpawnCount = 2 * m_waveNumber;
		m_totalEnemies = m_AsteroidSpawnCount + m_BettleSpawnCount + m_WaspSpawnCount;
		m_waveNumber++;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SpawnPlayerShip ()
{
	m_Ship = new PlayerShip( this , Vec2( WORLD_CENTER_X , WORLD_CENTER_Y ) , Vec2( 0.f , 0.f ) , 0.f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SpawnInitialEnemies ()
{
	SpawnNewAsteroidAtRandomPosition( m_AsteroidSpawnCount );
	SpawnNewBettleAtRandomPosition( m_BettleSpawnCount );
	SpawnNewWaspAtRandomPosition( m_WaspSpawnCount );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update ( float deltaSeconds )
{
	if ( m_waveNumber < 5 && m_totalEnemies == 0 )
	{
		Levels();
		SpawnInitialEnemies();
	}

	if ( ( ( m_Ship->m_age > 0 ) && ( m_Ship->m_health <= 0 ) ) )
	{
		WaitAndRestart( deltaSeconds );
	}

	if ( m_Ship->m_health > 0 && m_waveNumber == 5 )
	{
		WaitAndRestart( deltaSeconds );
		if ( m_Ship->m_age <= 0 )
		{
			return;
		}
	}

	m_Ship->Update( deltaSeconds );
	ReSpawn();
	KeyboardKeyPressEvents();
	MaxBulletWarning();
	UpdateBullet( deltaSeconds );
	MaxAsteroidWarning();
	UpdateAsteroid( deltaSeconds );
	UpdateBettles( deltaSeconds );
	UpdateWasps( deltaSeconds );
	UpdateDebris( deltaSeconds );
	DetectCollissionBetweenAstroidAndShip();
	DetectCollissionBetweenAstroidsAndBullets();
	DetectCollissionBetweenBettleAndShip();
	DetectCollisionBetweenBettleAndBullet();
	DetectCollissionBetweenWaspAndShip();
	DetectCollisionBetweenWaspAndBullet();
	UpdateCamera();

	GarbageCollection();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::KeyboardKeyPressEvents ()
{
	if ( g_theInput->WasKeyJustPressed( KEY_SPACE ) && g_theApp->m_isPaused == false && m_Ship->m_health > 0 )
	{
		AddScreenShakeIntensity( 0.2f );
		SpawnBullet( m_Ship->m_position , m_Ship->m_orientationDegrees );
	}
	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		SpawnNewAsteroidAtRandomPosition( m_AsteroidSpawnCount++ );
	}
	if ( g_theInput->WasKeyJustPressed( KEY_F1 ) )
	{
		m_devMode = !m_devMode;
	}
	if ( g_theInput->WasKeyJustPressed( 'N' ) == true && g_theApp->m_isPaused == false )
	{
		if ( m_Ship->m_isDead )
		{
			delete m_Ship;
			m_Ship = nullptr;
			m_Ship = new PlayerShip( g_theApp->m_theGame , Vec2( WORLD_CENTER_X , WORLD_CENTER_Y ) , Vec2( 0.f , 0.f ) ,
			                         0.f );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render () const
{
	m_Ship->Render();
	if ( m_devMode )
	{
		ShipDebugDrawing();
	}

	for ( int BettlePointerArrayIndex = 0 ; BettlePointerArrayIndex < MAX_BETTLES ; BettlePointerArrayIndex++ )
	{
		if ( m_Bettles[ BettlePointerArrayIndex ] != nullptr )
		{
			m_Bettles[ BettlePointerArrayIndex ]->Render();
		}
	}
	for ( int BulletPointerArrayIndex = 0 ; BulletPointerArrayIndex < MAX_BULLETS ; BulletPointerArrayIndex++ )
	{
		if ( m_Bullets[ BulletPointerArrayIndex ] != nullptr )
		{
			m_Bullets[ BulletPointerArrayIndex ]->Render();
			if ( m_devMode )
			{
				BulletDebugDrawing( BulletPointerArrayIndex );
			}
		}
	}

	for ( int AsteroidPointerArrayIndex = 0 ; AsteroidPointerArrayIndex < MAX_ASTEROIDS ; AsteroidPointerArrayIndex++ )
	{
		if ( m_Asteroids[ AsteroidPointerArrayIndex ] != nullptr && m_Asteroids[ AsteroidPointerArrayIndex ]->
			m_isGarbage == false )
		{
			m_Asteroids[ AsteroidPointerArrayIndex ]->Render();
			if ( m_devMode )
			{
				AsteroidDebugDrawing( AsteroidPointerArrayIndex );
			}
		}
	}

	for ( int WaspPointerArrayIndex = 0 ; WaspPointerArrayIndex < MAX_WASPS ; WaspPointerArrayIndex++ )
	{
		if ( m_Wasps[ WaspPointerArrayIndex ] != nullptr )
		{
			m_Wasps[ WaspPointerArrayIndex ]->Render();
		}
	}

	for ( int DebrisPointerArrayIndex = 0 ; DebrisPointerArrayIndex < MAX_DEBRIS ; DebrisPointerArrayIndex++ )
	{
		if ( m_Debris[ DebrisPointerArrayIndex ] != nullptr )
		{
			m_Debris[ DebrisPointerArrayIndex ]->Render();
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderUI () const
{
	g_theRenderer->BeginCamera( *m_uiCamera );
	Rgba8 iconColor = Rgba8( 132 , 156 , 169 , 127 );


	int lifeIcons = ( m_Ship->m_health );
	for ( --lifeIcons ; lifeIcons > 0 ; lifeIcons-- )
	{
		Vertex_PCU lives[] = {
			Vertex_PCU( ( Vec3( 2.f , 1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( 0.f , 2.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( -2.f , 1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( -2.f , 1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( -2.f , -1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( 0.f , 1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( 0.f , 1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( 1.f , 0.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) , // nose of the ship
			Vertex_PCU( ( Vec3( 0.f , -1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( 0.f , 1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( -2.f , -1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( 0.f , -1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( -2.f , -1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( 2.f , -1.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) ) ,
			Vertex_PCU( ( Vec3( 0.f , -2.f , 0.f ) ) , iconColor , Vec2( 0.f , 0.f ) )
		};

		TransformVertexArray2D( NUM_SHIP_VERTS , lives , 10.f , 0.f ,
		                        Vec2( ( 10.f + ( 50 * lifeIcons ) ) , UI_SIZE_Y - 50.f ) );
		g_theRenderer->DrawVertexArray( NUM_SHIP_VERTS , lives );
	}
	g_theRenderer->EndCamera( *m_uiCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera ()
{
	RandomNumberGenerator rng;


	m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	m_screenShakeIntensity = Clamp( m_screenShakeIntensity , 0.f , 1.f );

	float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast< float >( rng.RollRandomIntInRange( ( int )-maxShakeDist , ( int )maxShakeDist ) );
	float cameraShakeY = static_cast< float >( rng.RollRandomIntInRange( ( int )-maxShakeDist , ( int )maxShakeDist ) );

	g_theApp->g_theCamera->SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( WORLD_SIZE_X , WORLD_SIZE_Y ) );
	g_theApp->g_theCamera->Translate2D( Vec2( cameraShakeX , cameraShakeY ) );


	//-----------------------------------------------------------------------------------------------------------------
	//Updating UI Camera
	//-----------------------------------------------------------------------------------------------------------------

	// 	for(int healthIcons = m_Ship->m_health; healthIcons > 0; healthIcons--)
	// 	{
	// 		RenderUI();
	// 	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity ( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//Clamp it!
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Game::SpawnBullet ( Vec2 pos , float orientation )
{
	//AddScreenShakeIntensity(0.4f);
	//UpdateCamera();
	m_currentlyAliveBullets += 1;
	pos = pos + ( 2.5 * Vec2( CosDegrees( orientation ) , SinDegrees( orientation ) ) );

	for ( int BulletPointerArrayIndex = 0 ; BulletPointerArrayIndex < MAX_BULLETS ; BulletPointerArrayIndex++ )
	{
		if ( m_Bullets[ BulletPointerArrayIndex ] == nullptr )
		{
			m_Bullets[ BulletPointerArrayIndex ] = new Bullet( this , pos , orientation );
			m_Bullets[ BulletPointerArrayIndex ]->m_orientationDegrees = orientation;
			return m_currentlyAliveBullets;
		}
	}
	return m_currentlyAliveBullets;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::MaxBulletWarning ()
{
	if ( m_currentlyAliveBullets > MAX_BULLETS )
	{
		ERROR_RECOVERABLE( Stringf("This Error was caused due to spawning MAX Number of Bullets") );
		m_currentlyAliveBullets = m_currentlyAliveBullets - 1;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateBullet ( float deltaSeconds )
{
	for ( int BulletPointerArrayIndex = 0 ; BulletPointerArrayIndex < MAX_BULLETS ; BulletPointerArrayIndex++ )
	{
		if ( m_Bullets[ BulletPointerArrayIndex ] != nullptr )
		{
			m_Bullets[ BulletPointerArrayIndex ]->Update( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::MaxAsteroidWarning ()
{
	if ( m_AsteroidSpawnCount > MAX_ASTEROIDS )
	{
		ERROR_RECOVERABLE( Stringf("This Error was caused due to spawning MAX Number of Asteroids") );
		m_AsteroidSpawnCount = m_AsteroidSpawnCount - 1;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateAsteroid ( float deltaSeconds )
{
	for ( int AsteroidPointerArrayIndex = 0 ; AsteroidPointerArrayIndex < MAX_ASTEROIDS ; AsteroidPointerArrayIndex++ )
	{
		if ( m_Asteroids[ AsteroidPointerArrayIndex ] != nullptr && m_Asteroids[ AsteroidPointerArrayIndex ]->
			m_isGarbage == false )
		{
			m_Asteroids[ AsteroidPointerArrayIndex ]->Update( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateBettles ( float deltaSeconds )
{
	for ( int BettlePointerArrayIndex = 0 ; BettlePointerArrayIndex < MAX_BETTLES ; BettlePointerArrayIndex++ )
	{
		if ( m_Bettles[ BettlePointerArrayIndex ] != nullptr && m_Bettles[ BettlePointerArrayIndex ]->m_isGarbage ==
			false )
		{
			m_Bettles[ BettlePointerArrayIndex ]->Update( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateWasps ( float deltaSeconds )
{
	for ( int WaspsPointerArrayIndex = 0 ; WaspsPointerArrayIndex < MAX_WASPS ; WaspsPointerArrayIndex++ )
	{
		if ( m_Wasps[ WaspsPointerArrayIndex ] != nullptr && m_Wasps[ WaspsPointerArrayIndex ]->m_isGarbage == false )
		{
			m_Wasps[ WaspsPointerArrayIndex ]->Update( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SpawnDebrisField ( const Vec2& center , int howmany , const Rgba8& color )
{
	float orientationOffset = center.GetAngleDegrees();
	
	for ( int index = 0 ; index < howmany ; index++ )
	{
		for ( int DebrisPointerArrayIndex = 0 ; DebrisPointerArrayIndex < MAX_DEBRIS ; DebrisPointerArrayIndex++ )
		{
			Vec2 temp = Vec2::MakeFromPolarDegrees( center.GetAngleDegrees() , center.GetLength() );

			RandomNumberGenerator rng;
			float x = rng.RollRandomFloatLessThan( 5.0f );
			float y = rng.RollRandomFloatLessThan( 5.0f );
			if ( !m_Debris[ DebrisPointerArrayIndex ] )
			{
				orientationOffset += 360 / howmany;
				m_Debris[ DebrisPointerArrayIndex ] = new Debris( this , temp + Vec2( x , y ) , Vec2( 5.f , 5.f ) ,
				                                                  orientationOffset , color );
				break;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateDebris ( float deltaSeconds )
{
	for ( int DebrisPointerArrayIndex = 0 ; DebrisPointerArrayIndex < MAX_DEBRIS ; DebrisPointerArrayIndex++ )
	{
		if ( m_Debris[ DebrisPointerArrayIndex ] != nullptr && m_Debris[ DebrisPointerArrayIndex ]->m_isGarbage == false
		)
		{
			m_Debris[ DebrisPointerArrayIndex ]->Update( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::GarbageCollection ()
{
	for ( int index = 0 ; index < MAX_BULLETS ; index++ )
	{
		if ( m_Bullets[ index ] != nullptr )
		{
			if ( OutOfScreenBounds( m_Bullets[ index ] ) )
			{
				m_Bullets[ index ]->m_isGarbage = true;
			}
		}
	}

	for ( int index = 0 ; index < MAX_DEBRIS ; index++ )
		if ( m_Debris[ index ] != nullptr )
		{
			if ( OutOfScreenBounds( m_Debris[ index ] ) )
			{
				m_Debris[ index ]->m_isGarbage = true;
			}
		}
	GarbageDeletion();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::GarbageDeletion ()
{
	for ( int index = 0 ; index < MAX_ASTEROIDS ; index++ )
	{
		if ( m_Asteroids[ index ] != nullptr )
		{
			if ( m_Asteroids[ index ]->m_isGarbage == true )
			{
				delete m_Asteroids[ index ];
				m_Asteroids[ index ] = nullptr;
				m_AsteroidSpawnCount = m_AsteroidSpawnCount - 1;
				m_totalEnemies = m_totalEnemies - 1;
			}
		}
	}

	for ( int index = 0 ; index < MAX_BULLETS ; index++ )
	{
		if ( m_Bullets[ index ] != nullptr )
		{
			if ( m_Bullets[ index ]->m_isGarbage )
			{
				delete m_Bullets[ index ];
				m_Bullets[ index ] = nullptr;
				m_currentlyAliveBullets = m_currentlyAliveBullets - 1;
			}
		}
	}

	for ( int index = 0 ; index < MAX_BETTLES ; index++ )
	{
		if ( m_Bettles[ index ] != nullptr )
		{
			if ( m_Bettles[ index ]->m_isGarbage )
			{
				delete m_Bettles[ index ];
				m_Bettles[ index ] = nullptr;
				m_BettleSpawnCount = m_BettleSpawnCount - 1;
				m_totalEnemies = m_totalEnemies - 1;
			}
		}
	}

	for ( int index = 0 ; index < MAX_WASPS ; index++ )
	{
		if ( m_Wasps[ index ] != nullptr )
		{
			if ( m_Wasps[ index ]->m_isGarbage )
			{
				delete m_Wasps[ index ];
				m_Wasps[ index ] = nullptr;
				m_WaspSpawnCount = m_WaspSpawnCount - 1;
				m_totalEnemies = m_totalEnemies - 1;
			}
		}
	}
	for ( int index = 0 ; index < MAX_DEBRIS ; index++ )
	{
		if ( m_Debris[ index ] && m_Debris[ index ]->m_isGarbage )
		{
			delete m_Debris[ index ];
			m_Debris[ index ] = nullptr;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AsteroidDebugDrawing ( int AsteroidPointerArrayIndex ) const
{
	g_theRenderer->DrawLine( m_Asteroids[ AsteroidPointerArrayIndex ]->m_position ,
	                         ( m_Asteroids[ AsteroidPointerArrayIndex ]->m_position + m_Asteroids[
		                         AsteroidPointerArrayIndex ]->m_velocity ) , Rgba8( 255 , 255 , 0 , 255 ) , 0.25f );
	g_theRenderer->DrawLine( m_Ship->m_position , m_Asteroids[ AsteroidPointerArrayIndex ]->m_position ,
	                         Rgba8( 50 , 50 , 50 , 255 ) , 0.25f );
	g_theRenderer->DrawRing( m_Asteroids[ AsteroidPointerArrayIndex ]->m_position ,
	                         m_Asteroids[ AsteroidPointerArrayIndex ]->m_cosmeticRadius ,
	                         m_Asteroids[ AsteroidPointerArrayIndex ]->m_cosmeticRing , 0.25f );
	g_theRenderer->DrawRing( m_Asteroids[ AsteroidPointerArrayIndex ]->m_position ,
	                         m_Asteroids[ AsteroidPointerArrayIndex ]->m_physicsRadius ,
	                         m_Asteroids[ AsteroidPointerArrayIndex ]->m_physicsRing , 0.25f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::BulletDebugDrawing ( int BulletPointerArrayIndex ) const
{
	g_theRenderer->DrawLine( m_Ship->m_position , m_Bullets[ BulletPointerArrayIndex ]->m_position ,
	                         Rgba8( 50 , 50 , 50 , 255 ) , 0.25f );

	g_theRenderer->DrawRing( m_Bullets[ BulletPointerArrayIndex ]->m_position ,
	                         m_Bullets[ BulletPointerArrayIndex ]->m_cosmeticRadius ,
	                         m_Bullets[ BulletPointerArrayIndex ]->m_cosmeticRing , 0.25f );

	g_theRenderer->DrawRing( m_Bullets[ BulletPointerArrayIndex ]->m_position ,
	                         m_Bullets[ BulletPointerArrayIndex ]->m_physicsRadius ,
	                         m_Bullets[ BulletPointerArrayIndex ]->m_physicsRing , 0.25f );

	g_theRenderer->DrawLine( m_Bullets[ BulletPointerArrayIndex ]->m_position ,
	                         ( m_Bullets[ BulletPointerArrayIndex ]->m_velocity + m_Bullets[ BulletPointerArrayIndex ]->
		                         m_position ) , Rgba8( 255 , 255 , 0 , 255 ) , 0.25f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::ShipDebugDrawing () const
{
	g_theRenderer->DrawLine( m_Ship->m_position , ( m_Ship->m_velocity + m_Ship->m_position ) ,
	                         Rgba8( 255 , 255 , 0 , 255 ) , 0.25f );
	g_theRenderer->DrawRing( m_Ship->m_position , m_Ship->m_cosmeticRadius , m_Ship->m_cosmeticRing , 0.25f );
	g_theRenderer->DrawRing( m_Ship->m_position , m_Ship->m_physicsRadius , m_Ship->m_physicsRing , 0.25f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Game::SpawnNewAsteroidAtRandomPosition ( int newAsteroidNumber )
{
	for ( int AsteroidPointerArrayIndex = 0 ; AsteroidPointerArrayIndex < newAsteroidNumber ; AsteroidPointerArrayIndex
	      ++ )
	{
		if ( m_Asteroids[ AsteroidPointerArrayIndex ] == nullptr )
		{
			m_Asteroids[ AsteroidPointerArrayIndex ] = new Asteroid( this );
		}
	}
	return m_AsteroidSpawnCount;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Game::SpawnNewBettleAtRandomPosition ( int newBettleNumber )
{
	for ( int BettlePointerArrayIndex = 0 ; BettlePointerArrayIndex < newBettleNumber ; BettlePointerArrayIndex++ )
	{
		if ( m_Bettles[ BettlePointerArrayIndex ] == nullptr )
		{
			m_Bettles[ BettlePointerArrayIndex ] = new Bettle( this , Vec2( -5.0 , 0.0f ) , Vec2( 0.f , 0.f ) , 0.f );
		}
	}
	return m_BettleSpawnCount;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Game::SpawnNewWaspAtRandomPosition ( int newWaspNumber )
{
	for ( int WaspPointerArrayIndex = 0 ; WaspPointerArrayIndex < newWaspNumber ; WaspPointerArrayIndex++ )
	{
		if ( m_Wasps[ WaspPointerArrayIndex ] == nullptr )
		{
			m_Wasps[ WaspPointerArrayIndex ] = new Wasp( this , Vec2( -5.0 , 0.0f ) , Vec2( 0.f , 0.f ) , 0.f );
		}
	}
	return m_BettleSpawnCount;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Die ()
{
	m_Ship->IsDead();

	for ( int BulletPointerArrayIndex = 0 ; BulletPointerArrayIndex < MAX_BULLETS ; BulletPointerArrayIndex++ )
	{
		if ( m_Bullets[ BulletPointerArrayIndex ]->m_health == 0 )
		{
			delete m_Bullets[ BulletPointerArrayIndex ];
			m_totalEnemies--;
		}
	}
	for ( int AsteroidPointerArrayIndex = 0 ; AsteroidPointerArrayIndex < MAX_ASTEROIDS ; AsteroidPointerArrayIndex++ )
	{
		if ( m_Asteroids[ AsteroidPointerArrayIndex ]->m_health == 0 )
		{
			delete m_Asteroids[ AsteroidPointerArrayIndex ];
			m_totalEnemies--;
		}
	}
	for ( int BettlePointerArrayIndex = 0 ; BettlePointerArrayIndex < MAX_BETTLES ; BettlePointerArrayIndex++ )
	{
		if ( m_Bettles[ BettlePointerArrayIndex ]->m_health == 0 )
		{
			delete m_Bettles[ BettlePointerArrayIndex ];
			m_totalEnemies--;
		}
	}
	for ( int WaspPointerArrayIndex = 0 ; WaspPointerArrayIndex < MAX_WASPS ; WaspPointerArrayIndex++ )
	{
		if ( m_Wasps[ WaspPointerArrayIndex ]->m_health == 0 )
		{
			delete m_Wasps[ WaspPointerArrayIndex ];
			m_totalEnemies--;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::OutOfScreenBounds ( const Entity* entity )
{
	if ( entity->m_position.x - entity->m_cosmeticRadius > 200.f || entity->m_position.x + entity->m_cosmeticRadius <
		0.f
		|| entity->m_position.y - entity->m_cosmeticRadius > 100.f || entity->m_position.y + entity->m_cosmeticRadius <
		0.f )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DetectCollissionBetweenAstroidsAndBullets ()
{
	for ( int bulletIndex = 0 ; bulletIndex < MAX_BULLETS ; bulletIndex++ )
	{
		if ( m_Bullets[ bulletIndex ] != nullptr && m_Bullets[ bulletIndex ]->m_isGarbage != true )
		{
			for ( int astroidIndex = 0 ; astroidIndex < MAX_ASTEROIDS ; astroidIndex++ )
			{
				if ( m_Asteroids[ astroidIndex ] != nullptr && m_Asteroids[ astroidIndex ]->m_isGarbage != true )
				{
					if ( DoDiscsOverlap( m_Bullets[ bulletIndex ]->m_position ,
					                     m_Bullets[ bulletIndex ]->m_physicsRadius ,
					                     m_Asteroids[ astroidIndex ]->m_position ,
					                     m_Asteroids[ astroidIndex ]->m_physicsRadius ) )
					{
						AddScreenShakeIntensity( 0.3f );
						m_Asteroids[ astroidIndex ]->m_health--;
						m_Bullets[ bulletIndex ]->m_isGarbage = true;
						SpawnDebrisField( m_Bullets[ bulletIndex ]->m_position , NUM_BULLET_VERTS / 3 ,
						                  DEBRISBULLETCOLOR );
						SpawnDebrisField( m_Asteroids[ astroidIndex ]->m_position , NUM_ASTEROID_VERTS / 3 ,
						                  DEBRISASTEROIDCOLOR );

						if ( m_Asteroids[ astroidIndex ]->m_health <= 0 )
						{
							AddScreenShakeIntensity( 0.4f );
							m_Asteroids[ astroidIndex ]->m_isGarbage = true;
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DetectCollisionBetweenBettleAndBullet ()
{
	for ( int bulletIndex = 0 ; bulletIndex < MAX_BULLETS ; bulletIndex++ )
	{
		if ( m_Bullets[ bulletIndex ] != nullptr && m_Bullets[ bulletIndex ]->m_isGarbage != true )
		{
			for ( int bettleIndex = 0 ; bettleIndex < MAX_BETTLES ; bettleIndex++ )
			{
				if ( m_Bettles[ bettleIndex ] != nullptr && m_Bettles[ bettleIndex ]->m_isGarbage != true )
				{
					if ( DoDiscsOverlap( m_Bullets[ bulletIndex ]->m_position ,
					                     m_Bullets[ bulletIndex ]->m_physicsRadius ,
					                     m_Bettles[ bettleIndex ]->m_position ,
					                     m_Bettles[ bettleIndex ]->m_physicsRadius ) )
					{
						m_Bettles[ bettleIndex ]->m_health--;
						AddScreenShakeIntensity( 0.4f );
						m_Bullets[ bulletIndex ]->m_isGarbage = true;
						SpawnDebrisField( m_Bettles[ bettleIndex ]->m_position , NUM_BETTLE_VERTS / 3 ,
						                  DEBRISBETTLECOLOR );
						SpawnDebrisField( m_Bullets[ bulletIndex ]->m_position , NUM_BULLET_VERTS / 3 ,
						                  DEBRISBULLETCOLOR );

						if ( m_Bettles[ bettleIndex ]->m_health <= 0 )
						{
							AddScreenShakeIntensity( 0.5f );
							m_Bettles[ bettleIndex ]->m_isGarbage = true;
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DetectCollisionBetweenWaspAndBullet ()
{
	for ( int bulletIndex = 0 ; bulletIndex < MAX_BULLETS ; bulletIndex++ )
	{
		if ( m_Bullets[ bulletIndex ] != nullptr && m_Bullets[ bulletIndex ]->m_isGarbage != true )
		{
			for ( int waspIndex = 0 ; waspIndex < MAX_WASPS ; waspIndex++ )
			{
				if ( m_Wasps[ waspIndex ] != nullptr && m_Wasps[ waspIndex ]->m_isGarbage != true )
				{
					if ( DoDiscsOverlap( m_Bullets[ bulletIndex ]->m_position ,
					                     m_Bullets[ bulletIndex ]->m_physicsRadius , m_Wasps[ waspIndex ]->m_position ,
					                     m_Wasps[ waspIndex ]->m_physicsRadius ) )
					{
						m_Wasps[ waspIndex ]->m_health--;
						AddScreenShakeIntensity( 0.4f );
						m_Bullets[ bulletIndex ]->m_isGarbage = true;
						SpawnDebrisField( m_Wasps[ waspIndex ]->m_position , NUM_WASP_VERTS / 3 , DEBRISWASPCOLOR );
						SpawnDebrisField( m_Bullets[ bulletIndex ]->m_position , NUM_BULLET_VERTS / 3 ,
						                  DEBRISBULLETCOLOR );

						if ( m_Wasps[ waspIndex ]->m_health <= 0 )
						{
							AddScreenShakeIntensity( 0.5f );
							m_Wasps[ waspIndex ]->m_isGarbage = true;
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::WaitAndRestart ( float deltaSeconds )
{
	m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
	XboxController controller = g_theInput->GetXboxController( m_controllerID );
	m_Ship->m_age -= deltaSeconds;
	if ( m_Ship->m_age > 0 )
	{
		controller.SetVibration( m_Ship->m_age , m_Ship->m_age );
	}
	else
	{
		controller.SetVibration( 0.f , 0.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DetectCollissionBetweenAstroidAndShip ()
{
	for ( int index = 0 ; index < MAX_ASTEROIDS ; index++ )
	{
		if ( m_Asteroids[ index ] != nullptr )
		{
			if ( DoDiscsOverlap( m_Ship->m_position , m_Ship->m_physicsRadius , m_Asteroids[ index ]->m_position ,
			                     m_Asteroids[ index ]->m_physicsRadius ) )
			{
				m_Ship->m_health--;
				if ( m_Ship->m_health > 0 )
				{
					m_Ship->m_isDead = true;
					AddScreenShakeIntensity( 0.75f );
					SpawnDebrisField( m_Ship->m_position , NUM_SHIP_VERTS / 3 , DEBRISSHIPCOLOR );
					ReSpawn();
				}

				if ( m_Ship->m_health == 0 )
				{
					m_Ship->m_isDead = true;
					AddScreenShakeIntensity( 1.0f );
					SpawnDebrisField( m_Ship->m_position , NUM_SHIP_VERTS / 3 , DEBRISSHIPCOLOR );
				}
				if ( m_Asteroids[ index ]->m_health && m_Ship->m_health > 0 )
				{
					m_Asteroids[ index ]->m_health = 0;
					m_Asteroids[ index ]->m_isGarbage = true;
					SpawnDebrisField( m_Asteroids[ index ]->m_position , NUM_ASTEROID_VERTS / 3 , DEBRISASTEROIDCOLOR );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DetectCollissionBetweenBettleAndShip ()
{
	for ( int index = 0 ; index < MAX_BETTLES ; index++ )
	{
		if ( m_Bettles[ index ] != nullptr )
		{
			if ( DoDiscsOverlap( m_Ship->m_position , m_Ship->m_physicsRadius , m_Bettles[ index ]->m_position ,
			                     m_Bettles[ index ]->m_physicsRadius ) )
			{
				m_Ship->m_health--;
				if ( m_Ship->m_health > 0 )
				{
					m_Ship->m_isDead = true;
					AddScreenShakeIntensity( 0.75f );
					SpawnDebrisField( m_Ship->m_position , NUM_SHIP_VERTS / 3 , DEBRISSHIPCOLOR );
					ReSpawn();
				}

				if ( m_Ship->m_health == 0 )
				{
					m_Ship->m_isDead = true;
					AddScreenShakeIntensity( 1.f );
					SpawnDebrisField( m_Ship->m_position , NUM_SHIP_VERTS / 3 , DEBRISSHIPCOLOR );
				}
				if ( m_Bettles[ index ]->m_health && m_Ship->m_health > 0 )
				{
					m_Bettles[ index ]->m_health = 0;
					m_Bettles[ index ]->m_isGarbage = true;
					SpawnDebrisField( m_Bettles[ index ]->m_position , NUM_BETTLE_VERTS / 3 , DEBRISBETTLECOLOR );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DetectCollissionBetweenWaspAndShip ()
{
	for ( int index = 0 ; index < MAX_WASPS ; index++ )
	{
		if ( m_Wasps[ index ] != nullptr )
		{
			if ( DoDiscsOverlap( m_Ship->m_position , m_Ship->m_physicsRadius , m_Wasps[ index ]->m_position ,
			                     m_Wasps[ index ]->m_physicsRadius ) )
			{
				m_Ship->m_health--;
				if ( m_Ship->m_health > 0 )
				{
					m_Ship->m_isDead = true;
					AddScreenShakeIntensity( .75f );
					SpawnDebrisField( m_Ship->m_position , NUM_SHIP_VERTS / 3 , DEBRISSHIPCOLOR );
					ReSpawn();
				}

				if ( m_Ship->m_health == 0 )
				{
					AddScreenShakeIntensity( 1.f );
					m_Ship->m_isDead = true;
					SpawnDebrisField( m_Ship->m_position , NUM_SHIP_VERTS / 3 , DEBRISSHIPCOLOR );
				}
				if ( m_Wasps[ index ]->m_health && m_Ship->m_health > 0 )
				{
					m_Wasps[ index ]->m_health = 0;
					m_Wasps[ index ]->m_isGarbage = true;
					SpawnDebrisField( m_Wasps[ index ]->m_position , NUM_WASP_VERTS / 3 , DEBRISWASPCOLOR );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Game::GetPlayerPosition ()
{
	Vec2 currentPlayerPositon = m_Ship->m_position;
	return currentPlayerPositon;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Game::GetPlayerOrientation ()
{
	float currentPlayerOrientation = 0.f;
	currentPlayerOrientation = m_Ship->m_orientationDegrees;
	return currentPlayerOrientation;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::ReSpawn ()
{
	if ( m_Ship->m_isDead == true && ( m_Ship->m_health > 0 ) )
	{
		m_Ship->m_position = Vec2( WORLD_CENTER_X , WORLD_CENTER_Y );
		m_Ship->m_velocity = Vec2( 0.f , 0.f );
		m_Ship->m_isDead = false;
		m_Ship->m_age = 3.0f;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------