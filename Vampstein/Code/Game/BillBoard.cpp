#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/BillBoard.hpp"

#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

BillBoard::BillBoard ( AABB2 bounds , Mat44 initialTransform , Vec3 pos , Vec2 minUvs , Vec2 maxUvs , Texture* tex ,
                       eBillBoardType type /*= CAMERA_FACING_XY */ ) :
																	m_bounds ( bounds ) , 
																	m_transform ( initialTransform ) ,
																	m_pos ( pos ) ,
																	m_minUVs ( minUvs ) ,
																	m_maxUVs ( maxUvs ) ,
																	m_spriteTex ( tex ) ,
																	m_type( type )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void BillBoard::Update()
{
	switch ( m_type )
	{
		case CAMERA_FACING_XY:		UpdateCameraFacingXY();
			break;
		case CAMERA_OPPOSED_XY:		UpdateCameraOpposedXY();
			break;
		case CAMERA_OPPOSED_XYZ:	UpdateCameraOpposedXYZ();
			break;
		case CAMERA_FACING_XYZ:		UpdateCameraFacingXYZ();
			break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void BillBoard::Render()
{
	if( !g_theGame->m_debugDraw )
	{
		//g_theRenderer->BindTexture( g_theGame->m_textures[ TEST_TEXTURE ] );
		g_theRenderer->BindTexture( m_spriteTex );
		g_theRenderer->SetModelMatrix( m_transform );
		g_theRenderer->DrawAABB2( m_bounds , WHITE , m_minUVs , m_maxUVs );
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	}
	
	if ( g_theGame->m_debugDraw )
	{
		g_theRenderer->BindShader( g_theGame->m_shaders[ UV_DEBBUGER ] );
		g_theRenderer->SetModelMatrix( m_transform );
		g_theRenderer->DrawAABB2( m_bounds , WHITE );
		g_theRenderer->BindShader( nullptr );
		g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void BillBoard::UpdateCameraFacingXY()
{
	Vec3 targetLocation = g_theGame->m_gameCamera.GetCameraTransform().GetPostion();

	Vec3 forward = ( targetLocation - m_pos );
	forward = forward.GetNormalized();

	if( forward.GetLengthSquared() <= 0.001 )
	{
		m_transform = Mat44::IDENTITY;
	}

	Vec3 left = CrossProduct3D( -forward , Vec3::UNIT_VECTOR_ALONG_K_BASIS ).GetNormalized();

	if( left.GetLengthSquared() <= 0.001 )
	{
		left = CrossProduct3D( forward , Vec3( 0 , -1 , 0 ) );
	}

	Vec3 up		= Vec3::UNIT_VECTOR_ALONG_K_BASIS;
	forward.z	= 0.f;
	left.z		= 0.f;
	
	m_transform.SetBasisVectors3D( left , up , forward , m_pos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void BillBoard::UpdateCameraOpposedXY()
{
	Vec3 forward		= -g_theGame->m_gameCamera.GetCameraTransform().GetAsMatrix().GetIBasis3D();
	forward				= forward.GetNormalized();
	forward.z			= 0.f;

	Vec3 left( -forward.y , forward.x , 0.f );

	m_transform.SetBasisVectors3D( left , Vec3::UNIT_VECTOR_ALONG_K_BASIS , forward );
	m_transform.SetTranslation3D( m_pos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void BillBoard::UpdateCameraOpposedXYZ()
{
	Vec3 forward = -g_theGame->m_gameCamera.GetCameraTransform().GetAsMatrix( X_IN_Y_LEFT_Z_UP ).GetIBasis3D();
	forward		 = forward.GetNormalized();

	Vec3 left	 = -g_theGame->m_gameCamera.GetCameraTransform().GetAsMatrix( X_IN_Y_LEFT_Z_UP ).GetJBasis3D();
	left		 = left.GetNormalized();
	
	Vec3 up		 = g_theGame->m_gameCamera.GetCameraTransform().GetAsMatrix( X_IN_Y_LEFT_Z_UP ).GetKBasis3D();
	up			 = up.GetNormalized();

	m_transform.SetBasisVectors3D( left , up , forward );
	m_transform.SetTranslation3D( m_pos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void BillBoard::UpdateCameraFacingXYZ()
{
	Vec3 targetLocation = g_theGame->m_gameCamera.GetCameraTransform().GetPostion();

	Vec3 forward = ( targetLocation - m_pos );
	forward = forward.GetNormalized();

	if( forward.GetLengthSquared() <= 0.001 )
	{
		m_transform = Mat44::IDENTITY;
	}

	Vec3 left = CrossProduct3D( -forward , Vec3::UNIT_VECTOR_ALONG_K_BASIS ).GetNormalized();

	if( left.GetLengthSquared() <= 0.001 )
	{
		left = CrossProduct3D( forward , Vec3( 0 , -1 , 0 ) );
	}

	Vec3 up		= Vec3::UNIT_VECTOR_ALONG_K_BASIS;	
	m_transform.SetBasisVectors3D( left , up , forward , m_pos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------