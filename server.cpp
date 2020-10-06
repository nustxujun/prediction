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

	curFrameID++;
	auto& curframe = frames[curFrameID];
	curframe.id = curFrameID;


	//curframe.id++;
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
	auto& curframe = frames.begin()->second;
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


	frames.erase(frames.begin());
}

void Server::addClient( Client* client)
{
	clients.push_back({ client , Physics::createEntity(world)});
}

void Server::addInputs(size_t id, size_t frameid, const Inputs & inputs)
{
	Frame* frame = &frames.begin()->second;
	if (frame->id < frameid)
		frame = &frames[frameid];

	frame->states[id].inputs = inputs;
	//auto& state = curframe.states[id];
	//state.inputs = inputs;
}

