#pragma once

#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"


class Player :public Entity
{

public:
	explicit Player( Game* CurrentGameInstance , Vec2 m_position , Vec2 m_velocity , float m_orientationDegrees );
	void InitializePlayerShipMemberVariables();

	void Update( float deltaSeconds ) override;
	void Render() const override;
	void IsDead() override;

	//void RenderingDeveloperMode();

	void checkKeyboardKeyPressForShipMovement( float deltaSeconds );
	void UpdateFromJoystick( float deltaSeconds );
	void XboxButtonPresses( const XboxController& controller , float deltaSeconds );

	void Accelerate( float deltaTime );
	void RotateLeft( float deltaSeconds );
	void RotateRight( float deltaSeconds );

private:

	int m_controllerID = -1;
	float m_thrustFraction = 0.f;
	Rgba8 shipColor = Rgba8( 132 , 156 , 169 , 255 );
	bool m_updatefromjoystick = false;
};
