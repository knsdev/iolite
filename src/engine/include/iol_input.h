#ifndef IOLITE_INPUT_H
#define IOLITE_INPUT_H

#include "iol_definitions.h"
#include "iol_input_definitions.h"

namespace iol
{
	struct Event;

	IOL_API void			input_CreateSystem();
	IOL_API void			input_DestroySystem();
	IOL_API void			input_UpdateSystem(float deltaTime);
	IOL_API void			input_HandleEvent(const Event& evt, void* userData);

	IOL_API KeyState		input_GetKeyState(ScanCode scanCode);
	IOL_API bool			input_IsKeyDown(ScanCode scanCode);

	IOL_API KeyState		input_GetMouseButtonState(MouseButton btn);
	IOL_API bool			input_IsMouseButtonDown(MouseButton btn);
	IOL_API glm::vec2		input_GetMousePosition();
	IOL_API glm::vec2		input_GetMouseScrollDelta();

	/*
	 * Internal
	 */
	MouseButton				input_ConvertMouseButton(uint8 mouseButton);
}

#endif // IOLITE_INPUT_H