#include "Engine/Core/Devconsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/Map.hpp"
#include "Game/World.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

GameSinglePlayer::GameSinglePlayer()
{
	LOG_SYSMESSAGE( "SinglePlayer Game Started" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameSinglePlayer::~GameSinglePlayer()
{
	delete m_playerCamera;
	m_playerCamera = nullptr;

	delete m_UICamera;
	m_UICamera = nullptr;

	LOG_SYSMESSAGE( "SinglePlayer Game Terminated" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameSinglePlayer::Startup()
{
	m_playerCamera = new Camera();
	m_playerCamera->SetOrthoView( Vec2::ZERO , Vec2( INTITAL_MAP_SIZE_X , INTITAL_MAP_SIZE_Y ) );
	m_playerCamera->SetClearMode( CLEAR_COLOR_BIT , BLACK );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameSinglePlayer::Update( float deltaSeconds )
{
	Game::Update( deltaSeconds );
	if( m_world->m_Maps[0]->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] == nullptr )
	{
		m_player = nullptr;
	}
		
	//if( m_player != nullptr )
	//{
	//	m_player->Update( deltaSeconds );
	//}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameSinglePlayer::Render() const
{
	g_theRenderer->BeginCamera( *m_playerCamera );
	g_theRenderer->SetBlendMode( ALPHA );
	if( m_player != nullptr )
	{
		Game::Render();
		m_player->Render();
	}
	g_theRenderer->EndCamera( *m_playerCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameSinglePlayer::CreatePlayer()
{
	m_player = ( Player* ) m_world->m_currentMap->SpawnNewEntity( PLAYERTANK_ENTITY , FACTION_ALLY , Vec2( 1.5f , 1.5f ) , 45.f );
	//m_player = new Player( ( Game* ) this , Vec2( 1.5f , 1.5f ) , Vec2::ZERO , 45.f , PLAYERTANK_ENTITY , FACTION_ALLY );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
