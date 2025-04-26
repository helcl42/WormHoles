#ifndef __QUEUED_TESTS_H__
#define __QUEUED_TESTS_H__

#include "Common.h"

#include <worm/EventChannel.h>

#include <sstream>


TEST(PublishSubscribeTest, HandleQueuedEvent)
{
    ObjectWithHandler object;

    // Post a queued event
    worm::EventChannel::Post(TestEvent{ "Queued Message" }, worm::DispatchType::QUEUED);

    // Verify no messages are delivered yet
    EXPECT_TRUE(object.GetMessages().empty());

    // Dispatch all queued events
    worm::EventChannel::DispatchAllQueued();

    // Verify the handler processed the queued event
    EXPECT_EQ(object.GetMessages().size(), 1);
    EXPECT_EQ(object.GetMessages()[0], "Queued Message");
}

TEST(PublishSubscribeTest, HandleMultipleQueuedEvents)
{
    ObjectWithHandler object;

    // Post multiple queued events
    worm::EventChannel::Post(TestEvent{ "Queued Message 1" }, worm::DispatchType::QUEUED);
    worm::EventChannel::Post(TestEvent{ "Queued Message 2" }, worm::DispatchType::QUEUED);

    // Verify no messages are delivered yet
    EXPECT_TRUE(object.GetMessages().empty());

    // Dispatch all queued events
    worm::EventChannel::DispatchAllQueued();

    // Verify the handler processed all queued events
    EXPECT_EQ(object.GetMessages().size(), 2);
    EXPECT_EQ(object.GetMessages()[0], "Queued Message 1");
    EXPECT_EQ(object.GetMessages()[1], "Queued Message 2");
}

TEST(PublishSubscribeTest, HandleMultipleQueuedEvents2)
{
    ObjectWithHandler object;

    const uint32_t eventCount{ 1000 };

    auto getMessage = [](uint32_t i) {
        return (std::stringstream{} << "Queued Message " << i).str();
    };

    // Post multiple queued events
    for (uint32_t i = 0; i < eventCount; ++i) {
        TestEvent event{ getMessage(i) };
        worm::EventChannel::Post(event, worm::DispatchType::QUEUED);
    }

    // Verify no messages are delivered yet
    EXPECT_TRUE(object.GetMessages().empty());

    // Dispatch all queued events
    worm::EventChannel::DispatchAllQueued();

    EXPECT_EQ(object.GetMessages().size(), eventCount);
    for (uint32_t i = 0; i < eventCount; ++i) {
        EXPECT_EQ(object.GetMessages()[i], getMessage(i));
    }
}

#endif