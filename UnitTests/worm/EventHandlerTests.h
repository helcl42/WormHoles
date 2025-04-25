#ifndef __WORM_EVENT_HANDLER_TESTS_H__
#define __WORM_EVENT_HANDLER_TESTS_H__

#include "Common.h"

#include <worm/EventHandler.h>

TEST(EventHandlerTest, HandleSingleEvent)
{
    MockHandler handler;
    worm::EventHandler<MockHandler, TestEvent> eventHandler(handler);

    // Create a test event
    TestEvent event{ "Test Message" };

    // Invoke the handler
    eventHandler(event);

    // Verify the handler processed the event
    EXPECT_EQ(handler.GetMessages().size(), 1);
    EXPECT_EQ(handler.GetMessages()[0], "Test Message");
}

TEST(EventHandlerTest, HandleMultipleEvents)
{
    MockHandler handler;
    worm::EventHandler<MockHandler, TestEvent> eventHandler(handler);

    // Create multiple test events
    TestEvent event1{ "Message 1" };
    TestEvent event2{ "Message 2" };

    // Invoke the handler for each event
    eventHandler(event1);
    eventHandler(event2);

    // Verify the handler processed all events
    EXPECT_EQ(handler.GetMessages().size(), 2);
    EXPECT_EQ(handler.GetMessages()[0], "Message 1");
    EXPECT_EQ(handler.GetMessages()[1], "Message 2");
}

TEST(EventHandlerTest, HandleNoEvents)
{
    MockHandler handler;
    worm::EventHandler<MockHandler, TestEvent> eventHandler(handler);

    // Verify the handler has not processed any events
    EXPECT_TRUE(handler.GetMessages().empty());
}

#endif