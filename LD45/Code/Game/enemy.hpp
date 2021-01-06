#pragma once
#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class Enemy :public Entity
{

private:


public:
	
	Rgba8 enemyColor = Rgba8( 100 , 100 , 100 , 255 );

public:

	Enemy( Game* currentGameInstance );

	void InitializeMemberVariables();

	void Update( float deltaSeconds ) override;
	void Render() const;
	void Die();

	void RollEnemy( float deltaSeconds );

};
