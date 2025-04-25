#ifndef __WORM_DETAIL_EVENT_CHANNEL_QUEUE_MANAGER_TESTS_H__
#define __WORM_DETAIL_EVENT_CHANNEL_QUEUE_MANAGER_TESTS_H__

#include "../Common.h"

#include <worm/detail/EventChannelQueue.h>
#include <worm/detail/EventChannelQueueManager.h>

TEST(EventChannelQueueManagerTest, DispatchAllQueuedEvents)
{
    auto& queue = worm::detail::EventChannelQueue<TestEvent>::Instance();
    auto& manager = worm::detail::EventChannelQueueManager::Instance();

    MockHandler handler;
    queue.Add(handler);

    // Post events to the queue
    queue.PostQueued(TestEvent{ "Queued Message 1" });
    queue.PostQueued(TestEvent{ "Queued Message 2" });

    // Verify no messages are delivered yet
    EXPECT_TRUE(handler.GetMessages().empty());

    // Dispatch all queued events using the manager
    manager.DispatchAllQueued();

    // Verify the messages are delivered
    EXPECT_EQ(handler.GetMessages().size(), 2);
    EXPECT_EQ(handler.GetMessages()[0], "Queued Message 1");
    EXPECT_EQ(handler.GetMessages()[1], "Queued Message 2");

    queue.Remove(handler);
}

TEST(EventChannelQueueManagerTest, DispatchAllWithMultipleQueues)
{
    // queue1 and queue2 are exactly the same instance -> it is unique for each event type
    // so if you create a new event type, it will create a new instance
    // and you can have multiple queues for different event types

    auto& queue1 = worm::detail::EventChannelQueue<TestEvent>::Instance();
    auto& queue2 = worm::detail::EventChannelQueue<TestEvent>::Instance();

    EXPECT_EQ(&queue1, &queue2); // Both should point to the same memory address

    auto& manager = worm::detail::EventChannelQueueManager::Instance();

    MockHandler handler1, handler2;
    queue1.Add(handler1);
    queue2.Add(handler2);

    // Post events to both queues
    queue1.PostQueued(TestEvent{ "Queue1 Message" });
    queue2.PostQueued(TestEvent{ "Queue2 Message" });

    // Verify no messages are delivered yet
    EXPECT_TRUE(handler1.GetMessages().empty());
    EXPECT_TRUE(handler2.GetMessages().empty());

    // Dispatch all queued events using the manager
    manager.DispatchAll();

    // Verify the messages are delivered to the correct handlers
    EXPECT_EQ(handler1.GetMessages().size(), 2);
    EXPECT_EQ(handler1.GetMessages()[0], "Queue1 Message");
    EXPECT_EQ(handler1.GetMessages()[1], "Queue2 Message");

    EXPECT_EQ(handler2.GetMessages().size(), 2);
    EXPECT_EQ(handler2.GetMessages()[0], "Queue1 Message");
    EXPECT_EQ(handler2.GetMessages()[1], "Queue2 Message");

    queue1.Remove(handler1);
    queue2.Remove(handler2);
}

#endif