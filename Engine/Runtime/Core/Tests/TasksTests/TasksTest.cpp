/******************************************************
 * @file TasksTests/TasksTest.cpp
 * @brief
 *****************************************************/

#include "Tasks/Tasks.hpp"

#include <gtest/gtest.h>

namespace TE::Core::TypeUtils::Tests {
int Nothing() {
    return 233;
}

} // namespace TE::Core::TypeUtils::Tests

using namespace TE::Core::TypeUtils::Tests;
using namespace TE::Tasks;

TEST(TasksTest, TestLaunch) {
    auto result1 = Launch("Nothing", Nothing);
    EXPECT_EQ(result1.Result, 233);
}
