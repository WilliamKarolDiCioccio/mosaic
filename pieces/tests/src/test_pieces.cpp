#include <gtest/gtest.h>

#include "test_result.hpp"
#include "test_tsafe_map.hpp"
#include "test_tsafe_queue.hpp"
#include "test_work_stealing_queue.hpp"
#include "test_coroutines.hpp"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
