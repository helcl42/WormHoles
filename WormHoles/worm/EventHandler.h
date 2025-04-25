#ifndef __WH_EVENT_HANDLER_H__
#define __WH_EVENT_HANDLER_H__

#include "EventChannel.h"

namespace worm {
template <typename EventHandlerType, typename EventType>
class EventHandler final {
public:
    EventHandler(EventHandlerType& instance)
        : m_handlerInstance{ instance }
    {
        EventChannel::Add<EventType>(*this);
    }

    ~EventHandler()
    {
        EventChannel::Remove<EventType>(*this);
    }

public:
    EventHandler(const EventHandler& other) = delete;

    EventHandler& operator=(const EventHandler& other) = delete;

    EventHandler(EventHandler&& other) = default;

    EventHandler& operator=(EventHandler&& other) = default;

public:
    void operator()(const EventType& message)
    {
        m_handlerInstance(message);
    }

private:
    EventHandlerType& m_handlerInstance;
};
} // namespace worm

#endif