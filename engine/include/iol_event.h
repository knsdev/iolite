#ifndef IOLITE_EVENT_H
#define IOLITE_EVENT_H

#include "iol_event_types.h"
#include "iol_array.h"

namespace iol
{
	struct Event
	{
		uint32 type; // see enum EventType, you can use values that are not in this enum for custom events, then use data.raw and cast to your type*.
		EventData data;
	};

	struct EventSystemParam
	{
		size_t maxEventTypes = 100;
		size_t maxBufferedEvents = 512;
		size_t maxEventListeners = 512;
	};

	typedef void(*HandleEvent_t)(const Event& evt, void* userData);

	namespace event_system
	{
		void Create(const EventSystemParam& param);
		void Destroy();
		void Update(float deltaTime);
		void SendEvent(const Event& evt);
		void AddListener(uint32 eventType, HandleEvent_t callback, void* userData);
		void RemoveListener(uint32 eventType, HandleEvent_t callback, void* userData);
	};
}

#endif // IOLITE_EVENT_H