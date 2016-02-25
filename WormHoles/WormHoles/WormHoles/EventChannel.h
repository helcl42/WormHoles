#ifndef EVENT_CHANNEL_H
#define EVENT_CHANNEL_H

#include "EventChannelQueue.h"

namespace WormHoles
{
	class EventChannel
	{
	public:
		template <typename MessageType, class EventHandlerType>
		static void Add(EventHandlerType* handler);

		template <typename MessageType, class EventHandlerType>
		static void Remove(EventHandlerType* handler);

		template <typename MessageType>
		static void Broadcast(const MessageType& message);
	};


	template <typename MessageType, class EventHandlerType>
	void EventChannel::Add(EventHandlerType* handler)
	{
		EventChannelQueue<MessageType>::GetInstance().Add(handler);
	}

	template <typename MessageType, class EventHandlerType>
	void EventChannel::Remove(EventHandlerType* handler)
	{
		EventChannelQueue<MessageType>::GetInstance().Remove(handler);
	}

	template <typename MessageType>
	void EventChannel::Broadcast(const MessageType& message)
	{
		EventChannelQueue<MessageType>::GetInstance().Broadcast(message);
	}
}

#endif