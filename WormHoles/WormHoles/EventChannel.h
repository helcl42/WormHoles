#ifndef __EVENT_CHANNEL_H__
#define __EVENT_CHANNEL_H__

#include "EventChannelQueue.h"

namespace WormHoles
{
	class EventChannel final
	{
	private:
		EventChannel() = default;

		~EventChannel() = default;

	private:
		EventChannel(const EventChannel& other) = delete;

		EventChannel& operator=(const EventChannel& other) = delete;

		EventChannel(const EventChannel&& other) = delete;

		EventChannel& operator=(const EventChannel&& other) = delete;

	public:
		template <typename MessageType, typename EventHandlerType>
		static void Add(EventHandlerType& handler)
		{
			EventChannelQueue<MessageType>::GetInstance().Add(handler);
		}

		template <typename MessageType, typename EventHandlerType>
		static void Remove(EventHandlerType& handler)
		{
			EventChannelQueue<MessageType>::GetInstance().Remove(handler);
		}

		template <typename MessageType>
		static void Broadcast(const MessageType& message)
		{
			EventChannelQueue<MessageType>::GetInstance().Broadcast(message);
		}
	};
}

#endif