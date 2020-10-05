#pragma once 

#include "common.h"

class Physics
{
public:
	static b2World* constructWorld(b2World* world = nullptr);
	static b2Body* createEntity(b2World* world);
};