#pragma once 
#include "client.h"

class Client;
class Server
{

	Frame curframe;


	struct Unit
	{
		Client* client;
		b2Body* body;
	};
	std::vector<Unit> clients;

	b2World* world;
public:

	Server(b2World* w);

	void preupdate();
	void update(float dtime);
	void postupdate(float dtime);

	void addClient( Client* client);
	void addInputs(size_t id, const Inputs& inputs);
	const Frame& getFrame();

};