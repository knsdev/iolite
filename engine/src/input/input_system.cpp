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

	namespace input
	{
		void UpdateKeyState(ScanCode scanCode, bool isDown);
		void UpdateMouseButtonState(MouseButton mouseButton, bool isDown);
	}

	void input::CreateSystem()
	{
		event_system::AddListener(EventType_KeyPressed, input::HandleEvent, &s_input);
		event_system::AddListener(EventType_KeyReleased, input::HandleEvent, &s_input);
		event_system::AddListener(EventType_MouseButtonPressed, input::HandleEvent, &s_input);
		event_system::AddListener(EventType_MouseButtonReleased, input::HandleEvent, &s_input);
		event_system::AddListener(EventType_MouseMoved, input::HandleEvent, &s_input);
		event_system::AddListener(EventType_MouseScrolled, input::HandleEvent, &s_input);
	}

	void input::DestroySystem()
	{
		event_system::RemoveListener(EventType_KeyPressed, input::HandleEvent, &s_input);
		event_system::RemoveListener(EventType_KeyReleased, input::HandleEvent, &s_input);
		event_system::RemoveListener(EventType_MouseButtonPressed, input::HandleEvent, &s_input);
		event_system::RemoveListener(EventType_MouseButtonReleased, input::HandleEvent, &s_input);
		event_system::RemoveListener(EventType_MouseMoved, input::HandleEvent, &s_input);
		event_system::RemoveListener(EventType_MouseScrolled, input::HandleEvent, &s_input);
	}

	void input::UpdateSystem(float deltaTime)
	{
		memory::Copy(s_input.keysDownPrev, sizeof(s_input.keysDown), s_input.keysDown);
		memory::Copy(s_input.mouseButtonsDownPrev, sizeof(s_input.mouseButtonsDown), s_input.mouseButtonsDown);
		s_input.mouseScrollDelta = { 0, 0 };
	}

	void input::HandleEvent(const Event& evt, void* userData)
	{
		InputSystem* inputSystem = (InputSystem*)userData;

		switch (evt.type)
		{
		case EventType_KeyPressed:
			UpdateKeyState(evt.data.keyPressed.scanCode, true);
			break;

		case EventType_KeyReleased:
			UpdateKeyState(evt.data.keyReleased.scanCode, false);
			break;

		case EventType_MouseButtonPressed:
			UpdateMouseButtonState(evt.data.mouseButtonPressed.mouseButton, true);
			break;

		case EventType_MouseButtonReleased:
			UpdateMouseButtonState(evt.data.mouseButtonReleased.mouseButton, false);
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

	enum KeyState input::GetKeyState(ScanCode scanCode)
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

	bool input::IsKeyDown(ScanCode scanCode)
	{
		return s_input.keysDown[(int32)scanCode];
	}

	KeyState input::GetMouseButtonState(MouseButton btn)
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

	bool input::IsMouseButtonDown(MouseButton btn)
	{
		return s_input.mouseButtonsDown[(int32)btn];
	}

	glm::vec2 input::GetMousePosition()
	{
		return s_input.mousePosition;
	}

	glm::vec2 input::GetMouseScrollDelta()
	{
		return s_input.mouseScrollDelta;
	}

	MouseButton input::ConvertMouseButton(uint8 mouseButton)
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

	void input::UpdateKeyState(ScanCode scanCode, bool isDown)
	{
		s_input.keysDown[(int32)scanCode] = isDown;
	}

	void input::UpdateMouseButtonState(MouseButton mouseButton, bool isDown)
	{
		s_input.mouseButtonsDown[(int32)mouseButton] = isDown;
	}
}