#ifndef IOLITE_INPUT_H
#define IOLITE_INPUT_H

#include "iol_definitions.h"
#include "iol_input_definitions.h"

namespace iol
{
	struct Event;

	void           input_CreateSystem();
	void           input_DestroySystem();
	void           input_UpdateSystem(float deltaTime);
	void           input_HandleEvent(const Event& evt, void* userData);

	KeyState       input_GetKeyState(ScanCode scanCode);
	bool           input_IsKeyDown(ScanCode scanCode);

	KeyState       input_GetMouseButtonState(MouseButton btn);
	bool           input_IsMouseButtonDown(MouseButton btn);
	glm::vec2      input_GetMousePosition();
	glm::vec2      input_GetMouseScrollDelta();

	/*
	 * Internal
	 */
	MouseButton    input_ConvertMouseButton(uint8 mouseButton);
}

#endif // IOLITE_INPUT_H