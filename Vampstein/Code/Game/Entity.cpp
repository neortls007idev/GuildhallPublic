#include "Engine/Core/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entity.hpp"
#include "Game/EntityDef.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::Entity( EntityDef const& entityDef , Map* map )
{
	m_owner						= map;
	m_canBePushedByWalls		= entityDef.m_pushedByWalls;
	m_canBePushedByEntities		= entityDef.m_pushedByEntities;
	m_canPushEntities			= entityDef.m_pushesEntities;
								
	m_physicsRadius				= entityDef.m_physicsRadius;
	m_height					= entityDef.m_height;
	m_eyeHeight					= entityDef.m_eyeHeight;
	m_spriteSize				= entityDef.m_spriteSize;
	m_mass						= entityDef.m_mass;
	m_damage					= entityDef.m_damage;
	m_speed						= entityDef.m_speed;
	m_billboardType				= entityDef.m_billboard;
	m_entityDef					= const_cast< EntityDef* >( &entityDef );

	m_canBePushedByWalls		= true;
	m_canBePushedByEntities		= true;
	m_canPushEntities			= true;

	m_entityforwardVector		= Vec2::ZERO_ONE;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::Entity()
{
	m_canBePushedByWalls				= true;
	m_canBePushedByEntities				= true;
	m_canPushEntities					= true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::~Entity()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::ConstructEntityBillboard()
{
	Transform transform;
	transform.SetPosition( m_pos );
	transform.SetRotation( 0.f , m_startYawDegrees , 0.f );
	Vec2 halfDimensions = m_spriteSize * 0.5f;
	Texture* tex = const_cast< Texture* >( &( m_entityDef->m_spriteSheet->GetTexture() ) );

	m_billBoard = new BillBoard( AABB2( -halfDimensions.x , 0.f , halfDimensions.x ,m_spriteSize.y ) , transform.GetAsMatrix() , m_pos ,
	                             Vec2::ZERO , Vec2::ONE , tex , m_billboardType );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Update( float deltaSeconds )
{
	m_time += deltaSeconds;
	
	if ( g_theGame->m_player == this )
	{
		return;
	}
	if( m_billBoard != nullptr )
	{
		m_billBoard->m_pos = m_pos;
		m_billBoard->Update();
		UpdateBillboardSprites();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Render() const
{
	if( g_theGame->m_player == this )
	{
		return;
	}
	
	if ( m_billBoard != nullptr )
	{
		m_billBoard->Render();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::UpdateBillboardSprites()
{
	Vec3 cameraPos			= g_theGame->m_gameCamera.GetCameraTransform().GetPostion();
	Vec2 dispVec			= ( cameraPos - m_pos ).GetXYComponents();
	Vec2 forwardVec			= ( m_entityforwardVector );
	Vec2 backVec			= -forwardVec;
	Vec2 rightVec			= forwardVec.GetRotated90Degrees();
	Vec2 leftVec			= forwardVec.GetRotatedMinus90Degrees();
	Vec2 forwardRightVec	= forwardVec.GetRotatedDegrees( 45.f );
	Vec2 forwardLeftVec		= forwardVec.GetRotatedDegrees( -45.f );
	Vec2 backRightVec		= forwardVec.GetRotatedDegrees( 135.f );
	Vec2 backLeftVec		= forwardVec.GetRotatedDegrees( -135.f );
	float dotProduct1		= DotProduct2D( forwardVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct2		= DotProduct2D( backVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct3		= DotProduct2D( rightVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct4		= DotProduct2D( leftVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct5		= DotProduct2D( forwardRightVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct6		= DotProduct2D( forwardLeftVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct7		= DotProduct2D( backRightVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );
	float dotProduct8		= DotProduct2D( backLeftVec , Vec2( dispVec.x , dispVec.y ).GetNormalized() );

	float min = dotProduct1;
	if( min < dotProduct2 )
	{
		min = dotProduct2;
	}
	if( min < dotProduct3 )
	{
		min = dotProduct3;
	}
	if( min < dotProduct4 )
	{
		min = dotProduct4;
	}
	if( min < dotProduct5 )
	{
		min = dotProduct5;
	}
	if( min < dotProduct6 )
	{
		min = dotProduct6;
	}
	if( min < dotProduct7 )
	{
		min = dotProduct7;
	}
	if( min < dotProduct8 )
	{
		min = dotProduct8;
	}
	if( min == dotProduct1 )
	{
		SpriteAnimDefinition* def = m_entityDef->m_entityAnims[ ENTITY_WALK_ANIM ][ "front" ];
		if( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_billBoard->m_minUVs , m_billBoard->m_maxUVs );
		}
	}
	if( min == dotProduct2 )
	{
		SpriteAnimDefinition* def = m_entityDef->m_entityAnims[ ENTITY_WALK_ANIM ][ "back" ];
		if( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_billBoard->m_minUVs , m_billBoard->m_maxUVs );
		}
	}
	if( min == dotProduct3 )
	{
		SpriteAnimDefinition* def = m_entityDef->m_entityAnims[ ENTITY_WALK_ANIM ][ "left" ];
		if( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_billBoard->m_minUVs , m_billBoard->m_maxUVs );
		}
	}
	if( min == dotProduct4 )
	{
		SpriteAnimDefinition* def = m_entityDef->m_entityAnims[ ENTITY_WALK_ANIM ][ "right" ];
		if( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_billBoard->m_minUVs , m_billBoard->m_maxUVs );
		}
	}
	if( min == dotProduct5 )
	{
		SpriteAnimDefinition* def = m_entityDef->m_entityAnims[ ENTITY_WALK_ANIM ][ "frontLeft" ];
		if( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_billBoard->m_minUVs , m_billBoard->m_maxUVs );
		}
	}
	if( min == dotProduct6 )
	{
		SpriteAnimDefinition* def = m_entityDef->m_entityAnims[ ENTITY_WALK_ANIM ][ "frontRight" ];
		if( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_billBoard->m_minUVs , m_billBoard->m_maxUVs );
		}
	}
	if( min == dotProduct7 )
	{
		SpriteAnimDefinition* def = m_entityDef->m_entityAnims[ ENTITY_WALK_ANIM ][ "backLeft" ];
		if( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_billBoard->m_minUVs , m_billBoard->m_maxUVs );
		}
	}
	if( min == dotProduct8 )
	{
		SpriteAnimDefinition* def = m_entityDef->m_entityAnims[ ENTITY_WALK_ANIM ][ "backRight" ];
		if( def != nullptr )
		{
			SpriteDefinition d = def->GetSpriteDefAtTime( m_time );
			d.GetUVs( m_billBoard->m_minUVs , m_billBoard->m_maxUVs );
		}
	}
}

bool Entity::IsPlayer()
{
	if ( m_entityType == PLAYER )
	{
		return true;
	}
	
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsProjectile()
{
	if( m_entityType == PROJECTILE )
	{
		return true;
	}
	
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsNPC()
{
	if( m_entityType == NPC )
	{
		return true;
	}
	
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsPortal()
{
	if( m_entityType == PORTAL )
	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------