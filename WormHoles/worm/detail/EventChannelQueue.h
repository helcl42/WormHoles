#ifndef __WH_EVENT_CHANNEL_QUEUE_H__
#define __WH_EVENT_CHANNEL_QUEUE_H__

#include "EventChannelQueueManager.h"
#include "RingBuffer.h"
#include "ThreadPool.h"

#include <functional>
#include <future>
#include <mutex>
#include <vector>

namespace worm::detail {
template <typename EventType>
class EventChannelQueue final : public Singleton<EventChannelQueue<EventType>>, public IEventChannelQueue {
public:
    template <typename EventHandlerType>
    void Add(EventHandlerType& handler)
    {
        std::scoped_lock lock{ m_mutex };

        m_handlers.emplace_back(CreateHandler(handler));
        m_originalPointers.emplace_back(&handler);
    }

    template <typename EventHandlerType>
    void Remove(EventHandlerType& handler)
    {
        std::scoped_lock lock{ m_mutex };

        const auto it{ std::find(m_originalPointers.begin(), m_originalPointers.end(), &handler) };
        if (it == m_originalPointers.end()) {
            throw std::runtime_error("Tried to remove a handler that is not in the list.");
        }

        const auto idx{ it - m_originalPointers.begin() };
        m_handlers.erase(m_handlers.begin() + idx);
        m_originalPointers.erase(it);
    }

    void Post(const EventType& message)
    {
        std::scoped_lock lock{ m_mutex };

        DispatchEvent(message);
    }

    void PostQueued(const EventType& message)
    {
        std::scoped_lock lock{ m_mutex };

        m_eventsToDeliver.Push(message);

        if (m_eventsToDeliver.IsFull()) {
            DispatchAllQueuedInternal();
        }
    }

    void PostAsync(const EventType& message)
    {
        std::scoped_lock lock{ m_asyncTasksMutex };

        m_asyncTasks.MovePush(std::move(m_threadPool.Enqueue([this, message]() {
            std::scoped_lock lock{ this->m_mutex };

            DispatchEvent(message);
        })));

        if (m_asyncTasks.IsFull()) {
            DispatchAllAsyncInternal();
        }
    }

    void DispatchAllQueued() override
    {
        std::scoped_lock lock{ m_mutex };

        DispatchAllQueuedInternal();
    }

    void DispatchAllAsync() override
    {
        std::scoped_lock lock{ m_asyncTasksMutex };

        DispatchAllAsyncInternal();
    }

private:
    void DispatchAllQueuedInternal()
    {
        while (!m_eventsToDeliver.IsEmpty()) {
            DispatchEvent(m_eventsToDeliver.Pop());
        }
    }

    void DispatchAllAsyncInternal()
    {
        while (!m_asyncTasks.IsEmpty()) {
            m_asyncTasks.MovePop().get();
        }
    }

private:
    EventChannelQueue()
        : Singleton<EventChannelQueue<EventType>>()
    {
        EventChannelQueueManager::Instance().Add(*this);
    }

    ~EventChannelQueue()
    {
        EventChannelQueueManager::Instance().Remove(*this);
    }

private:
    void DispatchEvent(const EventType& message)
    {
        for (size_t i = 0; i < m_handlers.size(); ++i) {
            const auto& handler{ m_handlers[i] };
            handler(message);
        }
    }

    template <typename EventHandlerType>
    static std::function<void(const EventType&)> CreateHandler(EventHandlerType& handler)
    {
        return [&handler](const EventType& message) { handler(message); };
    }

private:
    friend class Singleton<EventChannelQueue<EventType>>;

private:
    static const inline size_t MAX_ASYNC_TASK_COUNT{ 1024 };

    static const inline size_t MAX_QUEUED_MESSAGE_COUNT{ 1024 };

    static const inline size_t THREAD_POOL_THREAD_COUNT{ 1 };

    std::recursive_mutex m_mutex;

    std::vector<std::function<void(const EventType&)>> m_handlers;

    std::vector<void*> m_originalPointers;

    RingBuffer<EventType, MAX_QUEUED_MESSAGE_COUNT> m_eventsToDeliver;

    ThreadPool m_threadPool{ THREAD_POOL_THREAD_COUNT };

    RingBuffer<std::future<void>, MAX_QUEUED_MESSAGE_COUNT> m_asyncTasks;

    std::mutex m_asyncTasksMutex;
};
} // namespace worm::detail

#endif