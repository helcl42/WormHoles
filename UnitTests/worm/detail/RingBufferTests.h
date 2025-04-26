#ifndef __WORM_RING_BUFFER_TESTS_H__
#define __WORM_RING_BUFFER_TESTS_H__

#include "../Common.h"

#include <worm/detail/RingBuffer.h>

#include <future>

TEST(RingBufferTest, PushAndPop)
{
    worm::detail::RingBuffer<int, 5> buffer;

    // Push elements into the buffer
    buffer.Push(1);
    buffer.Push(2);
    buffer.Push(3);

    // Verify the buffer is not empty
    EXPECT_FALSE(buffer.IsEmpty());
    EXPECT_EQ(buffer.Size(), 3);

    // Pop elements and verify their values
    EXPECT_EQ(buffer.Pop(), 1);
    EXPECT_EQ(buffer.Pop(), 2);
    EXPECT_EQ(buffer.Pop(), 3);

    // Verify the buffer is empty
    EXPECT_TRUE(buffer.IsEmpty());
    EXPECT_EQ(buffer.Size(), 0);
}

TEST(RingBufferTest, Overflow)
{
    worm::detail::RingBuffer<int, 3> buffer;

    // Fill the buffer
    buffer.Push(1);
    buffer.Push(2);
    buffer.Push(3);

    // Verify the buffer is full
    EXPECT_TRUE(buffer.IsFull());

    // Attempt to push another element and expect an exception
    EXPECT_THROW(buffer.Push(4), std::runtime_error);
}

TEST(RingBufferTest, Underflow)
{
    worm::detail::RingBuffer<int, 3> buffer;

    // Verify the buffer is empty
    EXPECT_TRUE(buffer.IsEmpty());

    // Attempt to pop an element and expect an exception
    EXPECT_THROW(buffer.Pop(), std::runtime_error);
}

TEST(RingBufferTest, CircularBehavior)
{
    worm::detail::RingBuffer<int, 3> buffer;

    // Push elements into the buffer
    buffer.Push(1);
    buffer.Push(2);
    buffer.Push(3);

    // Pop one element
    EXPECT_EQ(buffer.Pop(), 1);

    // Push another element
    buffer.Push(4);

    // Verify the circular behavior
    EXPECT_EQ(buffer.Pop(), 2);
    EXPECT_EQ(buffer.Pop(), 3);
    EXPECT_EQ(buffer.Pop(), 4);

    // Verify the buffer is empty
    EXPECT_TRUE(buffer.IsEmpty());
}

TEST(RingBufferTest, NonCopyableType)
{
    worm::detail::RingBuffer<std::future<int>, 2> buffer;

    // Push a non-copyable type (std::future)
    buffer.MovePush(std::async(std::launch::async, []() { return 42; }));

    // Pop the future and verify the result
    auto future = buffer.MovePop();
    EXPECT_EQ(future.get(), 42);

    // Verify the buffer is empty
    EXPECT_TRUE(buffer.IsEmpty());
}

#endif