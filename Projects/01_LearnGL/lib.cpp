/******************************************************
 * @file 01_LearnGL/lib.cpp
 * @brief
 *****************************************************/

#include "lib.hpp"

#include "TypeUtils/Invoke.hpp"

#include <print>

void test_invoke() {
    using namespace TE::Core::TypeUtils;

    // 1. 使用 Invoke 调用普通函数
    auto result1 = Invoke(Add, 3, 4);
    std::println("Add result: {}", result1);

    // 2. 使用 Invoke 调用成员函数
    MyClass myObj;
    auto    result2 = Invoke(&MyClass::Multiply, myObj, 3, 4);
    std::println("Multiply result: {}", result2);

    auto result3 = Invoke(&MyClass::value, myObj);           
    std::println("Member variable result: {}", result3);

    // 4. 使用 FUNC_PROJECTION 宏进行函数投影
    auto projectedAdd = FUNC_PROJECTION(Add);
    std::println("Projected Add result: {}", projectedAdd(5, 6));

    // 5. 使用 MEMBER_PROJECTION 宏进行成员函数投影
    auto projectedMultiply = MEMBER_PROJECTION(MyClass, Multiply);
    std::println("Projected Multiply result: {}", projectedMultiply(myObj, 2, 3));
}
