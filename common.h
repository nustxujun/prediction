#pragma once 

#include <vector>
#include <map>
#include <list>
#include <iostream>
#include "box2d/box2d.h"
#include "imgui/imgui.h"
#include <chrono>
using Inputs = std::map<int, int>;

struct Frame
{
	size_t id = 0;
	struct State
	{
		Inputs inputs;
		b2Vec2 pos;
		b2Vec2 vel;
	};
	std::map<size_t, State> states;
};

struct Common
{
	static float uploadloss ;
	static float downloadloss;
	static float uploadlag;
	static float downloadlag;

	static int fps;
	static float stun;

	static bool box2d;
	static bool visual;
	static bool gravity;
	static bool filter;
	static bool predict;
	static int predictCount;
	static bool focus;
	static ImGuiTextBuffer logContext;

	static b2Vec2 linearVel;
	static float latency;

	static void log(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		logContext.appendfv(fmt, args);
		va_end(args);
	}

	static bool lost(float loss, size_t count)
	{
		return int( loss * count);

	}

	static bool lost(float loss)
	{
		auto r = rand() % 100;
		return r < int(100 * loss);

	}
};

