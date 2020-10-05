#include "server.h"
#include "control.h"
#include "draw.h"
#include "physics.h"
Server::Server(b2World* w):
	world(w)
{
	Physics::constructWorld(w);


}

void Server::preupdate()
{
	curframe.id++;

	for (auto& c : clients)
	{
		auto& state = curframe.states[c.client->id];

		//simulate sending client msg 
		c.client->send(this);

		// apply collected inputs
		auto ctrl = c.client->controller;
		if (!ctrl)
			continue;
		ctrl(c.body, state.inputs);
	}
}

void Server::update(float dtime)
{
	preupdate();

	world->Step(dtime, 8, 3);

	postupdate(dtime);

}

void Server::postupdate(float dtime)
{
	// readback to logic
	for (auto& c : clients)
	{
		auto& pos = c.body->GetPosition();
		auto& vel = c.body->GetLinearVelocity();

		auto& state = curframe.states[c.client->id];
		state.pos = pos;
		state.vel = vel;

	}

	//simulate sending server msg
	for (auto& c : clients)
	{
		c.client->receive(curframe);
	}
}

void Server::addClient( Client* client)
{
	clients.push_back({ client , Physics::createEntity(world)});
}

void Server::addInputs(size_t id, const Inputs & inputs)
{
	auto& state = curframe.states[id];
	state.inputs = inputs;
}

const Frame& Server::getFrame()
{
	return curframe;
}