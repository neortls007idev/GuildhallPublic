﻿#pragma once
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Actor : public Entity
{
public:
	Actor( EntityDef const& entityDef , Map* map );
	~Actor();
};

//--------------------------------------------------------------------------------------------------------------------------------------------