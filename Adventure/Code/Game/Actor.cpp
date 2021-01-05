#include "Game/Actor.hpp"
#include "Game/TheApp.hpp"
#include "Game/ActorDefs.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/VertexUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

Actor::Actor( Game* theGame , Vec2 position , float orientation , ActorDefinition* definition ) :
	   Entity( theGame , position , orientation )
{
//ASSUMING CURRENTLY ONLY PLAYER ACTOR EXISTS
	
	m_actorDef = definition;

	m_velocity = definition->m_defaultVelocity;
	m_position = definition->m_startPosition;
	m_health = 1;
	m_isDead = definition->m_isDead;
	m_isGarbage = definition->m_isGarbage;

	m_physicsRadius = definition->m_physicsRadius;
	m_entityBounds = definition->m_localDrawBounds;

	m_canWalk = definition->m_canWalk;
	m_canSwim = definition->m_canSwim;
	m_canFly = definition->m_canFly;

	m_actorVerts[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , WHITE , definition->m_spriteUVs.m_mins );
	m_actorVerts[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f )  , WHITE , Vec2( definition->m_spriteUVs.m_maxs.x , definition->m_spriteUVs.m_mins.y ) );
	m_actorVerts[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f )  , WHITE , Vec2( definition->m_spriteUVs.m_mins.x , definition->m_spriteUVs.m_maxs.y ) );
	m_actorVerts[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f )  , WHITE , Vec2( definition->m_spriteUVs.m_maxs.x , definition->m_spriteUVs.m_mins.y ) );
	m_actorVerts[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f )   , WHITE , definition->m_spriteUVs.m_maxs );
	m_actorVerts[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f )  , WHITE , Vec2( definition->m_spriteUVs.m_mins.x , definition->m_spriteUVs.m_maxs.y ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Actor::Update( float deltaSeconds )
{
	UpdateFromJoystick( deltaSeconds );
	m_timeElapsed += deltaSeconds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Actor::Render() const
{
	SpriteAnimDefinition* anims = m_actorDef->m_animSet->GetDefinition( "Idle" );
	if ( m_orientationDegrees == 0.f )
	{
		anims = m_actorDef->m_animSet->GetDefinition( "Idle" );
	}
	else if ( m_orientationDegrees >= 70.f && m_orientationDegrees <= 110.f )
	{
		anims = m_actorDef->m_animSet->GetDefinition( "MoveNorth" );
	}
	else if ( m_orientationDegrees >= 90.f && m_orientationDegrees <= 180.f )
	{
		anims = m_actorDef->m_animSet->GetDefinition( "MoveWest" );
	}
	else if ( m_orientationDegrees >= 0.f && m_orientationDegrees <= 90.f )
	{
		anims = m_actorDef->m_animSet->GetDefinition( "MoveEast" );
	}

	const SpriteDefinition& currentSpriteDef = anims->GetSpriteDefAtTime( m_timeElapsed );
	Vec2 animMinUVs = Vec2::ZERO;
	Vec2 animMaxUVs = Vec2::ZERO;
	currentSpriteDef.GetUVs( animMinUVs , animMaxUVs );
	
	Vertex_PCU tempVerts[ 6 ];
	
	for ( int index = 0; index < 6; index++ )
	{
		tempVerts[ index ] = m_actorVerts[ index ];
	}

 	tempVerts[ 0 ].m_uvTexCoords = animMinUVs;
	tempVerts[ 1 ].m_uvTexCoords = Vec2( animMaxUVs.x , animMinUVs.y );
	tempVerts[ 2 ].m_uvTexCoords = Vec2( animMinUVs.x , animMaxUVs.y );
	tempVerts[ 3 ].m_uvTexCoords = Vec2( animMaxUVs.x , animMinUVs.y );
	tempVerts[ 4 ].m_uvTexCoords = animMaxUVs;
	tempVerts[ 5 ].m_uvTexCoords = Vec2( animMinUVs.x , animMaxUVs.y );
	
	TransformVertexArray2D( 6 , tempVerts , 1.f , 0.f , m_position );
	g_theRenderer->BindTexture( m_theGame->m_textures[ TEXTURE_CHARACTERS ] );
	g_theRenderer->DrawVertexArray( 6 , tempVerts );
	g_theRenderer->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Actor::IsDead()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Actor::UpdateFromJoystick( float deltaSeconds )
{
	m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
	// Do nothing if this player ship has no controller assigned to it
	if ( m_controllerID < 0 )
	{
		return;
	}

	const XboxController& controller = g_theInput->GetXboxController( m_controllerID );
	if ( !controller.IsConnected() )
	{
		return;
	}

	if ( g_theApp->m_isPaused == false )
	{
		XboxButtonPresses( controller , deltaSeconds );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Actor::XboxButtonPresses( const XboxController& controller , float deltaSeconds )
{
	const AnalogJoystick& leftStick = controller.GetLeftJoystick();
	float leftStickMagnitude = leftStick.GetMagnitude();
	if ( leftStickMagnitude > 0.f ) // This is already deadzone-corrected; so anything non-zero means “significant movement” (outside the inner dead zone)
	{
		m_orientationDegrees = leftStick.GetAngleDegrees();
		Entity::MoveEntity( deltaSeconds );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Strings Actor::GetActorDetails()
{
	Strings actorDetails;
	std::string trueTextForBoolean = "true";
	std::string FalseTextForBoolean = "false";
	if ( m_actorDef->m_canFly )
	{
		actorDetails.push_back( "canFly: " + trueTextForBoolean );
	}
	else
	{
		actorDetails.push_back( "canFly: " + FalseTextForBoolean );
	}
	if ( m_actorDef->m_canSwim )
	{
		actorDetails.push_back( "canSwim: " + trueTextForBoolean );
	}
	else
	{
		actorDetails.push_back( "canSwim: " + FalseTextForBoolean );
	}
	if ( m_actorDef->m_canWalk )
	{
		actorDetails.push_back( "canWalk: " + trueTextForBoolean );
	}
	else
	{
		actorDetails.push_back( "canWalk: " + FalseTextForBoolean );
	}
	actorDetails.push_back( m_actorDef->m_name );
	return actorDetails;
}

//--------------------------------------------------------------------------------------------------------------------------------------------