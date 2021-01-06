#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameMultiplayer.hpp"
#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

GameMultiplayer::GameMultiplayer()
{
	LOG_SYSMESSAGE( "Multiplayer Game Started" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameMultiplayer::~GameMultiplayer()
{
	delete m_playerCamera;
	m_playerCamera = nullptr;

	delete m_UICamera;
	m_UICamera = nullptr;
	
	LOG_SYSMESSAGE( "Multiplayer Game Terminated" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameMultiplayer::Startup()
{
	m_playerCamera = new Camera();
	m_playerCamera->SetOrthoView( Vec2::ZERO , Vec2( INTITAL_MAP_SIZE_X , INTITAL_MAP_SIZE_Y ) );
	m_playerCamera->SetClearMode( CLEAR_COLOR_BIT , BLACK );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameMultiplayer::Update( float deltaSeconds )
{
	Game::Update( deltaSeconds );
	
	for ( auto playerindex : m_world->m_Maps[ 0 ]->m_entityListsByType[ PLAYERTANK_ENTITY ] )
	{
		if( playerindex == nullptr )
		{
			playerindex = nullptr;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameMultiplayer::Render() const
{
	g_theRenderer->BeginCamera( *m_playerCamera );
	g_theRenderer->SetBlendMode( ALPHA );

	Game::Render();
	for( auto playerIndex : m_players )
	{
		if( playerIndex != nullptr )
		{
			playerIndex->Render();
		}
	}
	g_theRenderer->EndCamera( *m_playerCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	
Player* GameMultiplayer::CreateAndAddPlayerAtpositionAndOrientation( Vec2 pos /*= Vec2( 1.5f , 1.5f ) */ , float orientation /*= 45.f */ )
{
	Player* newPlayer = ( Player* ) m_world->m_currentMap->SpawnNewEntity( PLAYERTANK_ENTITY , FACTION_ALLY , pos , orientation );
	m_players.emplace_back( newPlayer );
	return newPlayer;
}

//--------------------------------------------------------------------------------------------------------------------------------------------