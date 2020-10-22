#ifndef __EVENT_CHANNEL_H__
#define __EVENT_CHANNEL_H__

#include "internal/EventChannelQueue.h"

namespace worm {
enum class DispatchType {
    MAIN_THREAD,
    SYNC,
    ASYNC
};

class EventChannel final {
public:
    template <typename MessageType, typename EventHandlerType>
    static void Add(EventHandlerType& handler)
    {
        internal::EventChannelQueue<MessageType>::GetInstance().Add(handler);
    }

    template <typename MessageType, typename EventHandlerType>
    static void Remove(EventHandlerType& handler)
    {
        internal::EventChannelQueue<MessageType>::GetInstance().Remove(handler);
    }

    template <typename MessageType>
    static void Broadcast(const MessageType& message, const DispatchType dispatchType)
    {
        switch (dispatchType) {
        case DispatchType::ASYNC:
            internal::EventChannelQueue<MessageType>::GetInstance().BroadcastAsync(message);
            break;
        case DispatchType::MAIN_THREAD:
            internal::EventChannelQueue<MessageType>::GetInstance().BroadcastMainThread(message);
            break;
        default:
            internal::EventChannelQueue<MessageType>::GetInstance().Broadcast(message);
            break;
        }
    }

    static void DispatchAll()
    {
        internal::EventChannelQueueManager::GetInstance().BroadcastAll();
    }

private:
    EventChannel() = default;

    ~EventChannel() = default;

private:
    EventChannel(const EventChannel& other) = delete;

    EventChannel& operator=(const EventChannel& other) = delete;

    EventChannel(const EventChannel&& other) = delete;

    EventChannel& operator=(const EventChannel&& other) = delete;
};
} // namespace worm

#endif