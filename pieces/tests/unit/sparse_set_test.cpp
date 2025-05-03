#pragma once

#include <gtest/gtest.h>

#include "pieces/sparse_set.hpp"

using namespace pieces;

using K = size_t;
using T = int;
using SparseSetType = SparseSet<K, T>;

// Test fixture
class SparseSetTest : public ::testing::Test
{
   protected:
    SparseSetType set;
};

// Test insertion and contains()
TEST_F(SparseSetTest, InsertAndContains)
{
    EXPECT_FALSE(set.contains(1));

    set.insert(1, 100);

    EXPECT_TRUE(set.contains(1));
    EXPECT_EQ(set.get(1).unwrap(), 100);
}

// Test overwriting existing key
TEST_F(SparseSetTest, OverwriteValue)
{
    set.insert(2, 200);

    EXPECT_EQ(set.get(2).unwrap(), 200);

    set.insert(2, 250);

    EXPECT_EQ(set.get(2).unwrap(), 250);
    EXPECT_EQ(set.size(), 1);
}

// Test removal of keys
TEST_F(SparseSetTest, RemoveKey)
{
    set.insert(3, 300);
    set.insert(5, 500);

    EXPECT_TRUE(set.contains(3));
    EXPECT_TRUE(set.contains(5));

    set.remove(3);

    EXPECT_FALSE(set.contains(3));
    EXPECT_TRUE(set.contains(5));
}

// Test get throws on missing key
TEST_F(SparseSetTest, GetMissingReturn) { EXPECT_EQ(set.get(10).error(), ErrorCode::out_of_range); }

// Test clear()
TEST_F(SparseSetTest, ClearEmptiesSet)
{
    set.insert(7, 700);
    set.insert(8, 800);

    EXPECT_EQ(set.size(), 2);

    set.clear();

    EXPECT_EQ(set.size(), 0);
    EXPECT_FALSE(set.contains(7));
    EXPECT_FALSE(set.contains(8));
}

// Test reserve does not break functionality
TEST_F(SparseSetTest, ReserveCapacity)
{
    // Reserve sparse capacity for keys up to 100 and values count 5
    set.reserve(100, 5);

    for (K key = 0; key < 5; ++key)
    {
        set.insert(key, static_cast<T>(key * 10));
    }

    for (K key = 0; key < 5; ++key)
    {
        EXPECT_TRUE(set.contains(key));
        EXPECT_EQ(set.get(key).unwrap(), static_cast<T>(key * 10));
    }

    EXPECT_EQ(set.size(), 5);
}

// Test iteration over keys and values maintains dense order
TEST_F(SparseSetTest, DenseIterationOrder)
{
    set.insert(10, 1000);
    set.insert(20, 2000);
    set.insert(15, 1500);

    const auto& keys = set.keys();
    const auto& values = set.values();

    ASSERT_EQ(keys.size(), 3);
    ASSERT_EQ(values.size(), 3);
    EXPECT_EQ(keys[0], 10);
    EXPECT_EQ(values[0], 1000);
    EXPECT_EQ(keys[1], 20);
    EXPECT_EQ(values[1], 2000);
    EXPECT_EQ(keys[2], 15);
    EXPECT_EQ(values[2], 1500);
}

// Test large key grows sparse array
TEST_F(SparseSetTest, LargeKeyGrow)
{
    K largeKey = 1000;
    T val = 123;

    set.insert(largeKey, val);

    EXPECT_TRUE(set.contains(largeKey));
    EXPECT_EQ(set.get(largeKey).unwrap(), val);
    EXPECT_EQ(set.size(), 1);
}

// Test get by reference
TEST_F(SparseSetTest, GetByReference)
{
    set.insert(4, 400);

    auto result = set.get(4);

    EXPECT_EQ(result.unwrap(), 400);

    result.unwrap() = 500;

    EXPECT_EQ(set.get(4).unwrap(), 500);
}
