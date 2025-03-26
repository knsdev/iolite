#ifndef IOLITE_INPUT_H
#define IOLITE_INPUT_H

#include "iol_definitions.h"
#include "iol_input_definitions.h"

namespace iol
{
	struct Event;

	namespace input
	{
		void           CreateSystem();
		void           DestroySystem();
		void           UpdateSystem(float deltaTime);
		void           HandleEvent(const Event& evt, void* userData);

		KeyState       GetKeyState(ScanCode scanCode);
		bool           IsKeyDown(ScanCode scanCode);

		KeyState       GetMouseButtonState(MouseButton btn);
		bool           IsMouseButtonDown(MouseButton btn);
		glm::vec2      GetMousePosition();
		glm::vec2      GetMouseScrollDelta();

		/*
		 * Internal
		 */
		MouseButton    ConvertMouseButton(uint8 mouseButton);
	}
}

#endif // IOLITE_INPUT_H