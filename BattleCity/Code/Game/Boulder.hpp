#pragma once
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Boulder : public Entity
{
public:
	
	explicit Boulder( Game* CurrentGameInstance , Vec2 position , Vec2 velocity , float orientation , EntityType type , Faction faction );
	~Boulder();
	
	void InitializeBoulderMemberVariables();

	void Update( float deltaSeconds ) override;
	void Render() const override;
	void IsDead() override;

private:

	Vertex_PCU m_boulderVerts[ 6 ] = {
								Vertex_PCU( Vec3( -0.4f,-0.4f,0.f ) , WHITE, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( Vec3(  0.4f,-0.4f,0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( -0.4f, 0.4f,0.f ) , WHITE, Vec2( 0.f, 1.f ) ),

								Vertex_PCU( Vec3( 0.4f,-0.4f,0.f ) , WHITE, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( 0.4f, 0.4f,0.f ) , WHITE, Vec2( 1.f, 1.f ) ),
								Vertex_PCU( Vec3( -0.4f,0.4f,0.f ) , WHITE, Vec2( 0.f, 1.f ) ) };

	Texture* m_boulderTextureSprite = nullptr;
	SpriteSheet* m_boulderSpriteSheet = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------