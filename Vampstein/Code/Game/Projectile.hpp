﻿#pragma once
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Projectile : public Entity
{
public:
	Projectile( EntityDef const& entityDef , Map* map );
	~Projectile();
};

//--------------------------------------------------------------------------------------------------------------------------------------------