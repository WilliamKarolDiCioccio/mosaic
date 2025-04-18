#pragma once

#include <gtest/gtest.h>
#include <unordered_map>
#include <optional>
#include <shared_mutex>

#include "pieces/tsafe/map.hpp"

using namespace pieces::tsafe;

TEST(ThreadSafeMapTest, InsertAndGet)
{
    ThreadSafeMap<int, std::string> map;
    map.insert(1, "one");

    auto result = map.get(1);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "one");
}

TEST(ThreadSafeMapTest, OverwriteAndErase)
{
    ThreadSafeMap<int, std::string> map;
    map.insert(1, "one");
    map.insert(1, "uno");

    EXPECT_EQ(map.get(1).value(), "uno");

    EXPECT_TRUE(map.erase(1));
    EXPECT_FALSE(map.get(1).has_value());
}

TEST(ThreadSafeMapTest, InsertIfAbsent)
{
    ThreadSafeMap<int, std::string> map;
    map.insertIfAbsent(1, "one");
    map.insertIfAbsent(1, "uno"); // Should not overwrite

    EXPECT_EQ(map.get(1).value(), "one");
}

TEST(ThreadSafeMapTest, TransformRollbackOnException)
{
    ThreadSafeMap<int, int> map;
    map.insert(1, 10);

    try
    {
        map.transform(
            [](auto& m)
            {
                m[1] = 42;
                throw std::runtime_error("fail");
            });
    }
    catch (...)
    {
    }

    EXPECT_EQ(map.get(1).value(), 10); // should rollback
}

TEST(ThreadSafeMapTest, ThreadedInsert)
{
    ThreadSafeMap<int, int> map;

    auto thread_fn = [&map](int start)
    {
        for (int i = 0; i < 100; ++i)
        {
            map.insert(start + i, i);
        }
    };

    std::thread t1(thread_fn, 0);
    std::thread t2(thread_fn, 100);
    t1.join();
    t2.join();

    EXPECT_EQ(map.size(), 200);
}
