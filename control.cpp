#include "control.h"
#define GLFW_INCLUDE_NONE
#include "glad/gl.h"
#include "GLFW/glfw3.h"

void Control::move(b2Body* body, const Inputs& inputs)
{

	auto fixture = body->GetFixtureList();
	if (fixture->GetRestitution() != 0)
	{
		fixture->SetRestitution(0.0f);
	}

	if (fixture->GetFriction() != 0)
		fixture->SetRestitution(0);
	float rot = 0;
	b2Vec2 imp = { 0,0 };
	b2Vec2 vel = { 0,0 };
	for (auto& i : inputs)
	{
		if (!i.second)
			continue;

		auto key = i.first;

		if (key == GLFW_KEY_A)
			vel += {-1, 0};
		if (key == GLFW_KEY_W)
			imp = { 0,1 };
		if (key == GLFW_KEY_D)
			vel += {1, 0};
		//if (key == GLFW_KEY_S)
		//	vel += {0, -1};
	}
	vel *= 20;
	imp *= 20;

	auto v = body->GetLinearVelocity();
	if (v.y != 0)
		imp = { 0,0 };
	vel.y = v.y;
	body->SetLinearVelocity(vel + imp);
}


void Control::move_without_gravity( b2Body* body, const Inputs& inputs)
{
	body->SetGravityScale(0.0f);
	b2Vec2 vel = { 0,0 };
	for (auto& i : inputs)
	{
		if (!i.second)
			continue;

		auto key = i.first;

		if (key == GLFW_KEY_A)
			vel += {-1, 0};
		if (key == GLFW_KEY_W)
			vel += {0, 1};
		if (key == GLFW_KEY_D)
			vel += {1, 0};
		if (key == GLFW_KEY_S)
			vel += {0, -1};
	}
	vel.Normalize();
	vel *= 10;

	body->SetLinearVelocity(vel);
}
