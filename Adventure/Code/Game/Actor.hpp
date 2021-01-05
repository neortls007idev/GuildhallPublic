#pragma once
#include "Game/Entity.hpp"
#include "Game/ActorDefs.hpp"
#include "Engine/Core/StringUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------


class Actor : public Entity
{
public:
	Actor( Game* theGame , Vec2 position,  float orientation , ActorDefinition* definition );
	~Actor() {};

	void Update( float deltaSeconds ) override;
	void Render() const override;
	void IsDead() override;

	void UpdateFromJoystick( float deltaSeconds );
	void XboxButtonPresses( const XboxController& controller , float deltaSeconds );

	Strings GetActorDetails();

private:

public:
	ActorDefinition* m_actorDef;

private:
	int						m_controllerID = -1;
	Vertex_PCU				m_actorVerts[ 6 ];
	std::vector<Vertex_PCU> m_actorVertsV;
	float					m_timeElapsed	= 0.f;

};
