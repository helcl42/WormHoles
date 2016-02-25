#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "EventChannel.h"

namespace WormHoles
{
	template <typename EventHandlerType, typename EventType>
	class EventHandler
	{
	private:
		EventHandlerType* m_handlerInstance;

	public:
		EventHandler(EventHandlerType* instance);

		virtual ~EventHandler();
	};


	template <typename EventHandlerType, typename EventType>
	EventHandler<EventHandlerType, EventType>::EventHandler(EventHandlerType* instance)
		: m_handlerInstance(instance)
	{
		EventChannel::Add<EventType>(m_handlerInstance);
	}

	template <typename EventHandlerType, typename EventType>
	EventHandler<EventHandlerType, EventType>::~EventHandler()
	{
		EventChannel::Remove<EventType>(m_handlerInstance);
	}
}

#endif