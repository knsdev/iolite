#ifndef IOLITE_ENGINE_H
#define IOLITE_ENGINE_H

#include "iol_definitions.h"

namespace iol
{
	class GraphicsSystem;

	struct EngineParams
	{
		int argc;
		char** argv;
		const char* windowTitle;
		uint32 windowWidth;
		uint32 windowHeight;
		uint32 fixedUPS;
		bool fullScreen;
		bool vsync;
		bool quitOnEscape;
	};

	class IOL_API Application
	{
	public:
		Application() {}
		virtual ~Application() {}

		virtual void Create(const EngineParams& params) = 0;
		virtual void Destroy() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void FixedUpdate(float deltaTime) = 0;

		GraphicsSystem* m_graphicsSystem;
	};

	namespace Engine
	{
		IOL_API void Run(const EngineParams& params, Application* app);
	}
}
	
#endif // IOLITE_ENGINE_H