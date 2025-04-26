#ifndef __WH_RING_BUFFER_H__
#define __WH_RING_BUFFER_H__

#include <array>
#include <stdexcept>

namespace worm::detail {

template <typename EventType, size_t BufferSize = 1024>
class RingBuffer {
public:
    void Push(const EventType& item)
    {
        if (IsFull()) {
            throw std::runtime_error("RingBuffer overflow");
        }
        m_buffer[m_tail] = item;
        m_tail = (m_tail + 1) % BufferSize;
        ++m_size;
    }

    void MovePush(EventType&& item)
    {
        if (IsFull()) {
            throw std::runtime_error("RingBuffer overflow");
        }
        m_buffer[m_tail] = std::move(item);
        m_tail = (m_tail + 1) % BufferSize;
        ++m_size;
    }

    EventType Pop()
    {
        if (IsEmpty()) {
            throw std::runtime_error("RingBuffer underflow");
        }
        EventType item = m_buffer[m_head];
        m_head = (m_head + 1) % BufferSize;
        --m_size;
        return item;
    }

    EventType&& MovePop()
    {
        if (IsEmpty()) {
            throw std::runtime_error("RingBuffer underflow");
        }
        EventType&& item = std::move(m_buffer[m_head]);
        m_head = (m_head + 1) % BufferSize;
        --m_size;
        return std::move(item);
    }

    bool IsEmpty() const
    {
        return m_size == 0;
    }

    bool IsFull() const
    {
        return m_size == BufferSize;
    }

    size_t Size() const
    {
        return m_size;
    }

    void Clear()
    {
        m_head = 0;
        m_tail = 0;
        m_size = 0;
    }

private:
    std::array<EventType, BufferSize> m_buffer{};
    size_t m_head{ 0 };
    size_t m_tail{ 0 };
    size_t m_size{ 0 };
};

} // namespace worm::detail

#endif