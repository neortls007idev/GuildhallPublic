#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/AttractMode.hpp"
#include "Game/TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	RenderContext*					g_theRenderer;
extern	TheApp*							g_theApp;
extern	InputSystem*					g_theInput;

//--------------------------------------------------------------------------------------------------------------------------------------------

AttractMode::AttractMode ()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AttractMode::InitializeAttractModeVertices ()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AttractMode::BeginFrame ()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AttractMode::Update ( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
	XboxController controller = g_theInput->GetXboxController( m_controllerID );
	const KeyButtonState& startButton = controller.GetButtonState( XBOX_BUTTON_ID_START );
	if ( g_theInput->WasKeyJustPressed( KEY_SPACE ) == true || g_theInput->WasKeyJustPressed( 'N' ) == true ||
		startButton.WasJustPressed() )
	{
		EndFrame();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AttractMode::Render () const
{
	static bool colorStatus = false;
	static Rgba8 color;
	color.r += 5;
	color.b += 7;
	color.g += 1;

	if ( colorStatus == false )
	{
		Vertex_PCU tempVerts[NUM_ATTRACTMODE_VERTS];
		for ( int vertIndex = 0 ; vertIndex < NUM_ATTRACTMODE_VERTS ; vertIndex++ )
		{
			tempVerts[ vertIndex ] = m_localAttractModeVerts[ vertIndex ];
			tempVerts[ vertIndex ].m_color = color;
		}
		TransformVertexArray2D( NUM_ATTRACTMODE_VERTS , tempVerts , 1.f , m_orientationDegrees , m_position );
		g_theRenderer->DrawVertexArray( NUM_ATTRACTMODE_VERTS , tempVerts );
		colorStatus = true;
	}
	else if ( colorStatus )
	{
		Vertex_PCU tempVerts[NUM_ATTRACTMODE_VERTS];
		for ( int vertIndex = 0 ; vertIndex < NUM_ATTRACTMODE_VERTS ; vertIndex++ )
		{
			tempVerts[ vertIndex ] = m_localAttractModeVerts[ vertIndex ];
			tempVerts[ vertIndex ].m_color = color;
		}
		TransformVertexArray2D( NUM_ATTRACTMODE_VERTS , tempVerts , 1.f , m_orientationDegrees , m_position );
		g_theRenderer->DrawVertexArray( NUM_ATTRACTMODE_VERTS , tempVerts );
		colorStatus = false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AttractMode::Die ()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AttractMode::EndFrame ()
{
	g_theApp->m_attractModeStatus = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
