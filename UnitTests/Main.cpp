#include "worm/detail/SingletonTests.h"
#include "worm/detail/ThreadPoolTests.h"
#include "worm/detail/RingBufferTests.h"

#include "worm/detail/EventChannelQueueManagerTests.h"
#include "worm/detail/EventChannelQueueTests.h"

#include "worm/EventChannelTests.h"
#include "worm/EventHandlerTests.h"

TEST(SampleTest, BasicAssertions)
{
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}