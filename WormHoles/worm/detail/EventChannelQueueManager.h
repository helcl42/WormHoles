#ifndef __EVENT_CHANNEL_QUEUE_MANAGER_H__
#define __EVENT_CHANNEL_QUEUE_MANAGER_H__

#include "IEventChannelQueue.h"
#include "Singleton.h"

#include <algorithm>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace worm::detail {
class EventChannelQueueManager final : public Singleton<EventChannelQueueManager> {
public:
    void Add(IEventChannelQueue& queue)
    {
        std::scoped_lock lock{ m_mutex };

        m_eventChannelQueues.emplace_back(&queue);
    }

    void Remove(IEventChannelQueue& queue)
    {
        std::scoped_lock lock{ m_mutex };

        auto it = std::find(m_eventChannelQueues.cbegin(), m_eventChannelQueues.cend(), &queue);
        m_eventChannelQueues.erase(it);
    }

    void DispatchAll()
    {
        std::shared_lock lock{ m_mutex };

        for (auto& queue : m_eventChannelQueues) {
            queue->DispatchAll();
        }
    }

private:
    EventChannelQueueManager() = default;

    ~EventChannelQueueManager() = default;

private:
    EventChannelQueueManager(EventChannelQueueManager&& other) = delete;

    EventChannelQueueManager& operator=(EventChannelQueueManager&& other) = delete;

    EventChannelQueueManager(const EventChannelQueueManager& other) = delete;

    EventChannelQueueManager& operator=(const EventChannelQueueManager& other) = delete;

private:
    friend class Singleton<EventChannelQueueManager>;

private:
    std::shared_mutex m_mutex;

    std::vector<IEventChannelQueue*> m_eventChannelQueues;
};
} // namespace worm::detail

#endif // !__EVENT_CHANNEL_QUEUE_MANAGER_H__
