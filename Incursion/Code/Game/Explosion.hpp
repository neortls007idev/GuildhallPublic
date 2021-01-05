#pragma once
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Explosion : public Entity
{


public:

	Explosion( Game* theGame , Vec2 position , float orientation , float blastRadius = 1.0f , float animationDuration = 1.0f );

	virtual void Update( float deltaSeconds ) override;
	void Render() const override;
	virtual void IsDead() override;
	Vec2 m_uvMins;
	Vec2 m_uvMaxs;

	//--------------------------------------------------------------------------------------------------------------------------------------------

private:

	float m_animationDuration = 1.f;
	float m_blastRadius = 1.f;
	Vertex_PCU m_ExplosionVerts[ 6 ] ={

					Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , m_uvMins ),
					Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_uvMaxs.x , m_uvMins.y ) ),
					Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_uvMins.x , m_uvMaxs.y ) ),

					Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_uvMaxs.x , m_uvMins.y ) ),
					Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , m_uvMaxs ),
					Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_uvMins.x , m_uvMaxs.y ) ) };

//--------------------------------------------------------------------------------------------------------------------------------------------
};