#include "physics.h"

b2World* Physics::constructWorld(b2World* world)
{
    if (world == nullptr)
    {
        world = new b2World({ 0,-10 });
    }

	b2BodyDef bd;
	auto body = world->CreateBody(&bd);

	b2FixtureDef fd;
	b2EdgeShape sd;
	sd.SetTwoSided({ -100,-1 }, { 100, -1 });

	fd.shape = &sd;
	fd.density = 1.0f;
	fd.restitution = 0.0f;  
	fd.friction = 1.0f;
	auto fixture = body->CreateFixture(&fd);

	sd.SetTwoSided({ -10, 10 }, { 10, 10 });
	fixture = body->CreateFixture(&fd);

    return world;
}

b2Body* Physics::createEntity(b2World* world)
{
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	auto body = world->CreateBody(&bd);

	b2FixtureDef fd;
	b2CircleShape sd;
	sd.m_radius = 1.0f;

	fd.shape = &sd;
	fd.density = 1.0f;
	fd.restitution = 1.0f;
	fd.friction = 1.0f;
	auto fixture = body->CreateFixture(&fd);
	body->SetFixedRotation(true);
	return body;
}
