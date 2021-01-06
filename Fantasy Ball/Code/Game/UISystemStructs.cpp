#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/UISystem.hpp"
#include "Game/UISystemStructs.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern UISystem*		g_theGamplayUISystem;
extern InputSystem*		g_theInput;

//--------------------------------------------------------------------------------------------------------------------------------------------

UISlider::UISlider( AABB2 cosmeticBase , AABB2 cosmeticFillBar , AABB2 cosmeticButton , Vec2 centerPos , float minValue , float maxValue , float value ) :
																													m_cosmeticBase( cosmeticBase ) ,
																													m_cosmeticFillBar( cosmeticFillBar ) ,
																													m_cosmeticButton( cosmeticButton ) ,
																													m_centerPos( centerPos ) ,
																													m_minRange( minValue ) ,
																													m_maxRange( maxValue ) ,
																													m_value( value )
{
	
	m_cosmeticBase.SetCenter( centerPos );
	m_cosmeticFillBar.SetCenter( centerPos );

	m_physicalButton.SetDimensions( m_cosmeticButton.GetDimensions() * 0.9f );

	m_buttonMovementRange = Vec2( m_cosmeticFillBar.m_mins.x , m_cosmeticFillBar.m_maxs.x );
	float buttonPosX = RangeMapFloat( m_minRange , m_maxRange , m_buttonMovementRange.x , m_buttonMovementRange.y , value );

	m_cosmeticButton.SetCenter( buttonPosX , centerPos.y );
	m_physicalButton.SetCenter( buttonPosX , centerPos.y );
	
	//m_cosmeticFillBar.m_mins.x = m_cosmeticBase.m_mins.x + m_cosmeticButton.GetDimensions().x * 1.5f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISlider::Update()
{
	Vec2	normalizedPos	= g_theInput->GetMouseNormalizedClientPosition();
	AABB2	screenSpace		= g_theGamplayUISystem->GetUIScreenSpace();
	float	mouseClientPosX = RangeMapFloatNormalizedInput( screenSpace.m_mins.x , screenSpace.m_maxs.x , normalizedPos.x );
	float	mouseClientPosY = RangeMapFloatNormalizedInput( screenSpace.m_mins.y , screenSpace.m_maxs.y , normalizedPos.y );
	Vec2	mouseClientPos( mouseClientPosX , mouseClientPosY );


	if( IsPointInsideAABB2D( mouseClientPos , m_physicalButton ) )
	{
		if( g_theInput->IsLeftMouseButtonHeldDown() )
		{
			float newXPos = Clamp( mouseClientPosX , m_buttonMovementRange.x , m_buttonMovementRange.y );
			//float newXPos = Clamp( mouseClientPosX , m_cosmeticBase.m_mins.x + 32.f, m_cosmeticBase.m_maxs.x - 32.f);
			m_physicalButton.SetCenter( newXPos , m_physicalButton.GetCenter().y );			
			m_cosmeticButton.SetCenter( newXPos , m_cosmeticButton.GetCenter().y );
		}
	}
	float value = m_cosmeticButton.GetCenter().x;
	m_value = RangeMapFloat( m_buttonMovementRange.x , m_buttonMovementRange.y , m_minRange , m_maxRange , value );
	m_cosmeticFillBar.m_maxs.x = m_cosmeticButton.GetCenter().x;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISlider::Render()
{
	g_theRenderer->BindTexture( g_theGamplayUISystem->GetUITexture( GEN_SLIDER_BASE ) );
	g_theRenderer->DrawAABB2( m_cosmeticBase , WHITE );
	g_theRenderer->BindTexture( g_theGamplayUISystem->GetUITexture( GEN_SLIDER_FILLBAR ) );
	g_theRenderer->DrawAABB2( m_cosmeticFillBar , WHITE );
	
	g_theRenderer->BindTexture( g_theGamplayUISystem->GetUITexture( GEN_SLIDER_BORDER ) );
	g_theRenderer->DrawAABB2( m_cosmeticBase , WHITE );
	g_theRenderer->BindTexture( g_theGamplayUISystem->GetUITexture( GEN_SLIDER_BUTTON ) );
	g_theRenderer->DrawAABB2( m_cosmeticButton , WHITE );
	g_theRenderer->BindTexture( nullptr );

	if( g_theGamplayUISystem->m_UIDebugDraw )
	{
		g_theRenderer->DrawUnfilledAABB2( m_cosmeticBase	, MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_cosmeticFillBar , MAGENTA , 2.f );
		g_theRenderer->DrawUnfilledAABB2( m_cosmeticButton	, MAGENTA , 2.f );
												  
		g_theRenderer->DrawUnfilledAABB2( m_physicalButton	, CYAN , 2.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UISlider::SetValue( const float value )
{
	m_value = Clamp( value , m_minRange , m_maxRange );
	//m_buttonMovementRange = Vec2( m_cosmeticFillBar.m_mins.x , m_cosmeticFillBar.m_maxs.x );
	float buttonPosX = RangeMapFloat( m_minRange , m_maxRange , m_buttonMovementRange.x , m_buttonMovementRange.y , value );
	float buttonPosY = m_physicalButton.GetCenter().y;
	m_cosmeticButton.SetCenter( buttonPosX , buttonPosY );
	m_physicalButton.SetCenter( buttonPosX , buttonPosY );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//Texture* UISlider::m_Base;
//Texture* UISlider::m_Border;
//Texture* UISlider::m_fillBar;
//Texture* UISlider::m_button;

//--------------------------------------------------------------------------------------------------------------------------------------------