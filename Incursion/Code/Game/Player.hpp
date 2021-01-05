#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Primitives/AABB2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Player :public Entity
{

public:

	explicit Player( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction );
	void InitializePlayerMemberVariables();

	void Update( float deltaSeconds ) override;
	void Render() const override;
	void IsDead() override;

	//void RenderingDeveloperMode();

	void checkKeyboardKeyPressForMovement( float deltaSeconds );

	void TankMovement( float deltaSeconds );

	void UpdateFromJoystick( float deltaSeconds );
	void XboxButtonPresses( const XboxController& controller , float deltaSeconds );

	void RotateTankLeft( float deltaSeconds );
	void RotateTankRight( float deltaSeconds );
	void RotateTankUp( float deltaSeconds );
	void RotateTankDown( float deltaSeconds );

	void RotateTurretLeft( float deltaSeconds );
	void RotateTurretRight( float deltaSeconds );
	void RotateTurretUp( float deltaSeconds );
	void RotateTurretDown( float deltaSeconds );

//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	float m_turretOrientation;

private:

	int m_controllerID = -1;
	Vec2 m_thrustFraction = Vec2(0.f,0.f);
	bool m_updatefromjoystick = false;

	Vertex_PCU m_playerTankVerts[ 6 ] = {
												Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ) , WHITE, Vec2( 0.f, 0.f ) ),
												Vertex_PCU( Vec3(  0.4f, -0.4f, 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
												Vertex_PCU( Vec3( -0.4f,  0.4f, 0.f ) , WHITE, Vec2( 0.f, 1.f ) ),
																			    
												Vertex_PCU( Vec3(  0.4f, -0.4f, 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
												Vertex_PCU( Vec3(  0.4f,  0.4f, 0.f ) , WHITE, Vec2( 1.f, 1.f ) ),
												Vertex_PCU( Vec3( -0.4f,  0.4f, 0.f ) , WHITE, Vec2( 0.f, 1.f ) ) };
	Vertex_PCU m_playerTurretVerts[ 6 ] = {
												Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ) , WHITE, Vec2( 0.f, 0.f ) ),
												Vertex_PCU( Vec3(  0.4f, -0.4f, 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
												Vertex_PCU( Vec3( -0.4f,  0.4f, 0.f ) , WHITE, Vec2( 0.f, 1.f ) ),

												Vertex_PCU( Vec3(  0.4f, -0.4f, 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
												Vertex_PCU( Vec3(  0.4f,  0.4f, 0.f ) , WHITE, Vec2( 1.f, 1.f ) ),
												Vertex_PCU( Vec3( -0.4f,  0.4f, 0.f ) , WHITE, Vec2( 0.f, 1.f ) ) };

};

//--------------------------------------------------------------------------------------------------------------------------------------------
