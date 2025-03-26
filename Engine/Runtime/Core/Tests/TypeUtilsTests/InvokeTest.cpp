/******************************************************
 * @file TypeUtilsTests/InvokeTest.cpp
 * @brief
 *****************************************************/

#include "TypeUtils/Invoke.hpp"

#include <gtest/gtest.h>

namespace TE::Core::TypeUtils::Tests {
struct MyClass {
    int Multiply(int a, int b) const { return a * b; }

    int value = 42;
};

int Add(int a, int b) {
    return a + b;
}
} // namespace TE::Core::TypeUtils::Tests

using namespace TE::Core::TypeUtils::Tests;

// 单元测试部分
TEST(InvokeTest, TestAddFunction) {
    auto result1 = Invoke(Add, 3, 4);
    EXPECT_EQ(result1, 7);
}

TEST(InvokeTest, TestMultiplyMemberFunction) {
    MyClass myObj;
    auto    result2 = Invoke(&MyClass::Multiply, myObj, 3, 4);
    EXPECT_EQ(result2, 12);
}

TEST(InvokeTest, TestMemberVariable) {
    MyClass myObj;
    auto    result3 = Invoke(&MyClass::value, myObj);
    EXPECT_EQ(result3, 42);
}

TEST(InvokeTest, TestFuncProjection) {
    auto projectedAdd = FUNC_PROJECTION(Add);
    auto result       = projectedAdd(5, 6);
    EXPECT_EQ(result, 11);
}

TEST(InvokeTest, TestMemberProjection) {
    MyClass myObj;
    auto    projectedMultiply = MEMBER_PROJECTION(MyClass, Multiply);
    auto    result            = projectedMultiply(myObj, 2, 3);
    EXPECT_EQ(result, 6);
}