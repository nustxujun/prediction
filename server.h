#pragma once 
#include "client.h"

class Client;
class Server
{
	size_t curFrameID = 0;
	std::map<size_t , Frame> frames;


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
	void addInputs(size_t id, size_t frameid,  const Inputs& inputs);

	size_t getFrameID()const { return curFrameID; }
};