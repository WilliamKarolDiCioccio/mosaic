#pragma once

#include <gtest/gtest.h>
#include <thread>

#include "mosaic/utils/tsafe/queue.hpp"

TEST(ThreadSafeQueueTest, PushAndTryPop)
{
    mosaic::utils::tsafe::ThreadSafeQueue<int> queue;
    queue.push(42);

    auto value = queue.try_pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 42);
}

TEST(ThreadSafeQueueTest, TryPopEmptyReturnsNullopt)
{
    mosaic::utils::tsafe::ThreadSafeQueue<int> queue;
    EXPECT_FALSE(queue.try_pop().has_value());
}

TEST(ThreadSafeQueueTest, WaitAndPopBlocksUntilAvailable)
{
    mosaic::utils::tsafe::ThreadSafeQueue<int> queue;
    int poppedValue = 0;

    std::thread producer(
        [&]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            queue.push(1337);
        });

    std::thread consumer([&]() { queue.wait_and_pop(poppedValue); });

    producer.join();
    consumer.join();

    EXPECT_EQ(poppedValue, 1337);
}

TEST(ThreadSafeQueueTest, MultiplePushAndPop)
{
    mosaic::utils::tsafe::ThreadSafeQueue<int> queue;

    for (int i = 0; i < 10; ++i)
    {
        queue.push(i);
    }

    for (int i = 0; i < 10; ++i)
    {
        auto val = queue.try_pop();
        ASSERT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), i);
    }

    EXPECT_TRUE(queue.empty());
}

TEST(ThreadSafeQueueTest, ThreadedProducerConsumer)
{
    mosaic::utils::tsafe::ThreadSafeQueue<int> queue;

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
                auto val = queue.try_pop();
                while (!val.has_value())
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    val = queue.try_pop();
                }
                consumed.push_back(val.value());
            }
        });

    producer.join();
    consumer.join();

    EXPECT_EQ(consumed.size(), 100);
}
