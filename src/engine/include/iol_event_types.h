#ifndef IOLITE_EVENT_TYPES_H
#define IOLITE_EVENT_TYPES_H

#include "iol_definitions.h"
#include "iol_debug.h"
#include "iol_input_definitions.h"

#ifndef IOL_EVENT_MAX_SIZE
#define IOL_EVENT_MAX_SIZE 64
#endif // IOL_EVENT_MAX_SIZE

namespace iol
{
	enum WindowResizeType
	{
		WindowResizeType_Normal,
		WindowResizeType_Maximized,
		WindowResizeType_Minimized
	};

	struct IOL_API Event_WindowQuit
	{
	};

	struct IOL_API Event_WindowClose
	{
	};

	struct IOL_API Event_WindowResize
	{
		uint32 newWidth, newHeight;
		WindowResizeType resizeType;
	};

	struct IOL_API Event_WindowFocus
	{
	};

	struct IOL_API Event_WindowLostFocus
	{
	};

	struct IOL_API Event_WindowMoved
	{
		int32 x, y;
	};

	struct IOL_API Event_KeyPressed
	{
		ScanCode scanCode; // key based on the physical location on the keyboard. e.g. WASD for character movement. Use IOL_SCANCODE_TO_KEYCODE to get the virtual key code.
		bool repeated;
	};

	struct IOL_API Event_KeyReleased
	{
		ScanCode scanCode; // key based on the physical location on the keyboard. e.g. WASD for character movement. Use IOL_SCANCODE_TO_KEYCODE to get the virtual key code.
	};

	struct IOL_API Event_KeyTyped
	{
		uint16 character; // UTF-16
	};

	struct IOL_API  Event_MouseButtonPressed
	{
		MouseButton mouseButton;
		int32 x, y;
	};

	struct IOL_API Event_MouseButtonReleased
	{
		MouseButton mouseButton;
		int32 x, y;
	};

	struct IOL_API Event_MouseMoved
	{
		int32 x, y;
	};

	struct IOL_API Event_MouseScrolled
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

	union IOL_API EventData
	{
		uint8 raw[IOL_EVENT_MAX_SIZE - sizeof(uint32)];
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