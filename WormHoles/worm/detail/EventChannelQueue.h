#ifndef __EVENT_CHANNEL_QUEUE_H__
#define __EVENT_CHANNEL_QUEUE_H__

#include "EventChannelQueueManager.h"
#include "ThreadPool.h"

namespace worm::detail {
template <typename EventType>
class EventChannelQueue final : public Singleton<EventChannelQueue<EventType> >, public IEventChannelQueue {
public:
    template <typename EventHandlerType>
    void Add(EventHandlerType& handler)
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };

        m_handlers.emplace_back(CreateHandler(handler));
        m_originalPointers.emplace_back(&handler);
    }

    template <typename EventHandlerType>
    void Remove(EventHandlerType& handler)
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };

        const auto it{ std::find(m_originalPointers.begin(), m_originalPointers.end(), &handler) };
        if (it == m_originalPointers.end()) {
            throw std::runtime_error("Tried to remove a handler that is not in the list");
        }

        const auto idx{ it - m_originalPointers.begin() };
        m_handlers.erase(m_handlers.begin() + idx);
        m_originalPointers.erase(it);
    }

    void Post(const EventType& message)
    {
        std::vector<std::function<void(const EventType&)> > currentHandlersCopy;

        {
            std::scoped_lock<std::mutex> lock{ m_mutex };

            currentHandlersCopy = m_handlers;
        }

        for (const auto& handler : currentHandlersCopy) {
            handler(message);
        }
    }

    void PostQueued(const EventType& message)
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };

        m_eventsToDeliver.emplace_back(message);
    }

    void PostAsync(const EventType& message)
    {
        m_threadPool.Enqueue([this, message]() {
            std::vector<std::function<void(const EventType&)> > currentHandlersCopy;

            {
                std::scoped_lock<std::mutex> lock{ this->m_mutex };
                currentHandlersCopy = m_handlers;
            }

            for (const auto& handler : m_handlers) {
                handler(message);
            }
        });
    }

    void DispatchAll() override
    {
        std::vector<std::function<void(const EventType&)> > currentHandlersCopy;
        std::vector<EventType> currentUnsendMessages;

        {
            std::scoped_lock<std::mutex> lock{ m_mutex };

            currentHandlersCopy = m_handlers;
            currentUnsendMessages = m_eventsToDeliver;
            m_eventsToDeliver.clear();
        }

        for (const auto& message : currentUnsendMessages) {
            for (const auto& handler : currentHandlersCopy) {
                handler(message);
            }
        }
    }

private:
    EventChannelQueue()
        : Singleton<EventChannelQueue<EventType> >()
    {
        EventChannelQueueManager::Instance().Add(*this);
    }

    ~EventChannelQueue()
    {
        EventChannelQueueManager::Instance().Remove(*this);
    }

private:
    EventChannelQueue(EventChannelQueue&& other) = delete;

    EventChannelQueue& operator=(EventChannelQueue&& other) = delete;

    EventChannelQueue(const EventChannelQueue& other) = delete;

    EventChannelQueue& operator=(const EventChannelQueue& other) = delete;

private:
    template <typename EventHandlerType>
    static std::function<void(const EventType&)> CreateHandler(EventHandlerType& handler)
    {
        return [&handler](const EventType& message) { handler(message); };
    }

private:
    friend class Singleton<EventChannelQueue<EventType> >;

private:
    std::mutex m_mutex;

    std::vector<std::function<void(const EventType&)> > m_handlers;

    std::vector<void*> m_originalPointers;

    std::vector<EventType> m_eventsToDeliver;

    ThreadPool m_threadPool{ 1 };
};
} // namespace worm::detail

#endif