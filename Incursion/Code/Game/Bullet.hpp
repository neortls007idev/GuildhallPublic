#pragma once
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Bullet : public Entity
{
public:
	
	explicit Bullet( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction );
	~Bullet()
	{
	};
	void InitializeBulletMemberVariables();

	void Update( float deltaSeconds ) override;
	void Render() const override;
	void IsDead() override;

private:

	Vertex_PCU m_bulletVerts[ 6 ] = {
								Vertex_PCU( Vec3( -0.05f, -0.05f, 0.f ) , WHITE, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( Vec3(  0.05f, -0.05f, 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( -0.05f,  0.05f, 0.f ) , WHITE, Vec2( 0.f, 1.f ) ),
														 	    
								Vertex_PCU( Vec3(  0.05f, -0.05f, 0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3(  0.05f,  0.05f, 0.f ) , WHITE, Vec2( 1.f, 1.f ) ),
								Vertex_PCU( Vec3( -0.05f,  0.05f, 0.f ) , WHITE, Vec2( 0.f, 1.f ) ) };

};

//--------------------------------------------------------------------------------------------------------------------------------------------

