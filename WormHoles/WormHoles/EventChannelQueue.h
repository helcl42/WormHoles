#ifndef __EVENT_CHANNEL_QUEUE_H__
#define __EVENT_CHANNEL_QUEUE_H__

#include <functional>
#include <mutex>
#include <vector>

#include "Singleton.h"

namespace WormHoles
{
	template <typename EventType>
	class EventChannelQueue final : public Singleton<EventChannelQueue<EventType>>
	{
	private:
		friend class Singleton<EventChannelQueue<EventType>>;

	private:
		std::mutex m_mutex;

		std::vector<std::function<void(const EventType&)>> m_handlers;

		std::vector<void*> m_originalPointers;

	private:
		EventChannelQueue(EventChannelQueue&& other) = delete;

		EventChannelQueue& operator=(EventChannelQueue&& other) = delete;

		EventChannelQueue(const EventChannelQueue& other) = delete;

		EventChannelQueue& operator=(const EventChannelQueue& other) = delete;

	private:
		EventChannelQueue() = default;

		~EventChannelQueue() = default;

	public:
		template <typename EventHandlerType>
		void Add(EventHandlerType* handler)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_handlers.emplace_back(CreateHandler(handler));
			m_originalPointers.emplace_back(handler);
		}

		template <typename EventHandlerType>
		void Remove(EventHandlerType* handler)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			auto it = std::find(m_originalPointers.begin(), m_originalPointers.end(), handler);
			if (it == m_originalPointers.end()) throw std::runtime_error("Tried to remove a handler that is not in the list");

			auto idx = (it - m_originalPointers.begin());

			m_handlers.erase(m_handlers.begin() + idx);
			m_originalPointers.erase(it);
		}

		void Broadcast(const EventType& message) // this shuld be const but it needs to be sycnhronised..
		{
			std::vector<std::function<void(const EventType&)>> localVector(m_handlers.size());

			{
				std::lock_guard<std::mutex> lock(m_mutex);
				localVector = m_handlers;
			}

			for (const auto& handler : localVector)
			{
				handler(message);
			}
		}

	private:
		template <typename EventHandlerType>
		static std::function<void(const EventType&)> CreateHandler(EventHandlerType* handler)
		{
			return [handler](const EventType& message) { (*handler)(message); };
		}
	};
}

#endif