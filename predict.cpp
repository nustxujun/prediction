
#include "test.h"
#include "client.h"
#include "server.h"
#include "imgui/imgui.h"
#include "control.h"

#include <array>
#include <chrono>

#define GLFW_INCLUDE_NONE
#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <windows.h>
#include <wingdi.h>

#pragma comment(lib, "Opengl32.lib")

float Common::uploadloss = 0.0f;
float Common::downloadloss = 0.0f;
float Common::uploadlag = 0;
float Common::downloadlag = 0;
int Common::fps = 30;
bool Common::box2d= true;
bool Common::visual = true;
bool Common::gravity = true;
bool Common::filter = true;
bool Common::predict = true;
int Common::predictCount = 1;
bool Common::focus = true;
ImGuiTextBuffer Common::logContext;
float Common::latency = 0;


b2Vec2 Common::linearVel;

class Predict: public Test
{
	using clock_t = std::chrono::high_resolution_clock;

	clock_t::time_point timer;
	
	struct Time
	{
		float duration;
		float interval;

		void operator += (float dtime)
		{
			duration += dtime;
		}

		bool costOneFrame()
		{
			if (duration < interval)
				return false;

			duration -= interval;
			return true;
		}

		operator float()const
		{
			return interval;
		}
	};

	Time constraint = { 0, 1.0f / 15.0f };

	std::vector<Client*> clients;
	Server* server;

	Client* inputReceiver;
	Client* cpuControl = 0;
public:

	Predict()
	{
		//auto& io = ImGui::GetIO();
		//auto font = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		//m_world->SetGravity(b2Vec2(0.0f, 0.0f));
		//ImGui::GetIO().FontDefault = font;
		server = new Server(m_world);


		clients.resize(3);
		for (auto& c : clients)
		{
			c = new Client();
			server->addClient(c);
		}

		inputReceiver = clients[0];
		inputReceiver->focus = true;
		clients[0]->setController(Control::move);
		
		if (clients.size() > 1)
		{
			clients[1]->setController(Control::move_without_gravity);
			cpuControl = clients[1];
		}
		timer = clock_t::now();

		setupGL();
	}

	void setupGL()
	{
		typedef void (APIENTRY* PFNWGLEXTSWAPCONTROLPROC) (int);
		typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);
		PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
		PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;
		
		typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);

		// this is pointer to function which returns pointer to string with list of all wgl extensions
		PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

		// determine pointer to wglGetExtensionsStringEXT function
		_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

		if (strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") == NULL)
		{
			// string was not found
			assert(0 && "cannot get swap control");
			return ;
		}

		// Extension is supported, init pointers.
		wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)wglGetProcAddress("wglSwapIntervalEXT");

		// this is another function from WGL_EXT_swap_control extension
		wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

		wglSwapIntervalEXT(0);
	
	}

	void gui(float dtime)
	{

		static float cache = 60.0f;
		cache = cache * 0.99f + (1.0f / dtime) * 0.01f;
		ImGui::Begin("control panel");
		//ImGui::Text("fps: %.3f", cache);
		ImGui::SetWindowSize({ 400,800 });
		ImGui::SliderFloat("upload loss", &Common::uploadloss, 0.0f, 1.0f);
		ImGui::SliderFloat("download loss", &Common::downloadloss, 0.0f, 1.0f);
		ImGui::SliderFloat("upload lag", &Common::uploadlag, 0.0f, 1.0f);
		ImGui::SliderFloat("download lag", &Common::downloadlag, 0.0f, 1.0f);
		ImGui::Text("latency %.3f", Common::latency);
		ImGui::SliderInt("limit fps", &Common::fps, 1, 60);

		ImGui::Checkbox("show box2d", &Common::box2d);
		ImGui::Checkbox("show visual", &Common::visual);
		ImGui::Checkbox("enable graivity", &Common::gravity);
		ImGui::Checkbox("enable filter", &Common::filter);
		ImGui::Checkbox("enable predict", &Common::predict);
		ImGui::SliderInt("predict count", &Common::predictCount, 1, 16);
		ImGui::Checkbox("focus on visual", &Common::focus);
		ImGui::Text("linear velocity: %.3f, %.3f", Common::linearVel.x, Common::linearVel.y);

		if (ImGui::Button("test1"))
		{
			Common::downloadlag = 0.1;
			Common::downloadloss = 0.1;
			Common::uploadlag = 0;
			Common::uploadloss = 0;
			Common::predictCount = 1;
		}

		ImGui::SameLine();


		if (ImGui::Button("test2"))
		{
			Common::downloadlag = 0.0;
			Common::downloadloss = 0.0;
			Common::uploadlag = 0.1;
			Common::uploadloss = 0.1;
			Common::predictCount = 3;
		}

		ImGui::SameLine();

		if (ImGui::Button("test3"))
		{
			Common::downloadlag = 0.1;
			Common::downloadloss = 0.05;
			Common::uploadlag = 0.1;
			Common::uploadloss = 0.05;
			Common::predictCount = 3;
		}


		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char* buf = Common::logContext.begin();
		const char* buf_end =  Common::logContext.end();
		{
			ImGui::TextUnformatted(buf, buf_end);
		}
		
		ImGui::PopStyleVar();

		ImGui::SetScrollHereY(1.0f);
		ImGui::EndChild();
		

		ImGui::End();
	}

	void cpu(float dtime)
	{
		if (!cpuControl)
			return;
		static float total = 0;
		total += dtime;
		auto v = std::signbit(std::sin(total));
		auto h = std::signbit(std::cos(total));


		cpuControl->addInput(GLFW_KEY_A,h);
		cpuControl->addInput(GLFW_KEY_W, v);
		cpuControl->addInput(GLFW_KEY_D,!h);
		cpuControl->addInput(GLFW_KEY_S,!v);

	}


	void background()
	{
		for (auto i = -100; i < 100; i+=5)
		{
			float s = std::sin(i) * 10 + 10;
			g_debugDraw.DrawSegment({ (float)i, -1 }, { (float)i, s }, { 0.5,0.5,0.5,1 });
		}
		g_debugDraw.DrawSegment({ -100,-1 }, { 100,-1 }, { 0.5,0.5,0.5,1 });
	}
	void Step(Settings& settings) override
	{

		constraint.interval = 1.0f / Common::fps;

		if (Common::gravity)
			m_world->SetGravity({ 0,-10 });
		else
			m_world->SetGravity({ 0,0 });

		auto cur = clock_t::now();
		auto dtime = (cur - timer).count() / 1000000000.0f;

		timer = cur;
		constraint += dtime;

		gui(dtime);


		g_debugDraw.SetFlags(1);

		while (constraint.costOneFrame())
		{
			cpu(constraint.interval);


			server->update(constraint.interval);
			for (auto& c : clients)
			{
				c->update(constraint.interval);
			}

		}

		for (auto& c : clients)
			c->visualize(dtime, constraint.interval);

		if (Common::box2d)
			m_world->DebugDraw();

		background();

		g_debugDraw.Flush();

		//Test::Step(settings);

	}

	void Keyboard(int key) override
	{
		inputReceiver->addInput(key, true);
	}

	void KeyboardUp(int key) override
	{
		inputReceiver->addInput(key, false);
	}

	static Test* Create()
	{
		return new Predict;
	}

};

static int testIndex = RegisterTest("Examples", "Predict", Predict::Create);
