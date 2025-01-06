#include "iolite.h"
#include "application.h"

using namespace iol;

int main(int argc, char* argv[])
{
	EngineParams params;
	params.argc = argc;
	params.argv = argv;
	params.windowTitle = "Test App";
	params.windowWidth = 1280;
	params.windowHeight = 800;
	params.fullScreen = false;
	params.vsync = true;
	params.fixedUPS = 60;
	params.quitOnEscape = true;

	{
		GameApplication app;
		Engine::Run(params, &app);
	}

#ifndef IOL_MASTER
	// Check for memory leaks
	iol_mem_log_allocations();
#endif
}

