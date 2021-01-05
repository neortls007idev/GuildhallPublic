#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/ActorDefs.hpp"

#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern BitmapFont* g_bitmapFont;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

SpriteSheet* g_tileSpriteSheet		= nullptr;
SpriteSheet* g_characterSpriteSheet = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	//m_font = g_theRenderer->CreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 20.f * CLIENT_ASPECT , 20.f ) );
	//m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( currentMapSize.x * CLIENT_ASPECT , currentMapSize.y ) );
	LoadAssets();

	if ( g_characterSpriteSheet == NULL )
	{
		g_characterSpriteSheet = new SpriteSheet( *( g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/KushnariovaCharacters_12x53.png" ) ) , IntVec2( 12 , 53 ) );
	}
	
	TileDefinition::CreateTileDefinitions( "Data/GamePlay/TileDefs.xml" );
	MapDefinition::CreateMapDefinitions( "Data/GamePlay/MapDefs.xml" );
	ActorDefinition::CreateActorDefinitions( "Data/GamePlay/ActorDefs.xml" );

	m_world1 = new World( this );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAssets()
{
	LoadAllTextures();
	LoadAllSounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllSounds()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllTextures()
{
 	m_textures[ TEXTURE_TERRAIN_32x32 ] = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_32x32.png" );
	m_textures[ TEXTURE_CHARACTERS ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/KushnariovaCharacters_12x53.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
//	UpdateCamera();
	m_world1->Update( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	m_world1->Render();
	InspectOnMouseOver();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
//--------------------------------------------------------------------------------------------------------------------------------------------
//					CLAMPING THE CAMERA 
//--------------------------------------------------------------------------------------------------------------------------------------------

	
	Vec2 player = m_world1->m_currentMap->m_player->m_position;

	Vec2 cameraCoords;

	cameraCoords.x = Clamp( player.x , ( m_tilesInViewHorizontally / 2 ) , m_world1->m_currentMap->m_dimensions.x - ( m_tilesInViewHorizontally / 2 ) );
	cameraCoords.y = Clamp( player.y , ( m_tilesInViewVertically / 2 ) , m_world1->m_currentMap->m_dimensions.y - ( m_tilesInViewVertically / 2 ) );

	m_worldCamera.SetOrthoView( cameraCoords - m_CameraCenter , cameraCoords + m_CameraCenter );
	
	

	if ( g_theApp->m_debugCamera )
	{
		
	}

	m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	m_screenShakeIntensity = ClampZeroToOne( m_screenShakeIntensity );

	float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	float cameraShakeY = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	
	m_worldCamera.Translate2D( Vec2( cameraShakeX , cameraShakeY ) );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InspectOnMouseOver() const
{
	Vec2 normalizedMousePos = g_theInput->GetMouseNormalizedClientPosition();
	float positionX = RangeMapFloat( 0.f , 1.f , m_worldCamera.GetOrthoMin().x , m_worldCamera.GetOrthoMax().x , normalizedMousePos.x );
	float positionY = RangeMapFloat( 0.f , 1.f , m_worldCamera.GetOrthoMin().y , m_worldCamera.GetOrthoMax().y , normalizedMousePos.y );
	Vec2 mouseLocalPos = Vec2( positionX , positionY );
	AABB2 temp = AABB2( mouseLocalPos , Vec2( positionX + 10.f , positionY + 1.f ) );
	IntVec2 tileInfo = IntVec2( mouseLocalPos );
	
	std::string textTileCoords = "";
	textTileCoords += std::to_string( tileInfo.x ) + " , " + std::to_string( tileInfo.y );
	std::string textTileInfo = m_world1->m_currentMap->GetTileInfoForTileCoords( tileInfo );
	Vec2 textOffset = Vec2( 0.f , temp.GetDimensions().y );
	std::vector<Vertex_PCU> debugTileCoordsInfo;
	g_bitmapFont->AddVertsForTextInBox2D( debugTileCoordsInfo , temp , 0.4f , "Tile Coords : " + textTileCoords , GRAY, 1.f , ALIGN_BOTTOM_LEFT );
	std::vector<Vertex_PCU> debugTileNameInfo;
	temp = AABB2( temp.m_mins , temp.m_maxs + textOffset );
	g_bitmapFont->AddVertsForTextInBox2D( debugTileNameInfo , temp , 0.4f , textTileInfo , GRAY , 1.f, ALIGN_CENTERED_LEFT );
	//temp = AABB2( temp.m_mins , temp.m_maxs + textOffset );
	std::vector<Vertex_PCU> mapNameInfo;
	g_bitmapFont->AddVertsForTextInBox2D( mapNameInfo , temp , 0.4f , m_world1->m_currentMap->GetMapInfo() , GRAY , 1.f , ALIGN_TOP_LEFT );

	
	Map*  currentMap   = m_world1->m_currentMap;
	Actor* currentActor = nullptr;
	if ( currentMap->IsMousePointingActor( mouseLocalPos , currentActor ) )
	{
		Strings actorInfo = currentMap->m_player->GetActorDetails();
		std::vector<Vertex_PCU> actorInfoVerts;
		//float tempTextStartPos = 0.f;
		AABB2 actorDetailsAABB2 = temp;
		actorDetailsAABB2 = AABB2( actorDetailsAABB2.m_mins.x , actorDetailsAABB2.m_maxs.y , ( actorDetailsAABB2.m_maxs + textOffset ).x , ( actorDetailsAABB2.m_maxs + textOffset ).y );
		for ( int index = 0; index < actorInfo.size(); index++ )
		{
			actorDetailsAABB2 = AABB2( actorDetailsAABB2.m_mins , actorDetailsAABB2.m_maxs + textOffset );
			g_bitmapFont->AddVertsForTextInBox2D( actorInfoVerts , actorDetailsAABB2 , 0.4f , actorInfo[ index ] , GRAY , 1.f , ALIGN_CENTERED_LEFT );
			//g_bitmapFont->AddVertsForText2D( actorInfoVerts , temp.m_maxs + Vec2( 3.f , 0.f ) /*+ tempTextStartPos*/ , 0.5f , actorInfo[ index ] , Rgba8( 100 , 100 , 200 , 255 ) );
			//tempTextStartPos += 1.f;
		}
		g_theRenderer->DrawAABB2( actorDetailsAABB2 , Rgba8( 0 , 0 , 100 , 200 ) );
		g_theRenderer->BindTexture( g_bitmapFont->GetTexture() );
		g_theRenderer->DrawVertexArray( actorInfoVerts );
		g_theRenderer->BindTexture( nullptr );
	}
	
	g_theRenderer->DrawAABB2( temp, Rgba8( 0 , 255 , 100 , 180 ) );
	g_theRenderer->BindTexture( g_bitmapFont->GetTexture() );
	g_theRenderer->DrawVertexArray( debugTileCoordsInfo );
	g_theRenderer->DrawVertexArray( debugTileNameInfo );
	g_theRenderer->DrawVertexArray( mapNameInfo );
	g_theRenderer->BindTexture( nullptr );
// 	if ( m_map->IsMousePointingNpcUnits( Vec2( m_posx , m_posy ) ) && !m_map->IsMousePointingActor( Vec2( m_posx , m_posy ) ) )
// 	{
// 		g_theRenderer->DrawAABB2D( AABB2( Vec2( m_posx , m_posy + 3.f ) , Vec2( m_posx + 10.f , m_posy + 7.f ) ) , Rgba8( 0 , 0 , 100 , 200 ) );
// 		Strings npcInfo = m_map->GetNpcDetails( Vec2( m_posx , m_posy ) );
// 		std::vector<Vertex_PCU> npcInfoVerts;
// 		float tempTextStartPos = 0.f;
// 		for ( int index = 0; index < npcInfo.size(); index++ )
// 		{
// 			m_BitmapFont->AddVertsForText2D( npcInfoVerts , Vec2( m_posx , m_posy + 3.f + tempTextStartPos ) , 0.5f , npcInfo[ index ] , Rgba8( 100 , 100 , 200 , 255 ) );
// 			tempTextStartPos += 1.f;
// 		}
// 		g_theRenderer->BindTexture( m_BitmapFont->GetTexture() );
// 		g_theRenderer->DrawVertexArray( npcInfoVerts );
// 		g_theRenderer->BindTexture( nullptr );
// 	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Die()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderUIVerts() const
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------
