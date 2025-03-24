/******************************************************
 * @file 01_LearnGL/lib.hpp
 * @brief
 *****************************************************/
#pragma once

struct MyClass {
    int Multiply(int a, int b) const { return a * b; }

    int value = 42;
};

inline int Add(int a, int b) {
    return a + b;
}

void test_invoke();