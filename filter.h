#pragma once 
#include "common.h"
#include <list>

class Filter
{
	struct Frame
	{
		b2Vec2 pos = {0,0};
		float time = 0;
	};
	std::list<Frame> frames;
	b2Vec2 mTargetPos;

public:
	void input(b2Vec2 pos);
	void merge(b2Vec2 pos, float time);

	b2Vec2 update(float dtime, float interval);
	void process(float dtime, float interval);

};