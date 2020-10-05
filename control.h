#pragma once 

#include "common.h"

class Control
{
public:
	static void move(b2Body* body, const Inputs& inputs);
	static void move_without_gravity(b2Body* body, const Inputs& inputs);
};