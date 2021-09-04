#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

#include "EventChannel.h"

namespace worm {
template <typename EventHandlerType, typename EventType>
class EventHandler final {
public:
    EventHandler(EventHandlerType& instance)
        : m_handlerInstance{ instance }
    {
        EventChannel::Add<EventType>(m_handlerInstance);
    }

    ~EventHandler()
    {
        EventChannel::Remove<EventType>(m_handlerInstance);
    }

public:
    EventHandler(const EventHandler& other) = default;

    EventHandler& operator=(const EventHandler& other) = default;

    EventHandler(EventHandler&& other) = default;

    EventHandler& operator=(EventHandler&& other) = default;

private:
    EventHandlerType& m_handlerInstance;
};
} // namespace worm

#endif