#ifndef __EVENT_CHANNEL_QUEUE_H__
#define __EVENT_CHANNEL_QUEUE_H__

#include "EventChannelQueueManager.h"

namespace WormHoles
{
	namespace Internal
	{
		template <typename EventType>
		class EventChannelQueue final : public Singleton<EventChannelQueue<EventType>>, public IEventChannelQueue
		{
		private:
			friend class Singleton<EventChannelQueue<EventType>>;

		private:
			std::mutex m_mutex;

			std::vector<std::function<void(const EventType&)>> m_handlers;

			std::vector<void*> m_originalPointers;

			std::vector<EventType> m_unsedMessages;

		private:
			EventChannelQueue(EventChannelQueue&& other) = delete;

			EventChannelQueue& operator=(EventChannelQueue&& other) = delete;

			EventChannelQueue(const EventChannelQueue& other) = delete;

			EventChannelQueue& operator=(const EventChannelQueue& other) = delete;

		private:
			EventChannelQueue()
				: Singleton<EventChannelQueue<EventType>>()
			{
				EventChannelQueueManager::GetInstance().Add(*this);
			}

			~EventChannelQueue()
			{
				EventChannelQueueManager::GetInstance().Remove(*this);
			}

		public:
			template <typename EventHandlerType>
			void Add(EventHandlerType& handler)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				m_handlers.emplace_back(CreateHandler(handler));
				m_originalPointers.emplace_back(&handler);
			}

			template <typename EventHandlerType>
			void Remove(EventHandlerType& handler)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				auto it = std::find(m_originalPointers.begin(), m_originalPointers.end(), &handler);
				if (it == m_originalPointers.end()) throw std::runtime_error("Tried to remove a handler that is not in the list");

				auto idx = (it - m_originalPointers.begin());

				m_handlers.erase(m_handlers.begin() + idx);
				m_originalPointers.erase(it);
			}

			void Broadcast(const EventType& message)
			{
				std::vector<std::function<void(const EventType&)>> currentHandlersCopy(m_handlers.size());

				{
					std::lock_guard<std::mutex> lock(m_mutex);
					currentHandlersCopy = m_handlers;
				}

				for (const auto& handler : currentHandlersCopy)
				{
					handler(message);
				}
			}

			void BroadcastWithDispatch(const EventType& message)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				m_unsedMessages.emplace_back(message);
			}

			void DispatchAll() override
			{
				std::vector<std::function<void(const EventType&)>> currentHandlersCopy(m_handlers.size());
				std::vector<EventType> currentUnsendMessages(m_unsedMessages.size());

				{
					std::lock_guard<std::mutex> lock(m_mutex);
					currentHandlersCopy = m_handlers;

					currentUnsendMessages = m_unsedMessages;
					m_unsedMessages.clear();
				}

				for (const auto& message : currentUnsendMessages)
				{
					for (const auto& handler : currentHandlersCopy)
					{
						handler(message);
					}
				}
			}

		private:
			template <typename EventHandlerType>
			static std::function<void(const EventType&)> CreateHandler(EventHandlerType& handler)
			{
				return [&handler](const EventType& message) { handler(message); };
			}
		};
	}
}

#endif