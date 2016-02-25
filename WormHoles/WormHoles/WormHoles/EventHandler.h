#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "EventChannel.h"

namespace WormHoles
{
	template <class EventHandlerType, class EventType>
	class EventHandler
	{
	private:
		EventHandlerType* m_handlerInstance;

	public:
		EventHandler(EventHandlerType* instance);

		virtual ~EventHandler();
	};


	template <class EventHandlerType, class EventType>
	EventHandler<EventHandlerType, EventType>::EventHandler(EventHandlerType* instance)
		: m_handlerInstance(instance)
	{
		EventChannel::Add<EventType>(m_handlerInstance);
	}

	template <class EventHandlerType, class EventType>
	EventHandler<EventHandlerType, EventType>::~EventHandler()
	{
		EventChannel::Remove<EventType>(m_handlerInstance);
	}
}

#endif