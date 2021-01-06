#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ParticleSystem/ParticleEmitter2D.hpp"
#include "Engine/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Time.hpp"
#include "Game//Game.hpp"
#include "Game/Ball.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Paddle.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*				g_theRenderer;
extern AudioSystem*					g_theAudioSystem;
extern ParticleSystem2D*			g_theParticleSystem2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( Game* owner ) :
								m_owner( owner ) 
{
	InitializeLevelBounds();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( Game* owner , MapDefinition* mapDefinition , std::string mapName ) :
																				m_owner( owner ) ,
																				m_name( mapName ) ,
																				m_mapDefinition( mapDefinition )
{
	m_dimensions.x = m_mapDefinition->m_width;
	m_dimensions.y = m_mapDefinition->m_height;

	InitializeLevelBounds();

	for( size_t mapGenSteps = 0; mapGenSteps < m_mapDefinition->m_generationSteps.size(); mapGenSteps++ )
	{
		m_mapDefinition->m_generationSteps[ mapGenSteps ]->RunStep( *this );
	}

	m_tileEmitter = g_theParticleSystem2D->CreateNewParticleEmitter( g_theRenderer , m_owner->m_gameSS[ SS_VFX_FLARE ] ,
	                                                                 10000 , nullptr , ADDITIVE );
	m_boundsEmitter = g_theParticleSystem2D->CreateNewParticleEmitter( g_theRenderer , m_owner->m_gameSS[ SS_VFX_LEAVES ] ,
																	   2500 , nullptr , ADDITIVE );
	m_paddleEmitter = g_theParticleSystem2D->CreateNewParticleEmitter( g_theRenderer , m_owner->m_gameSS[ SS_VFX_FLOWERS ] ,
																	   2000 , nullptr , ADDITIVE );

	SpawnNewEntity( PADDLE , Vec2::ZERO );

	Paddle* thePaddle = ( Paddle* ) m_entityListsByType[ PADDLE ][ 0 ];
	AABB2	paddleBounds = thePaddle->GetCollider();
	
	SpawnNewEntity( BALL , paddleBounds.GetCenter() + Vec2::ZERO_ONE * 37.5f , BALL_INITIAL_VELOCITY );
	m_numAliveBalls++;
	
	ResolveBallvPaddleCollisions();
	
	m_backgroundIndex = g_RNG->RollRandomIntInRange( TEX_BACKGROUND_FOREST_1 , TEX_BACKGROUND_AURORA_1 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::InitializeLevelBounds()
{
	Camera* gameCamera					= m_owner->GetWorldCamera();
	Vec2 cameraMins						= gameCamera->GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs						= gameCamera->GetOrthoMax().GetXYComponents();

	AABB2 cameraArea					= AABB2( cameraMins , cameraMaxs );
	Vec2 cameraDimensions				= cameraArea.GetDimensions(); 

	float sideWallOffset				= LEVEL_SIDEWALL_PERCENTAGE * 0.5f * cameraDimensions.x;
	float topWallOffset					= 0.125f * 0.75f * cameraDimensions.y;
	float pitOffset						= 0.1f * cameraDimensions.y * 0.5f;
	float additionalHeight				= cameraDimensions.y * 0.05f;
	float additionalWidth				= cameraDimensions.x * 0.05f;
	
	m_backGround						= cameraArea;
	
	m_leftWallCosmeticBounds			= AABB2( cameraMins.x , cameraMins.y - additionalHeight , cameraMins.x + sideWallOffset ,
												 cameraMaxs.y + additionalHeight );
	m_rightWallCosmeticBounds			= AABB2( cameraMaxs.x - sideWallOffset , cameraMins.y - additionalHeight , cameraMaxs.x ,
												 cameraMaxs.y + additionalHeight );
	m_topWallCosmeticBounds				= AABB2( cameraMins.x - additionalWidth , cameraMaxs.y - topWallOffset ,
												 cameraMaxs.x + additionalWidth , cameraMaxs.y + 50.f );
	m_pitCosmeticBounds					= AABB2( cameraMins.x * 10.f , cameraMins.y ,
												 cameraMaxs.x * 10.f, cameraMins.y + pitOffset );

	m_leftWallPhysicalBounds			= m_leftWallCosmeticBounds.GetBoxAtLeft( 0.85f , 0.f );
	m_rightWallPhysicalBounds			= m_rightWallCosmeticBounds.GetBoxAtRight( 0.85f , 0.f );
	m_topWallPhysicalBounds				= m_topWallCosmeticBounds.GetBoxAtTop( 0.15f , 0.f );
	m_pitPhysicalBounds					= m_pitCosmeticBounds.GetBoxAtBottom( 0.9f , 0.f );
	m_leftWallPhysicalBounds.m_mins.x	*= 5000.f;
	m_rightWallPhysicalBounds.m_maxs.x	*= 5000.f;
	m_topWallPhysicalBounds.m_maxs.y	*= 5000.f;
	m_pitPhysicalBounds.m_mins.y		*= 10000.f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::~Map()
{
	g_theParticleSystem2D->DestroyParticleEmitter( m_tileEmitter );
	m_tileEmitter = nullptr;

	g_theParticleSystem2D->DestroyParticleEmitter( m_boundsEmitter );
	m_boundsEmitter = nullptr;

	g_theParticleSystem2D->DestroyParticleEmitter( m_paddleEmitter );
	m_paddleEmitter = nullptr;
	
	m_mapDefinition = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Update( float deltaSeconds )
{
	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES; Entitytype++ )
	{
		Entitylist& currentList = m_entityListsByType[ Entitytype ];
		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
		{
			if ( currentList[ entityIndex ] != nullptr )
			{
				currentList[ entityIndex ]->Update( deltaSeconds );
			}

		}
	}
	ResolveCollisions();

	g_theRenderer->UpdateFrameTime( deltaSeconds );
	UpdateSideCosmeticBounds();
	UpdateTopCosmeticBounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Render()
{
	g_theRenderer->BindTexture( m_owner->m_gameTex[ m_backgroundIndex ] );
	g_theRenderer->DrawAABB2( m_backGround , WHITE );

	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES; Entitytype++ )
	{
		Entitylist& currentList = m_entityListsByType[ Entitytype ];
		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
		{
			if ( nullptr != currentList[ entityIndex ] && !currentList[ entityIndex ]->m_isGarbage )
			{
				currentList[ entityIndex ]->Render();
			}
		}
	}
	
	RenderLevelBounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::UpdateSideCosmeticBounds()
{
	float positionY = RangeMapFloat( -1.f , 1.f , -50.f , 50.f ,
								  SinDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );

	Vec2 leftWallCenter = m_leftWallCosmeticBounds.GetCenter();
	m_leftWallCosmeticBounds.SetCenter( Vec2( leftWallCenter.x , positionY ) );

	Vec2 rightWallCenter = m_rightWallCosmeticBounds.GetCenter();
	m_rightWallCosmeticBounds.SetCenter( Vec2( rightWallCenter.x , positionY ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::UpdateTopCosmeticBounds()
{
	float positionX = RangeMapFloat( -1.f , 1.f , -50.f , 50.f ,
								  SinDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );
	Vec2 movement = Vec2( CosDegrees( 300.f * ( float ) GetCurrentTimeSeconds() ) , 0.0f );
	Vec2 topWallCenter = m_topWallCosmeticBounds.GetCenter();
	m_topWallCosmeticBounds.SetCenter( Vec2( positionX , topWallCenter.y ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::RenderLevelBounds()
{
	RenderLevelSideBounds();
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_TOP_WALL_SECTION ] );
	g_theRenderer->DrawAABB2( m_topWallCosmeticBounds , WHITE );
	
	if( m_owner->m_isDebugDraw )
	{
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->DrawUnfilledAABB2( m_leftWallPhysicalBounds	, CYAN , 5.f );
		g_theRenderer->DrawUnfilledAABB2( m_rightWallPhysicalBounds , CYAN , 5.f );
		g_theRenderer->DrawUnfilledAABB2( m_topWallPhysicalBounds	, CYAN , 5.f );
		g_theRenderer->DrawUnfilledAABB2( m_pitPhysicalBounds		, CYAN , 5.f );

		g_theRenderer->DrawUnfilledAABB2( m_leftWallCosmeticBounds	, MAGENTA );
		g_theRenderer->DrawUnfilledAABB2( m_rightWallCosmeticBounds , MAGENTA );
		g_theRenderer->DrawUnfilledAABB2( m_topWallCosmeticBounds	, MAGENTA );
		g_theRenderer->DrawUnfilledAABB2( m_pitCosmeticBounds		, MAGENTA );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::RenderLevelSideBounds() const
{
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_LEFT_WALL ] );
	g_theRenderer->DrawAABB2( m_leftWallCosmeticBounds , WHITE );
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_RIGHT_WALL ] );
	g_theRenderer->DrawAABB2( m_rightWallCosmeticBounds , WHITE );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity* Map::SpawnNewEntity ( eEntityType type , const Vec2& position , const Vec2& velocity /*= Vec2::ZERO */ ,
                           TileDefinition* tileDef /*= nullptr */ , ePowerUpType powerUpType /*= PT_INVALID */ )
{
	Entity* newEntity = nullptr;

	switch ( type )
	{
		case INVALID_ENTITY:
			break;
		case PADDLE:
			newEntity = new Paddle( m_owner , m_owner->GetPaddleHealth() ,
			AABB2( -100.f , -25.f , 100.f , 25.f ) ,
			Vec2( 0.f , m_pitPhysicalBounds.m_maxs.y + 83.f ) );
			break;
		case BALL:
			if ( m_numAliveBalls >= 8 )
			{
				return nullptr;
			}
			newEntity = new Ball( m_owner , 1 , BALL_DEFAULT_RADIUS , position , velocity , m_owner->m_ballDefaultSpriteCoords );
			m_numAliveBalls++;
			break;
		case TILE:
			newEntity = new Tile( this , IntVec2( position ) , tileDef );
			break;
		case POWERUP:
			newEntity = new PowerUps( m_owner , position , velocity , powerUpType );
			break;
		default:
			break;
	}

	AddEntityToMap( newEntity );
	return newEntity;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToMap( Entity* entity )
{
	Entitylist& currentList = m_entityListsByType[ entity->GetEntityType() ];
	AddEntityToList( currentList , entity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToList( Entitylist& entityList , Entity* entity )
{
	EmplaceBackAtEmptySpace( entityList , entity );	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::UpdateBallPosWhenGrabbed( float newPosX )
{
	Entitylist& ballList = m_entityListsByType[ BALL ];
	for( size_t index = 0 ; index < ballList.size() ; index++ )
	{
		if( ballList[ index ] != nullptr && m_owner->m_isBallLaunchable )
		{
			Ball* ball = ( Ball* ) ballList[ index ];
			ball->m_pos.x = newPosX;
			ball->m_cosmeticRadius.SetCenter( ball->m_pos );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveCollisions()
{
	ResolveBallvBoundsCollisions();
	ResolveBallvPaddleCollisions();
	ResolveBallvTileCollisions();

	ResolvePowerUpvBoundsCollisions();
	ResolvePaddlevPowerUpCollisions();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvBoundsCollisions()
{
	std::vector<Vertex_PCU> particleVerts;
	std::vector<uint>		particleIndices;
		
	Entitylist& currentList = m_entityListsByType[ BALL ];
	for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ BALL ].size(); entityIndex++ )
	{
		if ( nullptr != currentList[ entityIndex ] )
		{
			Ball* ball = ( Ball* ) currentList[ entityIndex ];
			
			ResolveBallvSingleSideBoundCollision( ball , m_leftWallPhysicalBounds );
			ResolveBallvSingleSideBoundCollision( ball , m_rightWallPhysicalBounds );
			ResolveBallvSingleSideBoundCollision( ball , m_topWallPhysicalBounds );
			
			ball->m_pos.x = Clamp( ball->m_pos.x , m_leftWallPhysicalBounds.m_maxs.x + ball->m_physicsRadius , m_rightWallPhysicalBounds.m_mins.x - ball->m_physicsRadius );
			ball->m_pos.y = Clamp( ball->m_pos.y , m_pitPhysicalBounds.m_mins.y , m_topWallPhysicalBounds.m_mins.y - ball->m_physicsRadius );
			
			if( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , m_pitPhysicalBounds ) )
			{
				m_owner->m_highScore += ball->m_currentScore;
				ball->m_isGarbage = true;
				--m_numAliveBalls;

				if ( m_numAliveBalls <= 1 && m_owner->GetPaddleHealth() > 0 )
				{
					m_owner->m_playerHealth = --m_owner->m_playerHealth;
					m_owner->m_isBallLaunchable = true;
					Paddle* thePaddle = nullptr;
							thePaddle = ( Paddle* ) GetFirstValidEntryInList( PADDLE );
					AABB2	paddleBounds = thePaddle->GetCollider();

					if ( m_owner->GetPaddleHealth() > 0 )
					{
						SpawnNewEntity( BALL , paddleBounds.GetCenter() + Vec2::ZERO_ONE * 37.5f , BALL_INITIAL_VELOCITY );
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvSingleSideBoundCollision( Ball* ball , AABB2 bounds )
{
	if( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , bounds ) )
	{
		Vec2 refPoint = bounds.GetNearestPoint( ball->m_pos );

		Vec2 edgeNormal = ( ball->m_pos - refPoint ).GetNormalized();
		
		ball->m_velocity.Reflect( edgeNormal );

		PushDiscOutOfAABB( ball->m_pos , ball->m_physicsRadius , bounds );

		ball->m_currentScore *= WORLDBOUNDS_SCORE_ABLATION;
		
		uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );
		SpawnParticlesOnBallCollisionUsingEmitter( ball , ball->m_pos , numParticles , m_boundsEmitter ,
												   LEAVES_PARTICLE_MIN_AGE , LEAVES_PARTICLE_MAX_AGE ,
												   LEAVES_PARTICLE_DIMENSIONS , LEAVES_PARTICLE_VELOCITY );

		g_theAudioSystem->PlaySound( m_owner->GetSFX( SFX_LEAVES_RUSTLE ) , false , m_owner->m_sfxVol );
		m_owner->AddScreenShakeIntensity();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvSingleSideBoundCollision( PowerUps* powerUps , AABB2 bounds )
{
	if( DoDiscAndAABBOverlap( powerUps->m_pos , powerUps->m_physicsRadius , bounds ) )
	{
		Vec2 refPoint = bounds.GetNearestPoint( powerUps->m_pos );

		Vec2 edgeNormal = ( powerUps->m_pos - refPoint ).GetNormalized();

		powerUps->m_velocity.Reflect( edgeNormal );

		PushDiscOutOfAABB( powerUps->m_pos , powerUps->m_physicsRadius , bounds );

		//uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );
		//SpawnParticlesOnBallCollisionUsingEmitter( ball , ball->m_pos , numParticles , m_boundsEmitter ,
		//										   LEAVES_PARTICLE_MIN_AGE , LEAVES_PARTICLE_MAX_AGE ,
		//										   LEAVES_PARTICLE_DIMENSIONS , LEAVES_PARTICLE_VELOCITY );

		g_theAudioSystem->PlaySound( m_owner->GetSFX( SFX_LEAVES_RUSTLE ) , false , m_owner->m_sfxVol );
		m_owner->AddScreenShakeIntensity();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvPaddleCollisions()
{
	Entitylist& currentList = m_entityListsByType[ BALL ];
	for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ BALL ].size(); entityIndex++ )
	{
		if ( currentList[ entityIndex ] )
		{
			Ball* ball = ( Ball* ) currentList[ entityIndex ];
			
			Paddle* paddle = nullptr;
			if ( m_entityListsByType[ PADDLE ].data() )
			{
				paddle = ( Paddle* ) GetFirstValidEntryInList( PADDLE );
			}
			
			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , paddle->GetCollider() ) && !ball->m_isInCooldown )
			{
				if ( !m_owner->m_isBallLaunchable )
				{
					Vec2 refPoint = paddle->GetCollider().GetNearestPoint( ball->m_pos );
					
					Vec2 edgeNormal = ( ball->m_pos - refPoint ).GetNormalized();

					float deviationFactor = RangeMapFloat( paddle->GetCollider().m_mins.x , paddle->GetCollider().m_maxs.x ,
					                                  -PADDLE_COLLISION_DEVIATION , PADDLE_COLLISION_DEVIATION , refPoint.x );

					uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );

					SpawnParticlesOnBallCollisionUsingEmitter( ball , refPoint , numParticles , m_paddleEmitter ,
					                                           FLOWER_PARTICLE_MIN_AGE , FLOWER_PARTICLE_MAX_AGE ,
					                                           FLOWER_PARTICLE_DIMENSIONS , FLOWER_PARTICLE_VELOCITY );

					m_owner->m_highScore += ball->m_currentScore;
					ball->m_currentScore = 0.f;
					ball->m_currentScoreMultiplier = 0;
					
					ball->m_velocity.Reflect( edgeNormal );
					
					float magnitude = ball->m_velocity.GetLength();
						  magnitude *= 1.01f;
						  magnitude = Clamp( magnitude , BALL_INITIAL_VELOCITY_MAGNITUDE , MAX_BALL_VELOCITY_MAGNITUDE );
					float angleDegrees = ball->m_velocity.GetAngleDegrees() - deviationFactor;
					
					ball->m_velocity = Vec2::MakeFromPolarDegrees( angleDegrees , magnitude );

					ball->m_isInCooldown = true;
				}
				
				PushDiscOutOfAABB( ball->m_pos , ball->m_physicsRadius , paddle->GetCollider() );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvTileCollisions()
{
	Entitylist& currentList = m_entityListsByType[ BALL ];
	for( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ BALL ].size(); entityIndex++ )
	{
		if( nullptr != currentList[ entityIndex ] )
		{
			Ball* ball = ( Ball* ) currentList[ entityIndex ];
			Tile* currentTile = nullptr;

			for( size_t tileIndex = 0 ; tileIndex < m_entityListsByType[ TILE ].size() ; tileIndex++ )
			{
				currentTile = ( Tile* ) m_entityListsByType[ TILE ][ tileIndex ];
				if ( nullptr != currentTile )
				{
					if ( currentTile->m_isGarbage == true )
					{
						continue;
					}
					
					if ( currentTile->TileCollisionResponse( ball ) )
					{
						g_theAudioSystem->PlaySound( m_owner->GetSFX( SFX_GLASS_BREAK_2 ) , false , m_owner->m_sfxVol );
					}

					if( currentTile->GetHealth() <= 0 )
					{
						m_entityListsByType[ TILE ][ tileIndex ]->m_isGarbage = true;
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolvePaddlevPowerUpCollisions()
{
	Entitylist& currentList = m_entityListsByType[ POWERUP ];
	for( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ POWERUP ].size(); entityIndex++ )
	{
		if( currentList[ entityIndex ] )
		{
			Paddle* paddle = nullptr;

			if( m_entityListsByType[ PADDLE ].data() )
			{
				paddle = ( Paddle* ) GetFirstValidEntryInList( PADDLE );
			}
			
			PowerUps* powerUp = ( PowerUps* ) currentList[ entityIndex ];
			
			if( DoDiscAndAABBOverlap( powerUp->m_pos , powerUp->m_physicsRadius , paddle->GetCollider() ) )
			{
					
//					uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );
//
// 					SpawnParticlesOnBallCollisionUsingEmitter( ball , refPoint , numParticles , m_paddleEmitter ,
// 															   FLOWER_PARTICLE_MIN_AGE , FLOWER_PARTICLE_MAX_AGE ,
// 															   FLOWER_PARTICLE_DIMENSIONS , FLOWER_PARTICLE_VELOCITY );
				powerUp->PowerUpEffects();
				powerUp->m_isGarbage = true;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolvePowerUpvBoundsCollisions()
{
	Entitylist& currentList = m_entityListsByType[ POWERUP ];
	for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ POWERUP ].size(); entityIndex++ )
	{
		if ( currentList[ entityIndex ] )
		{
			PowerUps* powerup = ( PowerUps* ) currentList[ entityIndex ];

			ResolveBallvSingleSideBoundCollision( powerup , m_leftWallPhysicalBounds );
			ResolveBallvSingleSideBoundCollision( powerup , m_rightWallPhysicalBounds );
			ResolveBallvSingleSideBoundCollision( powerup , m_topWallPhysicalBounds );
			
			if( DoDiscAndAABBOverlap( powerup->m_pos , powerup->m_physicsRadius , m_pitPhysicalBounds ) )
			{
				powerup->m_isGarbage = true;

				/*
				if ( m_numAliveBalls <= 0 && m_owner->GetPaddleHealth() > 0 )
				{
					m_owner->m_playerHealth = --m_owner->m_playerHealth;
					m_owner->m_isBallLaunchable = true;
					Paddle* thePaddle = ( Paddle* ) GetFirstValidEntryInList( PADDLE );
					AABB2	paddleBounds = thePaddle->GetCollider();

					if ( m_owner->GetPaddleHealth() > 0 )
					{
						SpawnNewEntity( BALL , paddleBounds.GetCenter() + Vec2::ZERO_ONE * 37.5f , BALL_INITIAL_VELOCITY );
						m_numAliveBalls++;
					}
				}
				*/
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnParticlesOnBallCollisionUsingEmitter ( Ball* ball , Vec2 refPoint , uint num4XParticles ,
                                                      ParticleEmitter2D* emitter , float minLifeTime ,
                                                      float maxLifeTime , Vec2 dimensions , float velocityMultiplier )
{
	for( uint index = 0 ; index < num4XParticles ; index++ )
	{
		SpawnRandomParticleUsingEmitter( ball , refPoint , minLifeTime , maxLifeTime , dimensions , velocityMultiplier , emitter );
		SpawnRandomParticleUsingEmitter( ball , refPoint , minLifeTime , maxLifeTime , dimensions , velocityMultiplier , emitter );
		SpawnRandomParticleUsingEmitter( ball , refPoint , minLifeTime , maxLifeTime , dimensions , velocityMultiplier , emitter );
		SpawnRandomParticleUsingEmitter( ball , refPoint , minLifeTime , maxLifeTime , dimensions , velocityMultiplier , emitter );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnRandomParticleUsingEmitter ( Ball* ball , Vec2 refPoint , float minLifeTime , float maxLifeTime ,
                                            Vec2 dimensions , float velocityMultiplier , ParticleEmitter2D* emmiter )
{
	float ballVelocityMagnitude = ball->m_velocity.GetLength();

	Vec2 deviation				= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
	float orientationDegrees	= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
	float angularVelocity		= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
	float scale					= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
	float maxAge				= g_RNG->RollRandomFloatInRange( minLifeTime , maxLifeTime );

	emmiter->SpawnNewRandomParticleFromSpriteSheet( AABB2( Vec2::ZERO , dimensions ) , refPoint ,
															orientationDegrees , scale , angularVelocity ,
															( ball->m_velocity + deviation ) * velocityMultiplier , 0.0f ,
															maxAge , WHITE );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity* Map::GetFirstValidEntryInList( eEntityType type )
{
	for( size_t index = 0 ; index < m_entityListsByType[ type ].size() ; index++ )
	{
		if ( nullptr != m_entityListsByType[ type ][ index ] )
		{
			return m_entityListsByType[ type ][ index ];
		}
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entitylist* Map::GetEntityList( eEntityType type )
{
	return &m_entityListsByType[ type ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::GarbageCollection()
{
	for( size_t type = 0 ; type < NUM_ENTITY_TYPES ; type++ )
	{
		for( size_t index = 0 ; index < m_entityListsByType[ type ].size() ; index++ )
		{
			if( nullptr != m_entityListsByType[ type ][ index ] && m_entityListsByType[ type ][ index ]->m_isGarbage )
			{
				delete m_entityListsByType[ type ][ index ];
				m_entityListsByType[ type ][ index ] = nullptr;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::DestroyEmitters()
{
	g_theParticleSystem2D->DestroyParticleEmitter( m_boundsEmitter );
	g_theParticleSystem2D->DestroyParticleEmitter( m_tileEmitter );
	g_theParticleSystem2D->DestroyParticleEmitter( m_paddleEmitter );
}

//--------------------------------------------------------------------------------------------------------------------------------------------