/******************************************************
 * @file DebugUtils/CoreDebug.hpp
 * @brief
 *****************************************************/

#pragma once
#include <cassert>

// struct FDebug {
//     static
// };

// #include "TypeUtils/CoreMarco.hpp"

// Engine/Source/Runtime/Core/Public/Misc/AssertionMacros.h
// #define CHECK_IMPL(expr) \
//     { \
//         if (UNLIKELY(!(expr))) { \
//             if (FDebug::CheckVerifyFailedImpl2(#expr, __FILE__, __LINE__,
//             TEXT(""))) { \
//                 PLATFORM_BREAK(); \
//             } \
//         } \
//     }
#define CHECK_IMPL(expr) assert(expr)

#define check(expr) CHECK_IMPL(expr)