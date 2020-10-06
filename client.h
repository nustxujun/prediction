#pragma once 

#include "common.h"
#include "box2d/box2d.h"
#include "filter.h"
#include "predicter.h"
#include <chrono>

class Server;
class Client
{
	friend class Server;
	static size_t index_gen ;
	

	size_t curFrameID;
	std::list<Frame> frames;

	struct Packet
	{
		size_t frameid;
		Inputs inputs;
		float time;
	};
	std::list<Packet> sendCache;
	float sendTime = 0;
	
	struct FrameMsg
	{
		float time = 0;
		Frame frame;

		FrameMsg(Frame f) :
			frame(f)
		{}
	};
	std::list<FrameMsg> receiveCache;


	Inputs records;



	Filter filter;
	Predicter predicter;

	size_t downloadlossCount = 0;
	size_t uploadlossCount = 0;

	struct
	{
		float up = 0;
		float down = 0;
	}latency;


public:
	

	size_t id = index_gen++;
	struct State
	{
		b2Vec2 vel;
		b2Vec2 pos;
	}state;

	using Controller = void (*)(b2Body*, const Inputs&);
	Controller controller = nullptr;

	Client();

	void visualize(float dtime, float interval);
	void sync();
	void send(Server* server);
	bool receive(const Frame& f);
	void update(float interval, Server* server);

	void addInput(int a, bool val);

	void setController(Controller ctrl) { controller = ctrl; }

	const b2Vec2& getPos() { return state.pos; }

	bool focus = false;
};