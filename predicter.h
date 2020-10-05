#pragma once 

#include "common.h"
#include "filter.h"
class Client;
class Predicter
{
	
	struct Frame
	{
		size_t id;
		b2Vec2 pos;
		b2Vec2 vel;
		Inputs inputs;
		float interval;
	};

	size_t curFrameID = 0;
	std::list<Frame> frames;
	Inputs inputs;

	b2World* world;
	b2Body* body;
public:
	Filter* filter = 0;
	Client* client = 0;
	
	Predicter();
	void inputFromServer(int id, b2Vec2 pos, b2Vec2 vel);
	void inputFromClient(Inputs inputs);
	void predict(float interval);

	void update();

	void recover( b2Vec2 pos, b2Vec2 vel);
	void clearFrames() { frames.clear(); }
};