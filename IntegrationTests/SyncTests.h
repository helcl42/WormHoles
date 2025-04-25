#ifndef __WORM_SYNC_TESTS_H__
#define __WORM_SYNC_TESTS_H__

#include "Common.h"

#include <worm/EventChannel.h>

#include <sstream>

TEST(PublishSubscribeTest, HandleSingleEvent)
{
    ObjectWithHandler object;

    // Post a test event
    TestEvent event{ "Test Message" };
    worm::EventChannel::Post(event, worm::DispatchType::SYNC);

    // Verify the handler processed the event
    EXPECT_EQ(object.GetMessages().size(), 1);
    EXPECT_EQ(object.GetMessages()[0], "Test Message");
}

TEST(PublishSubscribeTest, HandleMultipleEvents)
{
    ObjectWithHandler object;

    // Post multiple test events
    TestEvent event1{ "Message 1" };
    TestEvent event2{ "Message 2" };
    worm::EventChannel::Post(event1, worm::DispatchType::SYNC);
    worm::EventChannel::Post(event2, worm::DispatchType::SYNC);

    // Verify the handler processed all events
    EXPECT_EQ(object.GetMessages().size(), 2);
    EXPECT_EQ(object.GetMessages()[0], "Message 1");
    EXPECT_EQ(object.GetMessages()[1], "Message 2");
}

TEST(PublishSubscribeTest, HandleMultipleEvents2)
{
    ObjectWithHandler object;

    const uint32_t eventCount{ 1000 };

    auto getMessage = [](uint32_t i) {
        return (std::stringstream{} << "Message " << i).str();
    };

    // Post multiple test events
    for (uint32_t i = 0; i < eventCount; ++i) {
        TestEvent event{ getMessage(i) };
        worm::EventChannel::Post(event, worm::DispatchType::SYNC);
    }

    EXPECT_EQ(object.GetMessages().size(), eventCount);
    for (uint32_t i = 0; i < eventCount; ++i) {
        EXPECT_EQ(object.GetMessages()[i], getMessage(i));
    }
}

TEST(PublishSubscribeTest, HandleNoEvents)
{
    ObjectWithHandler object;

    // Verify the handler has not processed any events
    EXPECT_TRUE(object.GetMessages().empty());
}

#endif