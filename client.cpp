#include  "client.h"
#include <algorithm>
#define GLFW_INCLUDE_NONE
#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "draw.h"
#include "server.h"
size_t Client::index_gen = 0;

Client::Client()
{
	predicter.filter = &filter;
	predicter.client = this;
}

void Client::send(Server* server)
{
	auto frameid = predicter.inputFromClient(records);

	static float timer = 0;

	//collect inputs 
	sendCache.push_back({ frameid,records });
	


	while (!sendCache.empty())
	{
		auto& packet = sendCache.front();
		// lost packet
		if (Common::lost(Common::uploadloss))
		{
			timer += packet.time;
			packet.time = 0;
			return;
		}

		// lag packet
		if (packet.time < Common::uploadlag)
			return;

		//record latency
		timer += packet.time;
		latency.up =  timer ;
		timer = 0;

		//success
		server->addInputs(id, packet.frameid, packet.inputs);
		sendCache.pop_front();
	}
}

bool Client::receive(const Frame& frame)
{
	receiveCache.push_back(frame);

	static float timer = 0;
	while (!receiveCache.empty())
	{
		auto& first = receiveCache.front();
		// lost packet
		if (Common::lost(Common::downloadloss))
		{
			timer += first.time;
			first.time = 0;
			break;
		}

		// lag packet
		if (first.time < Common::downloadlag)
		{
			break;
		}

		timer += first.time;
		latency.down = timer ;
		timer = 0;

		//success
		frames.push_back(first.frame);
		receiveCache.pop_front();
	}
	return true;
}
void Client::update(float interval, Server* server)
{
	if (Common::predict)
		predicter.curFrameID = Common::predictCount + server->getFrameID() - 2;
	else
		predicter.curFrameID = server->getFrameID() - 1;
	predicter.predict(interval);
	predicter.update();



	if (focus)
		Common::latency = latency.up + latency.down;


	for (auto& p : sendCache)
	{
		p.time += interval;
	}
	for (auto& p : receiveCache)
	{
		p.time += interval;
	}
}

void Client::sync()
{
	// process received frames
	for (auto& f : frames)
	{
		auto& self = f.states[id];

		predicter.inputFromServer(f.id, self.pos, self.vel);
		if (!Common::predict)
		{
			state.pos = self.pos;
			state.vel = self.vel;
			filter.input(self.pos);
		}
	}
	frames.clear();

}


void Client::addInput(int key, bool val)
{
	records[key] = val;
}

void Client::visualize(float dtime, float interval)
{
	sync();
	


	auto pos = state.pos;


	if (Common::visual)
		g_debugDraw.DrawCircle(state.pos, 1.0f, { 1,0,0,0 });


	if (Common::filter)
	{
		pos = filter.update(dtime, interval);
		g_debugDraw.DrawCircle(pos, 1.0f, { 0,1,0,1 });
	}


	if (Common::focus && focus)
	{
		if (Common::filter)
			g_camera.m_center = pos;
		else if (Common::visual)
			g_camera.m_center = state.pos;
		//Common::linearVel = state.vel;
	}


}
