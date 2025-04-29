#pragma once

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>

#include "pieces/tsafe/work_stealing_queue.hpp"

using namespace pieces::tsafe;

/////////////////////////////////////////////////////////////////////////////
//                          Single-threaded tests
/////////////////////////////////////////////////////////////////////////////

TEST(WorkStealingQueueTest, PushPopSingleThread)
{
    WorkStealingQueue<int> queue;
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);

    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 3u);

    int value;
    // LIFO order: last in, first out
    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, 3);
    EXPECT_EQ(queue.size(), 2u);

    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, 2);
    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, 1);

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);

    // Popping when empty should fail
    EXPECT_FALSE(queue.tryPop(value));
}

TEST(WorkStealingQueueTest, TryStealSingleThread)
{
    WorkStealingQueue<int> queue;
    EXPECT_TRUE(queue.empty());

    queue.push(10);
    queue.push(20);
    queue.push(30);

    int value;
    // FIFO order: first in, first out
    EXPECT_TRUE(queue.trySteal(value));
    EXPECT_EQ(value, 10);
    EXPECT_EQ(queue.size(), 2u);

    EXPECT_TRUE(queue.trySteal(value));
    EXPECT_EQ(value, 20);
    EXPECT_TRUE(queue.trySteal(value));
    EXPECT_EQ(value, 30);

    EXPECT_TRUE(queue.empty());

    // Stealing when empty should fail
    EXPECT_FALSE(queue.trySteal(value));
}

/////////////////////////////////////////////////////////////////////////////
//                          Multi-threaded tests
/////////////////////////////////////////////////////////////////////////////

TEST(WorkStealingQueueTest, ConcurrentSteal)
{
    constexpr int kNumItems = 1000;
    constexpr int kNumThreads = 4;
    WorkStealingQueue<int> queue;

    // Fill the queue with 0..kNumItems-1
    for (int i = 0; i < kNumItems; ++i)
    {
        queue.push(i);
    }

    EXPECT_EQ(queue.size(), static_cast<size_t>(kNumItems));

    std::vector<int> stolen;
    stolen.reserve(kNumItems);
    std::mutex stolen_mutex;

    // Launch threads to steal
    std::vector<std::thread> threads;
    for (int t = 0; t < kNumThreads; ++t)
    {
        threads.emplace_back(
            [&]()
            {
                int value;
                while (queue.trySteal(value))
                {
                    std::lock_guard<std::mutex> lock(stolen_mutex);
                    stolen.push_back(value);
                }
            });
    }

    // Wait for all threads
    for (auto& th : threads)
    {
        th.join();
    }

    // After all stealing, queue should be empty
    EXPECT_TRUE(queue.empty());

    // All items should have been stolen
    EXPECT_EQ(stolen.size(), static_cast<size_t>(kNumItems));

    // Verify that stolen contains exactly the numbers 0..kNumItems-1
    std::sort(stolen.begin(), stolen.end());
    for (int i = 0; i < kNumItems; ++i)
    {
        EXPECT_EQ(stolen[i], i);
    }
}

// Empty queue tests
TEST(WorkStealingQueueTest, EmptyQueue)
{
    WorkStealingQueue<std::string> queue;
    std::string result;
    EXPECT_FALSE(queue.tryPop(result));
    EXPECT_FALSE(queue.trySteal(result));
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);
}
