
#include "Game/PlayerShip.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"

extern RenderContext* g_theRenderer;	
extern TheApp* g_theApp;

PlayerShip::PlayerShip(Game* CurrentGameInstance, Vec2 pos, Vec2 velocity, float orientation):
			Entity(CurrentGameInstance,
					pos,
					velocity,
					orientation)
{
	InitializePlayerShipMemberVariables();

}

void PlayerShip::Update (float deltaSeconds) 
{
	UpdateFromJoystick(deltaSeconds);
	if (m_isDead)
	{
		shipColor = Rgba8(132, 156, 169, 0);
		return;
	}
	checkKeyboardKeyPressForShipMovement(deltaSeconds);
	Entity::Movement(deltaSeconds);
	Entity::WrapAroundScreen();
}

void PlayerShip::Render() const
{
	
	Vertex_PCU tempVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++)
	{
		tempVerts[vertIndex] = m_localShipVerts[vertIndex];
		tempVerts[vertIndex].m_color = shipColor;
		
	}
	TransformVertexArray2D(NUM_SHIP_VERTS, tempVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, tempVerts);

}

void PlayerShip::IsDead()
{
	if ( m_health == 0)
	{
		m_isDead = true;
		m_game->AddScreenShakeIntensity( 1.f );
	}
}


void PlayerShip::UpdateFromJoystick(float deltaSeconds)
{
	m_controllerID = g_theInput->GetXboxController(m_controllerID).GetControllerID();
	// Do nothing if this player ship has no controller assigned to it
	if (m_controllerID < 0)
		return;

	// Do nothing if this player's controller is not plugged in
	const XboxController& controller = g_theInput->GetXboxController(m_controllerID);
	if (!controller.IsConnected())
		return;
	if (g_theApp->m_isPaused == false)
	{
	XboxButtonPresses(controller, deltaSeconds);
	}

}

void PlayerShip::checkKeyboardKeyPressForShipMovement(float deltaSeconds)
{
	if (g_theInput->IsKeyHeldDown(KEY_UPARROW)) // Up key
	{
		Accelerate(deltaSeconds);
	}
	if (g_theInput->IsKeyHeldDown(KEY_LEFTARROW)) // Left key
	{
		RotateLeft(deltaSeconds);
	}
	if (g_theInput->IsKeyHeldDown(KEY_RIGHTARROW)) // Right key
	{
		RotateRight(deltaSeconds);
	}
}

void PlayerShip::XboxButtonPresses(const XboxController& controller, float deltaSeconds)
{
	// Check if player is dead; if so, respawn on Start button (only)
	if (m_isDead)
	{
		const KeyButtonState& startButton = controller.GetButtonState(XBOX_BUTTON_ID_START);
		if (startButton.WasJustPressed())
		{
			if (m_isDead)
			{
				m_game->m_Ship = nullptr;
				delete m_game->m_Ship;
				m_game->m_Ship = new PlayerShip(m_game, Vec2(WORLD_CENTER_X, WORLD_CENTER_Y), Vec2(0.f, 0.f), 0.f);
			}
		}
	}

	// Apply turning and thrust from left stick IF it is actively being used
	const AnalogJoystick& leftStick = controller.GetLeftJoystick();
	float leftStickMagnitude = leftStick.GetMagnitude();
	if (leftStickMagnitude > 0.f && m_health>0) // This is already deadzone-corrected; so anything non-zero means “significant movement” (outside the inner dead zone)
	{
		m_orientationDegrees = leftStick.GetAngleDegrees();
		m_thrustFraction = leftStickMagnitude;

		m_velocity.x = m_velocity.x + ((PLAYER_SHIP_ACCELERATION * deltaSeconds * CosDegrees(m_orientationDegrees)) * m_thrustFraction);
		m_velocity.y = m_velocity.y + ((PLAYER_SHIP_ACCELERATION * deltaSeconds * SinDegrees(m_orientationDegrees)) * m_thrustFraction);
		
		if ( leftStickMagnitude > 0.9)
		{
		g_theInput->GetXboxController(m_controllerID).SetVibration(0.5f,0.5f);
		}
		else
		{
		g_theInput->GetXboxController(m_controllerID).SetVibration(0.f, 0.f);
		}

	}

	// Fire a bullet if "A" was just pressed this frame
	const KeyButtonState& aButton = controller.GetButtonState(XBOX_BUTTON_ID_A);
	if (aButton.WasJustPressed() && m_health > 0)
	{
		m_game->AddScreenShakeIntensity(0.2f);
		m_game->SpawnBullet((m_position + (2.5 * Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees)))), m_orientationDegrees);
	}
}

void PlayerShip::Accelerate(float deltaTime)
{
	m_velocity.x = m_velocity.x + (PLAYER_SHIP_ACCELERATION * deltaTime * CosDegrees(m_orientationDegrees));
	m_velocity.y = m_velocity.y + (PLAYER_SHIP_ACCELERATION * deltaTime * SinDegrees(m_orientationDegrees));
}

void PlayerShip::RotateLeft(float deltaSeconds)
{
	m_orientationDegrees += (300.f*deltaSeconds);
}

void PlayerShip::RotateRight(float deltaSeconds)
{
	m_orientationDegrees -= (300.f * deltaSeconds);
}

void PlayerShip::InitializePlayerShipMemberVariables()
{
	m_health = 4;
	m_isDead = false;
	m_isGarbage = false;
	m_physicsRadius = 1.75f;
	m_cosmeticRadius = 2.25f;
	m_age = 3.0f;
}
