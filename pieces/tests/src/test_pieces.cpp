#include <gtest/gtest.h>

#include "test_tsafe_map.hpp"
#include "test_tsafe_queue.hpp"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}