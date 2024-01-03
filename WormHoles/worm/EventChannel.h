#ifndef __WH_EVENT_CHANNEL_H__
#define __WH_EVENT_CHANNEL_H__

#include "detail/EventChannelQueue.h"

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
        detail::EventChannelQueue<MessageType>::Instance().Add(handler);
    }

    template <typename MessageType, typename EventHandlerType>
    static void Remove(EventHandlerType& handler)
    {
        detail::EventChannelQueue<MessageType>::Instance().Remove(handler);
    }

    template <typename MessageType>
    static void Post(const MessageType& message, const DispatchType dispatchType = DispatchType::SYNC)
    {
        switch (dispatchType) {
        case DispatchType::ASYNC:
            detail::EventChannelQueue<MessageType>::Instance().PostAsync(message);
            break;
        case DispatchType::QUEUED:
            detail::EventChannelQueue<MessageType>::Instance().PostQueued(message);
            break;
        default:
            detail::EventChannelQueue<MessageType>::Instance().Post(message);
            break;
        }
    }

    static void DispatchQueued()
    {
        detail::EventChannelQueueManager::Instance().DispatchAll();
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