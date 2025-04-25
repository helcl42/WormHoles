#include "SyncTests.h"
#include "AsyncTests.h"
#include "QueuedTests.h"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}