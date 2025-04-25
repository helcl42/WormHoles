#ifndef __WORM_DETAIL_EVENT_CHANNEL_QUEUE_TESTS_H__
#define __WORM_DETAIL_EVENT_CHANNEL_QUEUE_TESTS_H__

#include "../Common.h"

#include <worm/detail/EventChannelQueue.h>

#include <chrono>

TEST(EventChannelQueueTest, AddAndRemoveHandlers)
{
    auto& queue = worm::detail::EventChannelQueue<TestEvent>::Instance();

    MockHandler handler;

    // Add handler
    queue.Add(handler);

    // Post an event
    TestEvent event{ "Test Message" };
    queue.Post(event);

    // Verify the handler received the event
    EXPECT_EQ(handler.GetMessages().size(), 1);
    EXPECT_EQ(handler.GetMessages()[0], "Test Message");

    // Remove handler
    queue.Remove(handler);

    // Post another event
    queue.Post(TestEvent{ "Another Message" });

    // Verify the handler did not receive the second event
    EXPECT_EQ(handler.GetMessages().size(), 1); // Still only one message
}

TEST(EventChannelQueueTest, PostQueuedEvents)
{
    auto& queue = worm::detail::EventChannelQueue<TestEvent>::Instance();

    MockHandler handler;

    queue.Add(handler);

    // Post events to the queue
    queue.PostQueued(TestEvent{ "Queued Message 1" });
    queue.PostQueued(TestEvent{ "Queued Message 2" });

    // Verify no messages are delivered yet
    EXPECT_TRUE(handler.GetMessages().empty());

    // Dispatch queued events
    queue.DispatchAllQueued();

    // Verify the messages are delivered
    EXPECT_EQ(handler.GetMessages().size(), 2);
    EXPECT_EQ(handler.GetMessages()[0], "Queued Message 1");
    EXPECT_EQ(handler.GetMessages()[1], "Queued Message 2");

    queue.Remove(handler);
}

TEST(EventChannelQueueTest, PostAsyncEvents)
{
    auto& queue = worm::detail::EventChannelQueue<TestEvent>::Instance();

    MockHandler handler;

    queue.Add(handler);

    // Post an asynchronous event
    queue.PostAsync(TestEvent{ "Async Message" });

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Give some time for async processing

    queue.DispatchAllAsync();

    // Verify the message is delivered
    EXPECT_EQ(handler.GetMessages().size(), 1);
    EXPECT_EQ(handler.GetMessages()[0], "Async Message");

    queue.Remove(handler);
}

TEST(EventChannelQueueTest, RemoveNonexistentHandlerThrows)
{
    auto& queue = worm::detail::EventChannelQueue<TestEvent>::Instance();
    MockHandler handler;

    // Attempt to remove a handler that was never added
    EXPECT_THROW(queue.Remove(handler), std::runtime_error);
}

#endif