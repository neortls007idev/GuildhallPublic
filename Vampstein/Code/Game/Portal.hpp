#pragma once
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Portal : public Entity
{
public:
	Portal( EntityDef const& entityDef , Map* map );
	~Portal();

	//virtual void Update( float deltaseconds ) override;
	//virtual void Render() override;

public:
	std::string		m_destMap		= "";
	Vec2			m_desPos		= Vec2::ZERO;
	float			m_desYawOffset	= 0.f;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------