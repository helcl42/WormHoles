#ifndef __WORM_EVENT_CHANNEL_TESTS_H__
#define __WORM_EVENT_CHANNEL_TESTS_H__

#include "Common.h"

#include <worm/EventChannel.h>

#include <chrono>

TEST(EventChannelTest, AddAndRemoveHandlers)
{
    MockHandler handler;
    worm::EventChannel::Add<TestEvent>(handler);

    // Post an event
    TestEvent event{ "Test Message" };
    worm::EventChannel::Post(event, worm::DispatchType::SYNC);

    // Verify the handler received the event
    EXPECT_EQ(handler.GetMessages().size(), 1);
    EXPECT_EQ(handler.GetMessages()[0], "Test Message");

    // Remove the handler
    worm::EventChannel::Remove<TestEvent>(handler);

    // Post another event
    worm::EventChannel::Post(TestEvent{ "Another Message" }, worm::DispatchType::SYNC);

    // Verify the handler did not receive the second event
    EXPECT_EQ(handler.GetMessages().size(), 1); // Still only one message
}

TEST(EventChannelTest, PostQueuedEvents)
{
    MockHandler handler;
    worm::EventChannel::Add<TestEvent>(handler);

    // Post events to the queue
    worm::EventChannel::Post(TestEvent{ "Queued Message 1" }, worm::DispatchType::QUEUED);
    worm::EventChannel::Post(TestEvent{ "Queued Message 2" }, worm::DispatchType::QUEUED);

    // Verify no messages are delivered yet
    EXPECT_TRUE(handler.GetMessages().empty());

    // Dispatch queued events
    worm::EventChannel::DispatchAllQueued();

    // Verify the messages are delivered
    EXPECT_EQ(handler.GetMessages().size(), 2);
    EXPECT_EQ(handler.GetMessages()[0], "Queued Message 1");
    EXPECT_EQ(handler.GetMessages()[1], "Queued Message 2");

    worm::EventChannel::Remove<TestEvent>(handler);
}

TEST(EventChannelTest, PostAsyncEvents)
{
    MockHandler handler;
    worm::EventChannel::Add<TestEvent>(handler);

    // Post an asynchronous event
    worm::EventChannel::Post(TestEvent{ "Async Message" }, worm::DispatchType::ASYNC);

    // Dispatch all async events
    worm::EventChannel::DispatchAllAsync();

    // Verify the message is delivered
    EXPECT_EQ(handler.GetMessages().size(), 1);
    EXPECT_EQ(handler.GetMessages()[0], "Async Message");

    worm::EventChannel::Remove<TestEvent>(handler);
}

TEST(EventChannelTest, RemoveNonexistentHandlerThrows)
{
    MockHandler handler;

    // Attempt to remove a handler that was never added
    EXPECT_THROW(worm::EventChannel::Remove<TestEvent>(handler), std::runtime_error);
}

#endif