#ifndef __COMMON_H__
#define __COMMON_H__

#include <worm/EventHandler.h>

#include <gtest/gtest.h>

#include <mutex>
#include <string>
#include <vector>

struct TestEvent {
    std::string message;
};

class ObjectWithHandler {
public:
    ObjectWithHandler() = default;
    ~ObjectWithHandler() = default;

    void operator()(const TestEvent& event)
    {
        std::scoped_lock lock{ m_mutex };

        m_messages.push_back(event.message);
    }

    std::vector<std::string> GetMessages() const
    {
        std::scoped_lock lock{ m_mutex };

        return m_messages;
    }

private:
    worm::EventHandler<ObjectWithHandler, TestEvent> m_eventHandler{ *this };

    std::vector<std::string> m_messages;

    mutable std::mutex m_mutex;
};

#endif