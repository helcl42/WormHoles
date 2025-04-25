#ifndef __ASYNC_TESTS_H__
#define __ASYNC_TESTS_H__

#include "Common.h"

#include <worm/EventChannel.h>

#include <thread>
#include <chrono>

TEST(PublishSubscribeTest, HandleAsyncEvent)
{
    ObjectWithHandler object;

    // Post an asynchronous event
    worm::EventChannel::Post(TestEvent{ "Async Message" }, worm::DispatchType::ASYNC);
    
    worm::EventChannel::DispatchAllAsync();

    // Verify the handler processed the async event
    EXPECT_EQ(object.GetMessages().size(), 1);
    EXPECT_EQ(object.GetMessages()[0], "Async Message");
}

TEST(PublishSubscribeTest, HandleMultipleAsyncEvents)
{
    ObjectWithHandler object;

    // Post multiple asynchronous events
    worm::EventChannel::Post(TestEvent{ "Async Message 1" }, worm::DispatchType::ASYNC);
    worm::EventChannel::Post(TestEvent{ "Async Message 2" }, worm::DispatchType::ASYNC);

    worm::EventChannel::DispatchAllAsync();

    // Verify the handler processed the async events
    EXPECT_EQ(object.GetMessages().size(), 2);
}

TEST(PublishSubscribeTest, HandleMultipleAsyncEvents2)
{
    ObjectWithHandler object;

    const uint32_t eventCount{ 1000 };

    auto getMessage = [](uint32_t i) {
        return (std::stringstream{} << "Async Message " << i).str();
    };

    // Post multiple asynchronous events
    for (uint32_t i = 0; i < eventCount; ++i) {
        TestEvent event{ getMessage(i) };
        worm::EventChannel::Post(event, worm::DispatchType::ASYNC);
    }

    worm::EventChannel::DispatchAllAsync();

    EXPECT_EQ(object.GetMessages().size(), eventCount);
    for (uint32_t i = 0; i < eventCount; ++i) {
        EXPECT_EQ(object.GetMessages()[i], getMessage(i));
    }
}

#endif