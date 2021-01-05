#pragma once
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class NpcTurret : public Entity
{
public:

	explicit NpcTurret( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction );
	~NpcTurret() {};
	
	void InitializeNpcTurretMemberVariables();

	void Update( float deltaSeconds ) override;
	void Render() const override;
	void IsDead() override;

public:

	float		  m_turretOrientation;
	float		  m_lineOfSightRadius;
	float		  m_firingCoolDown;
	RayCastResult m_laser;
	Vec2		  m_lastSeenPosition;
	bool		  m_lastSeen;
	bool		  m_hasReachedInitialPosition;
	bool		  m_hasReachedFinalPosition;
	float		  m_goalDegrees;
	
private:
	Vertex_PCU m_npcTurretBaseVerts[ 6 ] = {
												Vertex_PCU( Vec3( -0.4f,-0.4f,0.f ) , WHITE, Vec2( 0.f, 0.f ) ),
												Vertex_PCU( Vec3(  0.4f,-0.4f,0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
												Vertex_PCU( Vec3( -0.4f, 0.4f,0.f ) , WHITE, Vec2( 0.f, 1.f ) ),

												Vertex_PCU( Vec3(  0.4f,-0.4f,0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
												Vertex_PCU( Vec3(  0.4f, 0.4f,0.f ) , WHITE, Vec2( 1.f, 1.f ) ),
												Vertex_PCU( Vec3( -0.4f, 0.4f,0.f ) , WHITE, Vec2( 0.f, 1.f ) ) };

	Vertex_PCU m_npcTurretVerts[ 6 ] = {
												Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ) , WHITE, Vec2( 0.f, 0.f ) ),
												Vertex_PCU( Vec3(  0.4f, -0.4f, 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
												Vertex_PCU( Vec3( -0.4f,  0.4f, 0.f ) , WHITE, Vec2( 0.f, 1.f ) ),

												Vertex_PCU( Vec3(  0.4f, -0.4f, 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
												Vertex_PCU( Vec3(  0.4f,  0.4f, 0.f ) , WHITE, Vec2( 1.f, 1.f ) ),
												Vertex_PCU( Vec3( -0.4f,  0.4f, 0.f ) , WHITE, Vec2( 0.f, 1.f ) ) };


};

//--------------------------------------------------------------------------------------------------------------------------------------------
