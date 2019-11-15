#ifndef __EVENT_CHANNEL_QUEUE_MANAGER_H__
#define __EVENT_CHANNEL_QUEUE_MANAGER_H__

#include <algorithm>
#include <functional>
#include <mutex>
#include <vector>

#include "Singleton.h"

namespace WormHoles
{
	namespace Internal
	{
		class IEventChannelQueue
		{
		public:
			virtual void DispatchAll() = 0;

		public:
			virtual ~IEventChannelQueue() = default;
		};

		class EventChannelQueueManager final : public Singleton<EventChannelQueueManager>
		{
		private:
			friend class Singleton<EventChannelQueueManager>;

		private:
			std::mutex m_mutex;

			std::vector<IEventChannelQueue*> m_eventChannelQueues;

		private:
			EventChannelQueueManager(EventChannelQueueManager&& other) = delete;

			EventChannelQueueManager& operator=(EventChannelQueueManager&& other) = delete;

			EventChannelQueueManager(const EventChannelQueueManager& other) = delete;

			EventChannelQueueManager& operator=(const EventChannelQueueManager& other) = delete;

		private:
			EventChannelQueueManager() = default;

			~EventChannelQueueManager() = default;

		public:
			void Add(IEventChannelQueue& queue)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				m_eventChannelQueues.emplace_back(&queue);
			}

			void Remove(IEventChannelQueue& queue)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				auto it = std::find(m_eventChannelQueues.cbegin(), m_eventChannelQueues.cend(), &queue);
				m_eventChannelQueues.erase(it);
			}

			void BroadcastAll()
			{
				std::vector<IEventChannelQueue*> queues(m_eventChannelQueues.size());

				{
					std::lock_guard<std::mutex> lock(m_mutex);
					queues = m_eventChannelQueues;
				}

				for (auto& queue : queues)
				{
					queue->DispatchAll();
				}
			}
		};
	}
}

#endif // !__EVENT_CHANNEL_QUEUE_MANAGER_H__

