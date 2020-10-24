#ifndef __EVENT_CHANNEL_H__
#define __EVENT_CHANNEL_H__

#include "internal/EventChannelQueue.h"

namespace worm {
enum class DispatchType {
    SYNC,
    ASYNC,
    QUEUED
};

class EventChannel final {
public:
    template <typename MessageType, typename EventHandlerType>
    static void Add(EventHandlerType& handler)
    {
        internal::EventChannelQueue<MessageType>::Instance().Add(handler);
    }

    template <typename MessageType, typename EventHandlerType>
    static void Remove(EventHandlerType& handler)
    {
        internal::EventChannelQueue<MessageType>::Instance().Remove(handler);
    }

    template <typename MessageType>
    static void Post(const MessageType& message, const DispatchType dispatchType = DispatchType::SYNC)
    {
        switch (dispatchType) {
        case DispatchType::ASYNC:
            internal::EventChannelQueue<MessageType>::Instance().PostAsync(message);
            break;
        case DispatchType::QUEUED:
            internal::EventChannelQueue<MessageType>::Instance().PostQueued(message);
            break;
        default:
            internal::EventChannelQueue<MessageType>::Instance().Post(message);
            break;
        }
    }

    static void DispatchQueued()
    {
        internal::EventChannelQueueManager::Instance().DispatchAll();
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