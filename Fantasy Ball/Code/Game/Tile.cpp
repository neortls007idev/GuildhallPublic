#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ParticleSystem/ParticleEmitter2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Ball.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/PowerUpDefinition.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*				g_theRenderer;
extern AudioSystem*					g_theAudioSystem;

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile::Tile( Map* owner , IntVec2 tileCoordinates , TileDefinition* tileType ) :
																								Entity( owner->m_owner , 0 , eEntityType::TILE ) ,
																								m_tileDef( tileType ) ,
																								m_tileCoords( tileCoordinates ) ,
																								m_ownerMap( owner )
{
	SetHealth( m_tileDef->m_health );
	m_tileColor = m_tileDef->m_tileColor;
	m_bounds = CreateColliderFromCoords();

	m_solidAtHealth		= tileType->m_solidAtHealth;
	m_visibleAtHealth	= tileType->m_visibleAtHealth;
	m_isSolid			= tileType->m_isSolid;
	m_isVisible			= tileType->m_isVisible;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_animTime += deltaSeconds;


//  	float sideMovement = RangeMapFloat( 0.f , 10.f , -TILE_DEVIATION , TILE_DEVIATION , m_animTime );
//  	m_bounds.Translate( Vec2( sideMovement , 0.f ) );
//  	
//  	if ( m_animTime >= 10.f )
//  	{
//  		m_animTime = 0.f;
//  	}
// 	if( m_animTime <= 1.0f )						{ m_tileAnim = MOVE_UP; }
// 	if ( m_animTime >= 1.0f )						{ m_tileAnim = MOVE_DOWN; }
// 	if ( m_animTime >= 2.0f )						{ m_tileAnim = MOVE_LEFT; }
// 	if ( m_animTime >= 3.0f )						{ m_tileAnim = MOVE_RIGHT; }
// 	if( m_animTime >= 4.0f )						{ m_tileAnim = MOVE_CENTER; m_animTime = 0.f; }
// 	
// 	UpdateCollider();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::Render() const
{
	if ( m_owner->m_isDebugDraw  )
	{
		g_theRenderer->DrawUnfilledAABB2( m_bounds , WHITE , 5.f );
	}

	if( !m_isVisible )
	{
		return;
	}
	
	const Texture* tex = &g_theGame->m_gameSS[ SS_BRICKS ]->GetTexture();
	g_theRenderer->BindTexture( tex );
	g_theRenderer->DrawAABB2( m_bounds , WHITE , m_tileDef->m_spriteUVs.m_mins , m_tileDef->m_spriteUVs.m_maxs );
	g_theRenderer->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Tile::GetTileIndex()
{
	IntVec2 mapDimensions = m_ownerMap->m_dimensions;

	return m_tileCoords.y * mapDimensions.x + m_tileCoords.x; 
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Tile::TileCollisionResponse( Ball* ball )
{
	if( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , m_bounds ) )
	{
		ball->m_color = m_tileColor;

		Vec2 refPoint = m_bounds.GetNearestPoint( ball->m_pos );
		Vec2 edgeNormal = ( ball->m_pos - refPoint ).GetNormalized();

		uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );
		
		SpawnParticlesOnBallCollision( ball , refPoint , numParticles );
		
		if ( !ball->m_brickThrough )
		{
			ball->m_velocity.Reflect( edgeNormal );

			SpawnParticlesOnBallCollision( ball , refPoint , numParticles );
			
			PushDiscOutOfAABB( ball->m_pos , ball->m_physicsRadius , m_bounds );
			ball->m_currentScore += ball->m_currentScoreMultiplier * PER_TILE_SCORE;
		}

		int powerUpSpawnType = g_RNG->RollRandomIntInRange( PT_INVALID + 1 , NUM_POWERUP_TYPES - 1 );
		PowerUpDefinition* powerupDef = PowerUpDefinition::GetPowerUpDefinitionForPowerUp( ( ePowerUpType ) powerUpSpawnType );
		float powerupSpawnProbablity = g_RNG->RollRandomFloatBetweenZerotoOne();

		if( nullptr != powerupDef )
		{
			if( powerupSpawnProbablity >= powerupDef->m_spawnProbablityRequirement )
			{
				m_ownerMap->SpawnNewEntity( POWERUP , refPoint , POWERUP_VELOCITY_MULTIPLIER * ball->m_velocity ,
				                            nullptr , ( ePowerUpType )powerUpSpawnType );
				m_owner->AddScreenShakeIntensity();
			}
		}

// 		PowerUps*  test = (PowerUps*) m_ownerMap->SpawnNewEntity( POWERUP , refPoint , POWERUP_VELOCITY_MULTIPLIER * ball->m_velocity ,
// 									nullptr , ( ePowerUpType ) PT_BRICK_ALL_VISIBLE );



		//float collisionScore = 0.f;
				
		float scrrenShake = g_RNG->RollRandomFloatBetweenZerotoOne();
		if ( scrrenShake >= m_tileDef->m_screenshakeMinProbablity )
		{
			m_owner->AddScreenShakeIntensity();
		}
		
		if ( !m_isSolid )
		{
			ball->m_currentScoreMultiplier++;
			ball->m_currentScore += ball->m_currentScoreMultiplier * PER_TILE_SCORE;
			--m_health;
		}
		
		if ( m_health <= m_solidAtHealth )
		{
			m_isSolid = true;
		}

		if ( m_health <= m_visibleAtHealth )
		{
			m_isVisible = true;
		}
		
		//ball->UpdateCurrentTexture( m_tileColor );
		ball->UpdateCurrentTexture( m_tileDef->m_ballSpriteCoords );

		if ( ball->m_brickThrough )
		{
			ball->m_currentScoreMultiplier++;
			ball->m_currentScore += ball->m_currentScoreMultiplier * PER_TILE_SCORE * ( float ) m_health * 0.631f;
			m_health = 0;
		}
		
		if ( m_health == 0 )
		{
			ball->m_currentScore += ball->m_currentScoreMultiplier * PER_TILE_SCORE * 1.25f;
			m_isGarbage = true;
			m_ownerMap->m_numAliveTiles--;
		}

		//ball->m_currentScore += collisionScore;
		
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::SpawnParticlesOnBallCollision( Ball* ball , Vec2 refPoint , uint num4XParticles )
{

	for ( uint index = 0 ; index < num4XParticles ; index++ )
	{
		float ballVelocityMagnitude = ball->m_velocity.GetLength();

		Vec2 deviation				= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		float orientationDegrees	= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		float angularVelocity		= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		float scale					= g_RNG->RollRandomFloatInRange(0.f,1.5f );		
		float maxAge				= g_RNG->RollRandomFloatInRange( VFX_PARTICLE_MIN_AGE , VFX_PARTICLE_MAX_AGE );
		
		m_ownerMap->m_tileEmitter->SpawnNewParticle( AABB2( -VFX_PARTICLE_DIMENSIONS , VFX_PARTICLE_DIMENSIONS ) , refPoint ,
		                                             orientationDegrees , scale , angularVelocity ,
		                                             ( ball->m_velocity + deviation ) * VFX_PARTICLE_VELOCITY , 0.0f ,
													 maxAge , WHITE , m_tileDef->m_VFXSpriteCoords );

		deviation				= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees		= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity			= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale					= g_RNG->RollRandomFloatInRange(0.f,1.5f );	
		maxAge					= g_RNG->RollRandomFloatInRange( VFX_PARTICLE_MIN_AGE , VFX_PARTICLE_MAX_AGE );
		
		m_ownerMap->m_tileEmitter->SpawnNewParticle( AABB2( -VFX_PARTICLE_DIMENSIONS , VFX_PARTICLE_DIMENSIONS ) , refPoint ,
													 orientationDegrees , scale , angularVelocity ,
													 ( ball->m_velocity + deviation ) * VFX_PARTICLE_VELOCITY , 0.0f ,
													 maxAge , WHITE , m_tileDef->m_VFXSpriteCoords );

		deviation				= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees		= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity			= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale					= g_RNG->RollRandomFloatInRange(0.f,1.5f );
		maxAge					= g_RNG->RollRandomFloatInRange( VFX_PARTICLE_MIN_AGE , VFX_PARTICLE_MAX_AGE );

		m_ownerMap->m_tileEmitter->SpawnNewParticle( AABB2( -VFX_PARTICLE_DIMENSIONS , VFX_PARTICLE_DIMENSIONS ) , refPoint ,
													 orientationDegrees , scale , angularVelocity ,
													 ( ball->m_velocity + deviation ) * VFX_PARTICLE_VELOCITY , 0.0f ,
													 maxAge , WHITE , m_tileDef->m_VFXSpriteCoords );

		deviation				= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees		= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity			= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale					= g_RNG->RollRandomFloatInRange(0.f,1.5f );	
		maxAge					= g_RNG->RollRandomFloatInRange( VFX_PARTICLE_MIN_AGE , VFX_PARTICLE_MAX_AGE );
		
		m_ownerMap->m_tileEmitter->SpawnNewParticle( AABB2( -VFX_PARTICLE_DIMENSIONS , VFX_PARTICLE_DIMENSIONS ) , refPoint ,
													 orientationDegrees , scale , angularVelocity ,
													 ( ball->m_velocity + deviation ) * VFX_PARTICLE_VELOCITY , 0.0f ,
													 maxAge , WHITE , m_tileDef->m_VFXSpriteCoords );
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------

AABB2 Tile::CreateColliderFromCoords() const
{
	//Vec2 offset(halfTileDimensions.x*2.f,halfTileDimensions.y);
	//AABB2 collider = AABB2( Vec2::ZERO , Vec2( TILE_LENGTH , TILE_HEIGHT ) );
	AABB2 collider = AABB2( -halfTileDimensions , halfTileDimensions );
	collider.SetCenter( Vec2( m_tileCoords ) );
	return collider;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::UpdateCollider()
{
	switch ( m_tileAnim )
	{
		case MOVE_UP :
							{
								Vec2 m_boundsDim = m_bounds.GetDimensions();
								m_boundsDim -= Vec2( 0.f , TILE_DEVIATION );
								//m_bounds.Translate( Vec2( 0.f , TILE_DEVIATION ) );
								m_bounds.SetDimensions( m_boundsDim );
							}break;
		case MOVE_DOWN :
							{
								Vec2 m_boundsDim = m_bounds.GetDimensions();
								m_boundsDim += Vec2( 0.f , TILE_DEVIATION );
								//m_bounds.Translate( Vec2( 0.f , -TILE_DEVIATION ) );
								m_bounds.SetDimensions( m_boundsDim );
							}break;
		case MOVE_LEFT :
							{
								Vec2 m_boundsDim = m_bounds.GetDimensions();
								m_boundsDim -= Vec2( TILE_DEVIATION , 0.f );
								//m_bounds.Translate( Vec2( -TILE_DEVIATION , 0.f ) );
								m_bounds.SetDimensions( m_boundsDim );
							}break;
		case MOVE_RIGHT :
							{
								Vec2 m_boundsDim = m_bounds.GetDimensions();
								m_boundsDim += Vec2( TILE_DEVIATION , 0.f );
								//m_bounds.Translate( Vec2( TILE_DEVIATION , 0.f ) );
								m_bounds.SetDimensions( m_boundsDim );
							}break;
		case MOVE_CENTER :
							{
								//m_bounds = CreateColliderFromCoords();
							}break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
