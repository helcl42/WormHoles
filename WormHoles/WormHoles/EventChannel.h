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
			Internal::EventChannelQueue<MessageType>::GetInstance().Add(handler);
		}

		template <typename MessageType, typename EventHandlerType>
		static void Remove(EventHandlerType& handler)
		{
			Internal::EventChannelQueue<MessageType>::GetInstance().Remove(handler);
		}

		// Should I add new function PostToDispatch instead of enum ??
		template <typename MessageType>
		static void Broadcast(const MessageType& message)
		{
			Internal::EventChannelQueue<MessageType>::GetInstance().Broadcast(message);
		}

		template <typename MessageType>
		static void BroadcastWithDispatch(const MessageType& message)
		{
			Internal::EventChannelQueue<MessageType>::GetInstance().BroadcastWithDispatch(message);
		}

		// rename it BroadcastDispatched
		static void DispatchAll()
		{
			Internal::EventChannelQueueManager::GetInstance().BroadcastAll();
		}
	};
}

#endif