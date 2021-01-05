#include "Game/NpcTurret.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;
extern Game* g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

NpcTurret::NpcTurret( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction ) :
	Entity( CurrentGameInstance ,
		position ,
		velocity ,
		orientation ,
		type ,
		faction )
{
	InitializeNpcTurretMemberVariables();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NpcTurret::Update( float deltaSeconds )
{
	m_firingCoolDown -= deltaSeconds;


	Vec2 playerPosition = Vec2::ZERO;
	
	if ( m_theGame->m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
	{
		playerPosition = m_theGame->m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_position;
	}
	
	Vec2 displacementBetweenTurretAndPlayer = ( playerPosition - m_position );
	float directionToFace = displacementBetweenTurretAndPlayer.GetAngleDegrees();
	Vec2 ForwardDirection = displacementBetweenTurretAndPlayer.GetNormalized();

	if ( m_theGame->m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
	{
		if ( m_theGame->m_world1->m_currentMap->InLineOfSight( playerPosition , m_position , m_lineOfSightRadius ) && m_theGame->m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_health > 0 )
		{
			static bool isSoundPlayed = false;
			if ( !isSoundPlayed )
			{
				g_theAudioSystem->PlaySound( m_theGame->m_sounds[ SOUND_ENEMY_SPOTTED ] );
				isSoundPlayed = true;
			}
			m_turretOrientation = GetTurnedToward( m_turretOrientation , directionToFace , 90.0f * deltaSeconds );
			m_lastSeenPosition = playerPosition;
			m_lastSeen = true;

			if ( m_turretOrientation == directionToFace )
			{
				m_laser = m_theGame->m_world1->m_currentMap->RayCast( m_position , ForwardDirection , displacementBetweenTurretAndPlayer.GetLength() );
			}
			else
			{
				m_laser = m_theGame->m_world1->m_currentMap->RayCast( m_position , ForwardDirection , m_lineOfSightRadius );
			}
			if ( IsPointInSector( playerPosition , m_position , m_turretOrientation , 10.f , m_lineOfSightRadius ) )
			{
				if ( m_firingCoolDown <= 0.f )
				{
					m_theGame->m_world1->m_currentMap->SpawnNewEntity( EVIL_BULLET_ENTITY , m_faction , m_position , m_turretOrientation );
					SoundPlaybackID currentTurretShootID = g_theAudioSystem->PlaySound( m_theGame->m_sounds[ SOUND_ENEMY_SHOOT ] );
					float playbackBalance = CosDegrees( ( m_position - playerPosition ).GetAngleDegrees() );
					g_theAudioSystem->SetSoundPlaybackBalance( currentTurretShootID, playbackBalance );
					float playbackVolume = RangeMapFloat( 0.f , m_lineOfSightRadius , 1.0 , 0.f , ( m_position - playerPosition ).GetClamped( m_lineOfSightRadius ).GetLength() );
					g_theAudioSystem->SetSoundPlaybackVolume( currentTurretShootID , playbackVolume );
				}
			}
		}
		else if ( m_lastSeen )
		{
			Vec2 finalPosition = (m_lastSeenPosition-m_position).GetRotatedDegrees( 45.f );
			Vec2 initialPosition = (m_lastSeenPosition-m_position).GetRotatedDegrees( -45.f );

			if ( !m_hasReachedInitialPosition && !m_hasReachedFinalPosition )
			{
				m_goalDegrees = ( initialPosition ).GetAngleDegrees();
			}

			if ( abs( GetShortestAngularDisplacement( m_turretOrientation , m_goalDegrees ) ) <= 1.f && !m_hasReachedFinalPosition )
			{
				m_goalDegrees = ( finalPosition ).GetAngleDegrees();
				m_hasReachedFinalPosition = true;
				m_hasReachedInitialPosition = false;
			}

			if ( abs( GetShortestAngularDisplacement( m_turretOrientation , m_goalDegrees ) ) <= 1.f && !m_hasReachedInitialPosition )
			{
				m_goalDegrees = ( initialPosition ).GetAngleDegrees();
				m_hasReachedInitialPosition = true;
				m_hasReachedFinalPosition  = false;
			}

			m_turretOrientation = GetTurnedToward( m_turretOrientation , m_goalDegrees , 90.f * deltaSeconds );
			Vec2 laserPosition = m_position + Vec2( 0.4f * CosDegrees( m_turretOrientation ) , 0.4f * SinDegrees( m_turretOrientation ) );
			m_laser = m_theGame->m_world1->m_currentMap->RayCast( laserPosition , Vec2( CosDegrees( m_turretOrientation ) , SinDegrees( m_turretOrientation ) ) , m_lineOfSightRadius );
		}
		else
		{
			m_turretOrientation = GetTurnedToward( m_turretOrientation , m_turretOrientation - 90.f , 30.f * deltaSeconds );
			Vec2 laserPosition = m_position + Vec2( 0.4f * CosDegrees( m_orientationDegrees ) , 0.4f * SinDegrees( m_orientationDegrees ) );
			m_laser = m_theGame->m_world1->m_currentMap->RayCast( laserPosition , Vec2::MakeFromPolarDegrees( m_turretOrientation ) , m_lineOfSightRadius );
		}

		if ( m_firingCoolDown <= 0.f )
		{
			m_firingCoolDown = 1.3f;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NpcTurret::Render() const
{
	if (m_faction == FACTION_ALLY)
	{
		g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_FRIENDLY_TURRET_BASE ] );
	}
	else
	{
		g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_ENEMY_TURRET_BASE ] );
	}
	Vertex_PCU tempVerts[ 6 ];
	for ( int index = 0; index < 6; index++ )
	{
		tempVerts[ index ] = m_npcTurretBaseVerts[ index ];
	}
	TransformVertexArray2D( 6 , tempVerts , 1.f , m_orientationDegrees , m_position );
	g_theRenderer->DrawVertexArray( 6 , tempVerts );
	g_theRenderer->BindTexture( nullptr );

	g_theRenderer->DrawLine( m_position , m_laser.m_impactPosition , RED , 0.02f );

	
	if ( m_faction == FACTION_ALLY )
	{
		g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_FRIENDLY_TURRET ] );
	}
	else
	{
		g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_ENEMY_TURRET ] );
	}
	for ( int index = 0; index < 6; index++ )
	{
		tempVerts[ index ] = m_npcTurretVerts[ index ];
	}
	TransformVertexArray2D( 6 , tempVerts , 1.f , m_turretOrientation , m_position );
	g_theRenderer->DrawVertexArray( 6 , tempVerts );
	g_theRenderer->BindTexture( nullptr );

	//--------------------------------------------------------------------------------------------------------------------------------------------

	if ( g_theGame->m_debugDraw )
	{
		g_theRenderer->DrawRing( m_position , m_cosmeticRadius , m_cosmeticRing , 0.02f );
		g_theRenderer->DrawRing( m_position , m_physicsRadius , m_physicsRing , 0.02f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NpcTurret::IsDead()
{
	m_isDead = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NpcTurret::InitializeNpcTurretMemberVariables()
{
	m_velocity = Vec2::ZERO;
	m_health = 2;
	m_isDead = false;
	m_isGarbage = false;
	m_physicsRadius = 0.2f;
	m_cosmeticRadius = 0.4f;
	m_age = 0.0f;
	m_lineOfSightRadius = 15.0f;
	m_firingCoolDown = 1.3f;

	m_isPushedByEntities = false;
	m_pushesEntiites	 = true;
	m_isPushedByWalls	 = true;
	m_isHitByBullets	 = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------