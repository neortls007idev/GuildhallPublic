#pragma once
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class NpcTank : public Entity
{

//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	explicit NpcTank( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction );
	~NpcTank() {};
	
	void InitializeNpcTankMemberVariables();

	void Update( float deltaSeconds ) override;
	void Render() const override;
	void IsDead() override;

//--------------------------------------------------------------------------------------------------------------------------------------------

public:

	float m_lineOfSightRadius;
	float m_firingCoolDown;
	float m_orientationChangeTimer;
	float m_newGoalOrientation = 0.f;
	Vec2 m_positionBeforeASecond;
	Vec2 m_lastSeenPosition;
	bool m_wasPlayerSeen = false;
	RayCastResult m_whiskers[3];
	/*float m_positionTimer = 0.f;*/
	
//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	Vertex_PCU m_npcTankVerts[ 6 ] = {
								Vertex_PCU( Vec3( -0.4f,-0.4f,0.f ) , WHITE, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( Vec3( 0.4f,-0.4f,0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( -0.4f,0.4f,0.f ) , WHITE, Vec2( 0.f, 1.f ) ),

								Vertex_PCU( Vec3( 0.4f,-0.4f,0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( 0.4f,0.4f,0.f ) , WHITE, Vec2( 1.f, 1.f ) ),
								Vertex_PCU( Vec3( -0.4f,0.4f,0.f ) , WHITE, Vec2( 0.f, 1.f ) ) };

};

//--------------------------------------------------------------------------------------------------------------------------------------------
