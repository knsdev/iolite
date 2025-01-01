#ifndef IOLITE_ENGINE_H
#define IOLITE_ENGINE_H

#include "iol_definitions.h"

namespace iol
{
	class GraphicsSystem;
	class Application;

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

	namespace Engine
	{
		IOL_API void Run(const EngineParams& params, Application* app);
	}
}
	
#endif // IOLITE_ENGINE_H