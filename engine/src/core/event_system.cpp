#include "iol_event.h"

namespace iol
{
	struct EventListener
	{
		HandleEvent_t callback;
		void* userData;

		bool operator==(const EventListener& other) const
		{
			return callback == other.callback && userData == other.userData;
		}

		bool operator!=(const EventListener& other) const
		{
			return operator==(other);
		}
	};

	struct EventSystemData
	{
		EventSystemParam param;
		Array<EventListener>* listenerMap;
		size_t listenerMapSize;
		Array<Event> events;
	};

	static EventSystemData* eventSystem = nullptr;

	void event_system::Create(const EventSystemParam& param)
	{
		iol_assert(eventSystem == nullptr);

		eventSystem = new EventSystemData();

		eventSystem->param = param;
		eventSystem->events.Create(param.maxBufferedEvents);

		eventSystem->listenerMapSize = param.maxEventTypes;
		eventSystem->listenerMap = new Array<EventListener>[eventSystem->listenerMapSize];

		for (int i = 0; i < eventSystem->listenerMapSize; ++i)
		{
			eventSystem->listenerMap[i].Create(param.maxEventListeners);
		}
	}

	void event_system::Destroy()
	{
		iol_assert(eventSystem);

		for (int i = 0; i < eventSystem->listenerMapSize; ++i)
		{
			eventSystem->listenerMap[i].Destroy();
		}

		delete[] eventSystem->listenerMap;
		delete eventSystem;
		eventSystem = nullptr;
	}

	void event_system::Update(float deltaTime)
	{
		while (eventSystem->events.count > 0)
		{
			Event& evt = eventSystem->events[0];
			const Array<EventListener>& listeners = eventSystem->listenerMap[evt.type];

			for (size_t i = 0; i < listeners.count; ++i)
			{
				listeners[i].callback(evt, listeners[i].userData);
			}

			eventSystem->events.RemoveAt(0);
		}
	}

	void event_system::SendEvent(const Event& evt)
	{
		eventSystem->events.PushBack(evt);
	}

	void event_system::AddListener(uint32 eventType, HandleEvent_t callback, void* userData)
	{
		iol_assert(eventType < eventSystem->listenerMapSize);

		EventListener listener;
		listener.callback = callback;
		listener.userData = userData;

		eventSystem->listenerMap[eventType].PushBack(listener);
	}

	void event_system::RemoveListener(uint32 eventType, HandleEvent_t callback, void* userData)
	{
		EventListener listener;
		listener.callback = callback;
		listener.userData = userData;

		eventSystem->listenerMap[eventType].Remove(listener);
	}
}