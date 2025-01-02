#include "iol_input.h"
#include "iol_event.h"
#include "iol_memory.h"
#include <SDL.h>

namespace iol
{
	struct InputSystem
	{
		bool keysDown[IOL_NUM_SCANCODES];
		bool keysDownPrev[IOL_NUM_SCANCODES];
		bool mouseButtonsDown[MouseButton_Count];
		bool mouseButtonsDownPrev[MouseButton_Count];
		glm::vec2 mousePosition;
		glm::vec2 mouseScrollDelta;
	};

	static InputSystem s_input;

	void input_UpdateKeyState(ScanCode scanCode, bool isDown);
	void input_UpdateMouseButtonState(MouseButton mouseButton, bool isDown);

	void input_CreateSystem()
	{
		EventSystem::AddListener(EventType_KeyPressed, input_HandleEvent, &s_input);
		EventSystem::AddListener(EventType_KeyReleased, input_HandleEvent, &s_input);
		EventSystem::AddListener(EventType_MouseButtonPressed, input_HandleEvent, &s_input);
		EventSystem::AddListener(EventType_MouseButtonReleased, input_HandleEvent, &s_input);
		EventSystem::AddListener(EventType_MouseMoved, input_HandleEvent, &s_input);
		EventSystem::AddListener(EventType_MouseScrolled, input_HandleEvent, &s_input);
	}

	void input_DestroySystem()
	{
		EventSystem::RemoveListener(EventType_KeyPressed, input_HandleEvent, &s_input);
		EventSystem::RemoveListener(EventType_KeyReleased, input_HandleEvent, &s_input);
		EventSystem::RemoveListener(EventType_MouseButtonPressed, input_HandleEvent, &s_input);
		EventSystem::RemoveListener(EventType_MouseButtonReleased, input_HandleEvent, &s_input);
		EventSystem::RemoveListener(EventType_MouseMoved, input_HandleEvent, &s_input);
		EventSystem::RemoveListener(EventType_MouseScrolled, input_HandleEvent, &s_input);
	}

	void input_UpdateSystem(float deltaTime)
	{
		memory_Copy(s_input.keysDownPrev, sizeof(s_input.keysDown), s_input.keysDown);
		memory_Copy(s_input.mouseButtonsDownPrev, sizeof(s_input.mouseButtonsDown), s_input.mouseButtonsDown);
		s_input.mouseScrollDelta = { 0, 0 };
	}

	void input_HandleEvent(const Event& evt, void* userData)
	{
		InputSystem* inputSystem = (InputSystem*)userData;

		switch (evt.type)
		{
		case EventType_KeyPressed:
			input_UpdateKeyState(evt.data.keyPressed.scanCode, true);
			break;

		case EventType_KeyReleased:
			input_UpdateKeyState(evt.data.keyReleased.scanCode, false);
			break;

		case EventType_MouseButtonPressed:
			input_UpdateMouseButtonState(evt.data.mouseButtonPressed.mouseButton, true);
			break;

		case EventType_MouseButtonReleased:
			input_UpdateMouseButtonState(evt.data.mouseButtonReleased.mouseButton, false);
			break;

		case EventType_MouseMoved:
			inputSystem->mousePosition[0] = evt.data.mouseMoved.x;
			inputSystem->mousePosition[1] = evt.data.mouseMoved.y;
			break;

		case EventType_MouseScrolled:
			inputSystem->mouseScrollDelta[0] += evt.data.mouseScrolled.dx;
			inputSystem->mouseScrollDelta[1] += evt.data.mouseScrolled.dy;
			break;

		default:
			break;
		}
	}

	enum KeyState input_GetKeyState(ScanCode scanCode)
	{
		KeyState keyState = KeyState_None;
		int32 i = (int32)scanCode;

		if (s_input.keysDown[i])
		{
			if (s_input.keysDownPrev[i])
			{
				keyState = KeyState_Holding;
			}
			else
			{
				keyState = KeyState_Pressed;
			}
		}
		else if (s_input.keysDownPrev[i])
		{
			keyState = KeyState_Released;
		}

		return keyState;
	}

	bool input_IsKeyDown(ScanCode scanCode)
	{
		return s_input.keysDown[(int32)scanCode];
	}

	KeyState input_GetMouseButtonState(MouseButton btn)
	{
		KeyState keyState = KeyState_None;
		int32 i = (int32)btn;

		if (s_input.mouseButtonsDown[i])
		{
			if (s_input.mouseButtonsDownPrev[i])
			{
				keyState = KeyState_Holding;
			}
			else
			{
				keyState = KeyState_Pressed;
			}
		}
		else if (s_input.mouseButtonsDownPrev[i])
		{
			keyState = KeyState_Released;
		}

		return keyState;
	}

	bool input_IsMouseButtonDown(MouseButton btn)
	{
		return s_input.mouseButtonsDown[(int32)btn];
	}

	glm::vec2 input_GetMousePosition()
	{
		return s_input.mousePosition;
	}

	glm::vec2 input_GetMouseScrollDelta()
	{
		return s_input.mouseScrollDelta;
	}

	MouseButton input_ConvertMouseButton(uint8 mouseButton)
	{
		switch (mouseButton)
		{
		case SDL_BUTTON_LEFT:
			return MouseButton_Left;
		case SDL_BUTTON_MIDDLE:
			return MouseButton_Middle;
		case SDL_BUTTON_RIGHT:
			return MouseButton_Right;
		case SDL_BUTTON_X1:
			return MouseButton_X1;
		case SDL_BUTTON_X2:
			return MouseButton_X2;
		default:
			return MouseButton_None;
		}
	}

	void input_UpdateKeyState(ScanCode scanCode, bool isDown)
	{
		s_input.keysDown[(int32)scanCode] = isDown;
	}

	void input_UpdateMouseButtonState(MouseButton mouseButton, bool isDown)
	{
		s_input.mouseButtonsDown[(int32)mouseButton] = isDown;
	}
}