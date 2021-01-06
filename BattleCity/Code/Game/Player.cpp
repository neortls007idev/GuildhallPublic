#include "Game/player.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//		GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*			g_theRenderer;
extern TheApp*					g_theApp;
extern AudioSystem*				g_theAudioSystem;
extern DevConsole*				g_theDevConsole;
extern RandomNumberGenerator*	g_RNG;
extern BitmapFont*				g_bitmapFont;

//--------------------------------------------------------------------------------------------------------------------------------------------

Player::Player( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation, EntityType type, Faction faction ) :
	Entity( CurrentGameInstance ,
		position ,
		velocity ,
		orientation, 
		type,
		faction
		)
{
	InitializePlayerMemberVariables();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::Update( float deltaSeconds )
{
	m_lastSpawnedBullet = nullptr;
	//UpdateFromJoystick( deltaSeconds );
	//if ( m_isDead )
	if ( !IsAlive() )
	{
		return;
	}
	if( !g_theDevConsole->IsOpen() )
	{
		checkKeyboardKeyPressForMovement( deltaSeconds );
	}
	Entity::WrapAroundScreen();
	
	m_didPlayerMoveLastFrame = m_didPlayerMoveThisFrame;
	m_didPlayerMoveThisFrame = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::UpdatePlayerColor( Rgba8 playerTint )
{
	for ( int index = 0; index < 6; index++ )
	{
		m_playerTankVerts[ index ].m_color = playerTint;
		m_playerTurretVerts[ index ].m_color = playerTint;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::Render() const
{
	
	g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_PLAYER_TANK ] );
	Vertex_PCU tempTank[6];
	for (int index = 0; index < 6;index++)
	{
		tempTank[ index ] = m_playerTankVerts[index];
	}
	TransformVertexArray2D( 6 , tempTank , 1.f , m_orientationDegrees , m_position );
	g_theRenderer->DrawVertexArray( 6 , tempTank );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_PLAYER_TURRET ] );
	Vertex_PCU tempTurret[ 6 ];
	for ( int index = 0; index < 6; index++ )
	{
		tempTurret[ index ] = m_playerTurretVerts[ index ];
	}
	TransformVertexArray2D( 6 , tempTurret , 1.f , m_turretOrientation + m_orientationDegrees , m_position );
	g_theRenderer->DrawVertexArray( 6 , tempTurret );
	g_theRenderer->BindTexture( nullptr );

	AABB2 healthBar( 0.f , 0.f , 0.8f , 0.2f );
	Vec2 healthBarPos = Vec2( m_position.x - .4f , m_position.y - .8f );
	healthBar.Translate( healthBarPos );
	//g_theRenderer->DrawAABB2( healthBar , BLACK );
	//float healthPercent = RangeMapFloatNormalizedOutput( 0.f , ( float ) TOTAL_PLAYER_LIVES * PLAYER_HEALTH_PER_LIVES , ( float ) m_health );
	//healthBar = healthBar.GetBoxAtLeft( 1.f - healthPercent , 0.f );
	//g_theRenderer->DrawAABB2( healthBar , GREEN );
	//--------------------------------------------------------------------------------------------------------------------------------------------
	std::vector< Vertex_PCU > healthVerts;
	if( m_health > 7 )
	{
		g_bitmapFont->AddVertsForTextInBox2D( healthVerts , healthBar , .3f , ToString( m_health ) , GREEN , 1.f , ALIGN_CENTERED );
	}
	else if( m_health > 3 )
	{
		g_bitmapFont->AddVertsForTextInBox2D( healthVerts , healthBar , .3f , ToString( m_health ) , YELLOW , 1.f , ALIGN_CENTERED );
	}
	else
	{
		g_bitmapFont->AddVertsForTextInBox2D( healthVerts , healthBar , .3f , ToString( m_health ) , RED , 1.f , ALIGN_CENTERED );
	}
	g_theRenderer->BindTexture( g_bitmapFont->GetTexture() );
	if ( healthVerts.size() > 0 )
	{
		g_theRenderer->DrawVertexArray( healthVerts );
	}
	g_theRenderer->BindTexture( nullptr );
	//if (g_theGame->m_debugDraw)
	//{
	//	g_theRenderer->DrawRing( m_position , m_cosmeticRadius , m_cosmeticRing , 0.02f );
	//	g_theRenderer->DrawRing( m_position , m_physicsRadius , m_physicsRing , 0.02f );
	//}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::IsDead()
{
	m_isDead = true;
	m_lastSpawnedBullet = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::UpdateFromJoystick( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
	// Do nothing if this player ship has no controller assigned to it
	if ( m_controllerID < 0 )
		return;

	// Do nothing if this player's controller is not plugged in
	const XboxController& controller = g_theInput->GetXboxController( m_controllerID );
	if ( !controller.IsConnected() )
		return;
	//if ( g_theApp->m_isPaused == false )
	//{
	//	XboxButtonPresses( controller , deltaSeconds );
	//}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::checkKeyboardKeyPressForMovement( float deltaSeconds )
{
	if ( !m_isDead /*|| !g_theApp->m_isPaused*/ )
	{
		if ( g_theInput->WasKeyJustPressed( KEY_SPACE ) )
		{
			 m_lastSpawnedBullet = m_theGame->m_world->m_currentMap->SpawnNewEntity( GOOD_BULLET_ENTITY , m_faction , m_position , m_turretOrientation + m_orientationDegrees );
			 //m_theGame->m_world->m_currentMap->SpawnNewEntity( EXPLOSION_ENTITY , m_faction , m_position , m_turretOrientation + m_orientationDegrees );
			// //SoundID testSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerShoot.wav" );
			// g_theAudioSystem->PlaySound( m_theGame->m_sounds[ SOUND_PLAYER_SHOOT ] );
		}
		if ( g_theInput->IsKeyHeldDown( 'E' ) )
		{
			RotateTankUp( deltaSeconds );
		}
		if ( g_theInput->IsKeyHeldDown( 'S' ) )
		{
			RotateTankLeft( deltaSeconds );
		}
		if ( g_theInput->IsKeyHeldDown( 'F' ) )
		{
			RotateTankRight( deltaSeconds );

		}
		if ( g_theInput->IsKeyHeldDown( 'D' ) )
		{
			RotateTankDown( deltaSeconds );
		}
		if ( g_theInput->IsKeyHeldDown( 'E' ) && g_theInput->IsKeyHeldDown( 'F' ) )
		{
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , 45.f , 180.f * deltaSeconds );
		}
		if ( g_theInput->IsKeyHeldDown( 'E' ) && g_theInput->IsKeyHeldDown( 'S' ) )
		{
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , 135.f , 180.f * deltaSeconds );
		}
		if ( g_theInput->IsKeyHeldDown( 'D' ) && g_theInput->IsKeyHeldDown( 'S' ) )
		{
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , 225.f , 180.f * deltaSeconds );
		}
		if ( g_theInput->IsKeyHeldDown( 'D' ) && g_theInput->IsKeyHeldDown( 'F' ) )
		{
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , 315.f , 180.f * deltaSeconds );
		}

		if ( g_theInput->IsKeyHeldDown( 'D' ) || g_theInput->IsKeyHeldDown( 'E' ) || g_theInput->IsKeyHeldDown( 'S' ) || g_theInput->IsKeyHeldDown( 'F' ) )
		{
			TankMovement( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::TankMovement( float deltaSeconds )
{
	m_didPlayerMoveThisFrame = true;
	m_velocity =  Vec2( 1.5f * CosDegrees( m_orientationDegrees ) , 1.5f * SinDegrees( m_orientationDegrees ) ) ;
	Entity::Movement( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateTankLeft( float deltaSeconds )
{
	m_orientationDegrees = GetTurnedToward( m_orientationDegrees , 180.f , 180.f * deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateTankRight( float deltaSeconds )
{
	m_orientationDegrees = GetTurnedToward( m_orientationDegrees , 0.f , 180.f * deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateTankUp( float deltaSeconds )
{
	m_orientationDegrees = GetTurnedToward( m_orientationDegrees , 90.f , 180.f * deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateTankDown( float deltaSeconds )
{
	m_orientationDegrees = GetTurnedToward( m_orientationDegrees , 270.f , 180.f * deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateTurretLeft( float deltaSeconds )
{
	m_turretOrientation = GetTurnedToward( m_turretOrientation , 180.f , 180.f*deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateTurretRight( float deltaSeconds )
{
	m_turretOrientation = GetTurnedToward( m_turretOrientation , 0.f , 180.f*deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateTurretUp( float deltaSeconds )
{
	m_turretOrientation = GetTurnedToward( m_turretOrientation , 90.f , 180.f * deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::RotateTurretDown( float deltaSeconds )
{
	m_turretOrientation = GetTurnedToward( m_turretOrientation , 270.f , 180.f * deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Player::InitializePlayerMemberVariables()
{
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees , 1.5f );
	m_health = TOTAL_PLAYER_LIVES * PLAYER_HEALTH_PER_LIVES;
	m_isDead = false;
	m_isGarbage = false;
	m_physicsRadius = 0.2f;
	m_cosmeticRadius = 0.4f;
	m_age = 3.0f;

	m_isPushedByEntities = true;
	m_pushesEntiites	 = true;
	m_isPushedByWalls	 = true;
	m_isHitByBullets	 = true;
	
	m_turretOrientation = 0.f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
