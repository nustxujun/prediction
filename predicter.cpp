#include "predicter.h"
#include "physics.h"
#include "client.h"

Predicter::Predicter()
{
	world = Physics::constructWorld();
	body = Physics::createEntity(world);
}

void Predicter::recover(b2Vec2 pos, b2Vec2 vel)
{

	frames.pop_front();

	//correct body transform from server
	auto ang = body->GetAngle();
	body->SetTransform(pos, ang);
	body->SetLinearVelocity(vel);

	// step to current frame,rebuild predicted frames
	std::list<Predicter::Frame> temp;
	while (!frames.empty())
	{
		auto& front = frames.front();

		if (client->controller)
			client->controller(body, front.inputs);// use inputs from sever

		world->Step(front.interval, 8, 3);

		temp.push_back({ front.id, body->GetPosition(), body->GetLinearVelocity(), front.inputs, front.interval });

		frames.pop_front();
	}

	//lerp pos
	if (temp.empty())
	{
		filter->merge(pos, 1.0);
	}
	else
	{
		auto& last = temp.back();
		filter->merge(last.pos, 1.0);
	}

	frames.swap(temp);


	//std::cout << "recover" << pos.x << "," << pos.y << std::endl;
	Common::log("%d recover: %.7f, , %.7f\n",client->id, pos.x, pos.y);
}


void Predicter::inputFromServer(int id, b2Vec2 pos, b2Vec2 vel)
{
	while (!frames.empty())
	{
		auto& first = frames.front();

		if (first.id == id)
		{
			// compare snapshot
			if (first.pos == pos && first.vel == vel)
			{
				return;
			}

			//snapshot failed
			recover(pos, vel);
			return;
		}
		else if (first.id > id)
		{
			//assert(0);
			return;
		}
		else
		{
			frames.pop_front();
		}
	}

}

size_t Predicter::inputFromClient(Inputs i)
{	
	inputs = std::move(i);
	return curFrameID + 1;
}

void Predicter::predict(float interval)
{
	if (client->controller)
		client->controller(body, inputs);

	world->Step(interval, 8, 3);

	frames.push_back({
		++curFrameID,
		body->GetPosition(),
		body->GetLinearVelocity(),
		std::move(inputs) ,
		interval });
}

void Predicter::update()
{
	if (frames.empty())
		return;

	if (!Common::predict)
		return;

	auto& last = frames.back();
	filter->input(last.pos);
	client->state.pos = last.pos;
	client->state.vel = last.vel;
}
