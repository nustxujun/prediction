#include "filter.h"
#include <algorithm>


const size_t maxsize = 16;

void Filter::input(b2Vec2 pos)
{
	if (frames.size() == maxsize)
		frames.pop_front();


	frames.push_back({ pos });

}

void Filter::merge(b2Vec2 pos, float time)
{
	frames.clear();


	frames.push_back({ mTargetPos });
	auto mid = mTargetPos + pos;
	mid *= 0.5f;
	frames.push_back({ mid});
	frames.push_back({ pos });

}

b2Vec2 Filter::update(float dtime, float interval)
{

	process(dtime, interval);
	
	return mTargetPos;
}

void Filter::process(float dtime, float interval)
{
	if (frames.size() <= 1)
		return ;

	int framecount = frames.size();

	auto scale = (float)std::max(0, framecount - 3) / (float)maxsize  + 1.0f;

	auto& first = frames.front();
	first.time += dtime * scale;


	auto lerp = [](auto a, auto b, auto t) 
	{
		a *= (1 - t);
		b *= t;
		return a + b;
	};

	auto diff = first.time - interval;
	if (diff > 0)
	{
		frames.pop_front();
		process(diff, interval);
		return;
	}

	auto next = ++frames.begin();
	auto t = std::min(first.time / interval, 1.0f);
	mTargetPos = lerp(first.pos, next->pos, t);


}
