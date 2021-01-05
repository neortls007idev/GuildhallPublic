#include "Game/Boulder.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/TheApp.hpp"
#include "TileDefinition.hpp"

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

Boulder::Boulder( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction ) :
	Entity( CurrentGameInstance ,
		position ,
		velocity ,
		orientation ,
		type ,
		faction )
{
		InitializeBoulderMemberVariables();
		m_boulderTextureSprite = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Extras_4x4.png" );
		m_boulderSpriteSheet = new SpriteSheet( *m_boulderTextureSprite , IntVec2( 4 , 4 ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Boulder::~Boulder()
{
	delete m_boulderTextureSprite;
	m_boulderTextureSprite = nullptr;
	delete m_boulderSpriteSheet;
	m_boulderSpriteSheet = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Boulder::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Boulder::Render() const
{
	const SpriteDefinition& boulderSprite = m_boulderSpriteSheet->GetSpriteDefinition( 3 );
	Vec2 minUVs;
	Vec2 maxUVs;
	boulderSprite.GetUVs( minUVs , maxUVs );
		
	Vertex_PCU tempBoulder[ 6 ];
	for ( int index = 0; index < 6; index++ )
	{
		tempBoulder[ index ] = m_boulderVerts[ index ];
	}
	tempBoulder[ 0 ].m_uvTexCoords = minUVs;
	tempBoulder[ 1 ].m_uvTexCoords = Vec2( maxUVs.x , minUVs.y );
	tempBoulder[ 2 ].m_uvTexCoords = Vec2( minUVs.x , maxUVs.y );
	tempBoulder[ 3 ].m_uvTexCoords = Vec2( maxUVs.x , minUVs.y ); 
	tempBoulder[ 4 ].m_uvTexCoords = maxUVs;
	tempBoulder[ 5 ].m_uvTexCoords = Vec2( minUVs.x , maxUVs.y );
	g_theRenderer->BindTexture( m_boulderTextureSprite );
	TransformVertexArray2D( 6 , tempBoulder , 1.f , m_orientationDegrees , m_position );
	g_theRenderer->DrawVertexArray( 6 , tempBoulder );
	g_theRenderer->BindTexture( nullptr );

	//--------------------------------------------------------------------------------------------------------------------------------------------

	if ( g_theGame->m_debugDraw )
	{
		g_theRenderer->DrawRing( m_position , m_cosmeticRadius , m_cosmeticRing , 0.02f );
		g_theRenderer->DrawRing( m_position , m_physicsRadius , m_physicsRing , 0.02f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Boulder::IsDead()
{
	m_isDead = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Boulder::InitializeBoulderMemberVariables()
{
	
	m_velocity = Vec2( 0.25f , 0.25f );
	m_health = 1;

	m_isDead = false;
	m_isGarbage = false;
	m_physicsRadius = 0.4f;
	m_cosmeticRadius = 0.4f;
	m_age = 0.f;

	m_isPushedByEntities = true;
	m_pushesEntiites = false;
	m_isHitByBullets = false;
	m_isPushedByWalls = true;

}

//--------------------------------------------------------------------------------------------------------------------------------------------