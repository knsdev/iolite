#include "iol_engine.h"
#include "iol_debug.h"
#include "iol_graphics.h"
#include "iol_memory.h"
#include "iol_event.h"
#include "iol_input.h"
#include "iol_core.h"
#include "iol_application.h"
#include <SDL.h>
#include <thread>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace iol
{
	struct EngineData
	{
		EngineParams params;
		SDL_Window* window;
		GraphicsSystem* graphicsSystem;
		bool quit;
	};

	static EngineData s_engine;

	namespace engine
	{
		void PollEvents(bool* quit);
		void OnKeyPressed(const Event& evt, void* userData);
	}

	void engine::Run(const EngineParams& params, Application* app)
	{
		s_engine.params = params;

		int result = SDL_Init(SDL_INIT_EVERYTHING);

		if (result != 0)
		{
			iol_log_error("Failed to init SDL! result: %d", result);
			return;
		}

		EventSystemParam eventSystemParam;
		event_system::Create(eventSystemParam);

		input::CreateSystem();

		uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

		if (params.fullScreen)
			windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		s_engine.window = SDL_CreateWindow(params.windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, params.windowWidth, params.windowHeight, windowFlags);

		GraphicsSystemParam graphicsParam;
		graphicsParam.pWindow = s_engine.window;
		graphicsParam.vsync = params.vsync;
		SDL_GetWindowSize(s_engine.window, (int*)&graphicsParam.screenWidth, (int*)&graphicsParam.screenHeight);

		app->m_graphicsSystem = s_engine.graphicsSystem = iol_new(GraphicsSystem);

		if (!s_engine.graphicsSystem->Create(graphicsParam))
		{
			iol_delete(s_engine.graphicsSystem);
			return;
		}

		event_system::AddListener(EventType_KeyPressed, OnKeyPressed, &s_engine);

		app->Create(params);

		//----------------------------------
		// Game Loop Start
		//----------------------------------

		s_engine.quit = false;
		double fixedTimeStep = 1.0 / params.fixedUPS;
		double timeFrameStart = core::GetCurrentTimeSeconds();
		float dtAccum = 0.0f;

		while (!s_engine.quit)
		{
			double lastTime = timeFrameStart;
			timeFrameStart = core::GetCurrentTimeSeconds();
			float dt = (float)(timeFrameStart - lastTime);
			dtAccum += dt;

			input::UpdateSystem(dt);
			PollEvents(&s_engine.quit);
			event_system::Update(dt);

			app->Update(dt);

			if (dtAccum > fixedTimeStep)
			{
				float dtFixed = (float)fixedTimeStep;
				app->FixedUpdate(dtFixed);
				dtAccum -= dtFixed;
			}

			std::chrono::duration<uint32, std::milli> dur(1u);
			std::this_thread::sleep_for(dur);
		}

		//----------------------------------
		// Shutdown
		//----------------------------------

		app->Destroy();

		input::DestroySystem();
		s_engine.graphicsSystem->Destroy();
		iol_delete(s_engine.graphicsSystem);
		SDL_DestroyWindow(s_engine.window);
		SDL_Quit();
		event_system::Destroy();
	}

	void engine::PollEvents(bool* quit)
	{
		SDL_Event evtSDL;
		Event evt;

		while (SDL_PollEvent(&evtSDL))
		{
			ImGui_ImplSDL2_ProcessEvent(&evtSDL);

			const ImGuiIO& io = ImGui::GetIO();

			switch (evtSDL.type)
			{
			case SDL_KEYDOWN:
				if (io.WantCaptureKeyboard)
					return;

				evt.type = EventType_KeyPressed;
				evt.data.keyPressed.scanCode = (ScanCode)evtSDL.key.keysym.scancode;
				evt.data.keyPressed.repeated = evtSDL.key.repeat != 0;
				event_system::SendEvent(evt);
				break;

			case SDL_KEYUP:
				evt.type = EventType_KeyReleased;
				evt.data.keyReleased.scanCode = (ScanCode)evtSDL.key.keysym.scancode;
				event_system::SendEvent(evt);
				break;

			case SDL_MOUSEMOTION:
				if (io.WantCaptureMouse)
					return;

				evt.type = EventType_MouseMoved;
				evt.data.mouseMoved.x = evtSDL.motion.x;
				evt.data.mouseMoved.y = evtSDL.motion.y;
				event_system::SendEvent(evt);
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (io.WantCaptureMouse)
					return;

				ImGui::SetWindowFocus(nullptr);
				ImGui::ClearActiveID();

				evt.type = EventType_MouseButtonPressed;
				evt.data.mouseButtonPressed.mouseButton = input::ConvertMouseButton(evtSDL.button.button);
				evt.data.mouseButtonPressed.x = evtSDL.button.x;
				evt.data.mouseButtonPressed.y = evtSDL.button.y;
				event_system::SendEvent(evt);
				break;

			case SDL_MOUSEBUTTONUP:
				evt.type = EventType_MouseButtonReleased;
				evt.data.mouseButtonReleased.mouseButton = input::ConvertMouseButton(evtSDL.button.button);
				evt.data.mouseButtonReleased.x = evtSDL.button.x;
				evt.data.mouseButtonReleased.y = evtSDL.button.y;
				event_system::SendEvent(evt);
				break;

			case SDL_MOUSEWHEEL:
				if (io.WantCaptureMouse)
					return;

				evt.type = EventType_MouseScrolled;
				evt.data.mouseScrolled.dx = evtSDL.wheel.x;
				evt.data.mouseScrolled.dy = evtSDL.wheel.y;
				event_system::SendEvent(evt);
				break;

			case SDL_WINDOWEVENT:
				switch (evtSDL.window.event)
				{
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					evt.type = EventType_WindowResize;
					evt.data.windowResize.resizeType = WindowResizeType_Normal;
					evt.data.windowResize.newWidth = evtSDL.window.data1;
					evt.data.windowResize.newHeight = evtSDL.window.data2;
					event_system::SendEvent(evt);
					break;
				}
				break;

			case SDL_QUIT:
				*quit = true;
				break;

			default:
				break;
			}
		}
	}

	void engine::OnKeyPressed(const Event& evt, void* userData)
	{
		EngineData* engine = (EngineData*)userData;

		if (evt.data.keyPressed.scanCode == IOL_SCANCODE_ESCAPE && engine->params.quitOnEscape)
		{
			engine->quit = true;
		}
	}
}