#include "Game/NpcTank.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem*		g_theInput;
extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;
extern TheApp*			g_theApp;
extern Game*			g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

NpcTank::NpcTank( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction ) :
	Entity( CurrentGameInstance ,
		position ,
		velocity ,
		orientation ,
		type ,
		faction )
{
	InitializeNpcTankMemberVariables();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NpcTank::Update( float deltaSeconds )
{
	m_firingCoolDown -= deltaSeconds;
	m_orientationChangeTimer -= deltaSeconds;
	m_velocity = Vec2::MakeFromPolarDegrees( m_orientationDegrees );
	Vec2 playerPosition = Vec2::ONE;
	Map* currentMap = m_theGame->m_world1->m_currentMap;
	
	m_whiskers[0] = m_theGame->m_world1->m_currentMap->RayCast( m_position , Vec2( CosDegrees( m_orientationDegrees ) , SinDegrees( m_orientationDegrees ) ) , 0.5f );
	m_whiskers[1] = m_theGame->m_world1->m_currentMap->RayCast( m_position - Vec2( 0.2f * CosDegrees( m_orientationDegrees ) , 0.2f * SinDegrees( m_orientationDegrees ) ).GetRotated90Degrees() , Vec2( CosDegrees( m_orientationDegrees ) , SinDegrees( m_orientationDegrees ) ) , 0.5f );
	m_whiskers[2] = m_theGame->m_world1->m_currentMap->RayCast( m_position + Vec2( 0.2f * CosDegrees( m_orientationDegrees ) , 0.2f * SinDegrees( m_orientationDegrees ) ).GetRotated90Degrees() , Vec2( CosDegrees( m_orientationDegrees ) , SinDegrees( m_orientationDegrees ) ) , 0.5f );
	
	if ( m_orientationChangeTimer <= 0.f )
	{
		m_newGoalOrientation = g_RNG->RollRandomFloatLessThan( 360.f );
		m_orientationChangeTimer = 2.0f;
		m_positionBeforeASecond = m_position;
	}
	if ( currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
	{
		playerPosition = currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_position;
	}
	Vec2 displacementBetweenNpcTankAndPlayer =  playerPosition - m_position ;
	
	if ( currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
	{
		if ( currentMap->InLineOfSight( playerPosition , m_position , m_lineOfSightRadius ) && currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_health > 0 )
		{
			static bool isSoundPlayed = false;
			if ( !isSoundPlayed )
			{
				g_theAudioSystem->PlaySound( m_theGame->m_sounds[ SOUND_ENEMY_SPOTTED ] );
				isSoundPlayed = true;
			}

			float goalDegrees = displacementBetweenNpcTankAndPlayer.GetAngleDegrees();
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , goalDegrees , 30.f * deltaSeconds );
			m_lastSeenPosition = playerPosition;
			m_wasPlayerSeen = true;

			if ( IsPointInSector( playerPosition , m_position , m_orientationDegrees , 45.f , m_lineOfSightRadius ) )
			{
				Entity::Movement( deltaSeconds );
			}

			if ( IsPointInSector( playerPosition , m_position , m_orientationDegrees , 10.f , m_lineOfSightRadius ) )
			{
				if ( m_firingCoolDown <= 0.f )
				{
					currentMap->SpawnNewEntity( EVIL_BULLET_ENTITY , m_faction , m_position , m_orientationDegrees );
					SoundPlaybackID currentNPCTankShootID = g_theAudioSystem->PlaySound( m_theGame->m_sounds[ SOUND_ENEMY_SHOOT ] );
					float playbackBalance = CosDegrees( ( m_position - playerPosition ).GetAngleDegrees() );
					g_theAudioSystem->SetSoundPlaybackBalance( currentNPCTankShootID , playbackBalance );
					float playbackVolume = RangeMapFloat( 0.f , m_lineOfSightRadius , 1.0 , 0.f , ( m_position - playerPosition ).GetClamped( m_lineOfSightRadius ).GetLength() );
					g_theAudioSystem->SetSoundPlaybackVolume( currentNPCTankShootID , playbackVolume );
					m_firingCoolDown = 1.7f;
				}
			}
		}
		else if ( m_whiskers[ 0 ].m_impactFraction < 1.f || m_whiskers[ 1 ].m_impactFraction < 1.f || m_whiskers[ 2 ].m_impactFraction < 1.f )
		{
			m_newGoalOrientation = m_orientationDegrees + 90.f;
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , m_newGoalOrientation , 180.f * deltaSeconds );
			Entity::Movement( deltaSeconds );
		}
		else if ( m_wasPlayerSeen )
		{
			m_newGoalOrientation = ( m_lastSeenPosition - m_position ).GetAngleDegrees();
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , m_newGoalOrientation , 75.f * deltaSeconds );
			Entity::Movement( deltaSeconds );

			if ( ( int ) m_position.GetLength() == ( int ) m_lastSeenPosition.GetLength() )
			{
				m_wasPlayerSeen = false;
			}
		}
		else
		{
			//m_orientationChangeTimer -= deltaSeconds;
			//m_orientationDegrees = GetTurnedToward( m_orientationDegrees , m_newGoalOrientation , 45.f * deltaSeconds );
			Entity::Movement( deltaSeconds );
		}


		if ( ( m_positionBeforeASecond - m_position ).GetLength() <= 0.001f )
		{
			m_orientationChangeTimer -= deltaSeconds;
			if ( m_orientationChangeTimer <= 0.f )
			{
				m_newGoalOrientation = g_RNG->RollRandomFloatLessThan( 360.f );
				m_orientationChangeTimer = 2.f;
			}
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , m_newGoalOrientation , 180.f * deltaSeconds );
			Entity::Movement( deltaSeconds );
			m_wasPlayerSeen = false;
		}
		else
		{
			//m_orientationChangeTimer -= deltaSeconds;
			//static float newGoalOrientation = 0.f; 
			m_orientationDegrees = GetTurnedToward( m_orientationDegrees , m_newGoalOrientation , 180.f * deltaSeconds );
			Entity::Movement( deltaSeconds );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NpcTank::Render() const
{
	if ( m_faction == FACTION_ALLY )
	{
		g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_FRIENDLY_TANK2 ] );
	}
	else
	{
		g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_ENEMY_TANK3 ] );
	}
	//g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_FRIENDLY_TANK1 ] );
	Vertex_PCU tempTank[ 6 ];
	
	for ( int index = 0; index < 6; index++ )
	{
		tempTank[ index ] = m_npcTankVerts[ index ];
	}
	
	TransformVertexArray2D( 6 , tempTank , 1.f , m_orientationDegrees , m_position );
	g_theRenderer->DrawVertexArray( 6 , tempTank );
	g_theRenderer->BindTexture( nullptr );

//--------------------------------------------------------------------------------------------------------------------------------------------

	if ( g_theGame->m_debugDraw )
	{
		g_theRenderer->DrawRing( m_position , m_cosmeticRadius , m_cosmeticRing , 0.02f );
		g_theRenderer->DrawRing( m_position , m_physicsRadius , m_physicsRing , 0.02f );
		g_theRenderer->DrawLine( m_position , m_whiskers[0].m_impactPosition , Rgba8( 0 , 0 , 100 ) , 0.05f );
		g_theRenderer->DrawLine( m_position , m_whiskers[1].m_impactPosition , Rgba8( 0 , 0 , 100 ) , 0.05f );
		g_theRenderer->DrawLine( m_position , m_whiskers[2].m_impactPosition , Rgba8( 0 , 0 , 100 ) , 0.05f );
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NpcTank::IsDead()
{
	m_isDead = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NpcTank::InitializeNpcTankMemberVariables()
{
	m_velocity				 = Vec2::MakeFromPolarDegrees( m_orientationDegrees , 1.f );
	m_health				 = 3;
	m_isDead				 = false;
	m_isGarbage				 = false;
	m_physicsRadius			 = 0.2f;
	m_cosmeticRadius		 = 0.4f;
	m_age					 = 0.f;
	m_firingCoolDown		 = 1.7f;
	m_lineOfSightRadius		 = 10.0f;
	m_orientationChangeTimer = 2.0f;

	m_isPushedByEntities = true;
	m_pushesEntiites	 = true;
	m_isPushedByWalls	 = true;
	m_isHitByBullets	 = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------