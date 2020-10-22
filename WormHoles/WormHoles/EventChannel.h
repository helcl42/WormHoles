#ifndef __EVENT_CHANNEL_H__
#define __EVENT_CHANNEL_H__

#include "EventChannelQueue.h"

namespace WormHoles
{
	enum class DispatchType {
		MAIN_THREAD,
		SYNC,
		ASYNC
	};

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

		template <typename MessageType>
		static void Broadcast(const MessageType& message, const DispatchType dispatchType)
		{
			switch (dispatchType)
			{
			case DispatchType::ASYNC:
				Internal::EventChannelQueue<MessageType>::GetInstance().BroadcastAsync(message);
				break;
			case DispatchType::MAIN_THREAD:
				Internal::EventChannelQueue<MessageType>::GetInstance().BroadcastMainThread(message);
				break;
			default:
				Internal::EventChannelQueue<MessageType>::GetInstance().Broadcast(message);
				break;
			}
		}

		// rename it BroadcastDispatched
		static void DispatchAll()
		{
			Internal::EventChannelQueueManager::GetInstance().BroadcastAll();
		}
	};
}

#endif