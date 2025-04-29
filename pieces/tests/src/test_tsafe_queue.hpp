#pragma once

#include <gtest/gtest.h>
#include <thread>

#include "pieces/tsafe/queue.hpp"

using namespace pieces::tsafe;

/////////////////////////////////////////////////////////////////////////////
//                          Single-threaded tests
/////////////////////////////////////////////////////////////////////////////

TEST(ThreadSafeQueueTest, PushAndTryPop)
{
    ThreadSafeQueue<int> queue;
    queue.push(42);

    auto value = queue.tryPop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 42);
}

TEST(ThreadSafeQueueTest, TryPopEmptyReturnsNullopt)
{
    ThreadSafeQueue<int> queue;

    // Popping when empty should fail
    EXPECT_FALSE(queue.tryPop().has_value());
}

TEST(ThreadSafeQueueTest, MultiplePushAndPop)
{
    ThreadSafeQueue<int> queue;

    for (int i = 0; i < 10; ++i)
    {
        queue.push(i);
    }

    for (int i = 0; i < 10; ++i)
    {
        auto val = queue.tryPop();
        ASSERT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), i);
    }

    EXPECT_TRUE(queue.empty());
}

/////////////////////////////////////////////////////////////////////////////
//                          Multi-threaded tests
/////////////////////////////////////////////////////////////////////////////

TEST(ThreadSafeQueueTest, WaitAndPopBlocksUntilAvailable)
{
    ThreadSafeQueue<int> queue;
    int poppedValue = 0;

    std::thread producer(
        [&]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            queue.push(1337);
        });

    std::thread consumer([&]() { queue.waitAndPop(poppedValue); });

    producer.join();
    consumer.join();

    EXPECT_EQ(poppedValue, 1337);
}

TEST(ThreadSafeQueueTest, ThreadedProducerConsumer)
{
    ThreadSafeQueue<int> queue;

    std::thread producer(
        [&]()
        {
            for (int i = 0; i < 100; ++i)
            {
                queue.push(i);
            }
        });

    std::vector<int> consumed;
    std::thread consumer(
        [&]()
        {
            for (int i = 0; i < 100; ++i)
            {
                auto val = queue.tryPop();
                while (!val.has_value())
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    val = queue.tryPop();
                }
                consumed.push_back(val.value());
            }
        });

    producer.join();
    consumer.join();

    EXPECT_EQ(consumed.size(), 100);
}
