#ifndef __WORM_DETAIL_THREAD_POOL_TESTS_H__
#define __WORM_DETAIL_THREAD_POOL_TESTS_H__

#include "../Common.h"

#include <worm/detail/ThreadPool.h>

#include <future>
#include <numeric>

TEST(ThreadPoolTest, ZeroThreads)
{
    EXPECT_THROW(worm::detail::ThreadPool pool(0), std::runtime_error);
}

TEST(ThreadPoolTest, ExecuteTasks)
{
    worm::detail::ThreadPool pool(4); // Create a thread pool with 4 threads

    std::vector<std::future<int>> results;

    // Submit tasks to the thread pool
    for (int i = 0; i < 10; ++i) {
        results.emplace_back(pool.Enqueue([i]() {
            return i * i; // Return the square of the number
        }));
    }

    // Verify the results
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(results[i].get(), i * i);
    }
}

TEST(ThreadPoolTest, ParallelSum)
{
    worm::detail::ThreadPool pool(4); // Create a thread pool with 4 threads

    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 1); // Fill with numbers 1 to 1000

    size_t chunkSize = data.size() / 4;
    std::vector<std::future<int>> results;

    // Divide the work into chunks and process in parallel
    for (size_t i = 0; i < 4; ++i) {
        results.emplace_back(pool.Enqueue([&, i]() {
            return std::accumulate(data.begin() + i * chunkSize,
                data.begin() + (i + 1) * chunkSize, 0);
        }));
    }

    // Combine the results
    int totalSum = 0;
    for (auto& result : results) {
        totalSum += result.get();
    }

    // Verify the result
    EXPECT_EQ(totalSum, std::accumulate(data.begin(), data.end(), 0));
}

TEST(ThreadPoolTest, HandleExceptions)
{
    worm::detail::ThreadPool pool(2); // Create a thread pool with 2 threads

    auto future = pool.Enqueue([]() -> int {
        throw std::runtime_error("Test exception");
    });

    // Verify that the exception is propagated
    EXPECT_THROW(future.get(), std::runtime_error);
}

TEST(ThreadPoolTest, WaitIdle)
{
    worm::detail::ThreadPool pool(4); // Create a thread pool with 4 threads

    std::atomic<int> counter{ 0 };
    const int numTasks = 10;

    // Submit tasks to the thread pool
    for (int i = 0; i < numTasks; ++i) {
        pool.Enqueue([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate work
            counter++;
        });
    }

    // Call WaitIdle to ensure all tasks are completed
    pool.WaitIdle();

    // Verify that all tasks have been completed
    EXPECT_EQ(counter.load(), numTasks);
}

#endif