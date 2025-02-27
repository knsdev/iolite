#ifndef IOLITE_EVENT_TYPES_H
#define IOLITE_EVENT_TYPES_H

#include "iol_definitions.h"
#include "iol_debug.h"
#include "iol_input_definitions.h"

namespace iol
{
	enum WindowResizeType
	{
		WindowResizeType_Normal,
		WindowResizeType_Maximized,
		WindowResizeType_Minimized
	};

	struct Event_WindowQuit
	{
	};

	struct Event_WindowClose
	{
	};

	struct Event_WindowResize
	{
		uint32 newWidth, newHeight;
		WindowResizeType resizeType;
	};

	struct Event_WindowFocus
	{
	};

	struct Event_WindowLostFocus
	{
	};

	struct Event_WindowMoved
	{
		int32 x, y;
	};

	struct Event_KeyPressed
	{
		ScanCode scanCode; // key based on the physical location on the keyboard. e.g. WASD for character movement. Use IOL_SCANCODE_TO_KEYCODE to get the virtual key code.
		bool repeated;
	};

	struct Event_KeyReleased
	{
		ScanCode scanCode; // key based on the physical location on the keyboard. e.g. WASD for character movement. Use IOL_SCANCODE_TO_KEYCODE to get the virtual key code.
	};

	struct Event_KeyTyped
	{
		uint16 character; // UTF-16
	};

	struct  Event_MouseButtonPressed
	{
		MouseButton mouseButton;
		int32 x, y;
	};

	struct Event_MouseButtonReleased
	{
		MouseButton mouseButton;
		int32 x, y;
	};

	struct Event_MouseMoved
	{
		int32 x, y;
	};

	struct Event_MouseScrolled
	{
		int32 dx, dy;
	};

	enum EventType
	{
		EventType_None = -1,

		EventType_WindowQuit,
		EventType_WindowClose,
		EventType_WindowResize,
		EventType_WindowFocus,
		EventType_WindowLostFocus,
		EventType_WindowMoved,

		EventType_KeyPressed,
		EventType_KeyReleased,
		EventType_KeyTyped,

		EventType_MouseButtonPressed,
		EventType_MouseButtonReleased,
		EventType_MouseMoved,
		EventType_MouseScrolled,

		EventType_Count
	};

	union EventData
	{
		uint8 raw[64];
		Event_WindowQuit windowQuit;
		Event_WindowClose windowClose;
		Event_WindowResize windowResize;
		Event_WindowFocus windowFocus;
		Event_WindowLostFocus windowLostFocus;
		Event_WindowMoved windowMoved;
		Event_KeyPressed keyPressed;
		Event_KeyReleased keyReleased;
		Event_KeyTyped keyTyped;
		Event_MouseButtonPressed mouseButtonPressed;
		Event_MouseButtonReleased mouseButtonReleased;
		Event_MouseMoved mouseMoved;
		Event_MouseScrolled mouseScrolled;
	};
}

#endif // IOLITE_EVENT_TYPES_H