/******************************************************
 * @file MarcoUtils/StringMarco.hpp
 * @brief 字符串相关宏
 *****************************************************/

#pragma once

// macro stringizing
#define TE_STRINGIZE(x) TE_PRIVATE_STRINGIZE(x)
#define TE_PRIVATE_STRINGIZE(x) #x

// macro concatenation
#define TE_JOIN(x, y) TE_PRIVATE_JOIN(x, y)
#define TE_PRIVATE_JOIN(x, y) x##y

#define TE_JOIN3(x, y, z) TE_JOIN(TE_JOIN(x, y), z)
#define TE_JOIN4(x, y, z, w) TE_JOIN(TE_JOIN3(x, y, z), w)
#define TE_JOIN5(x, y, z, v, w) TE_JOIN(TE_JOIN4(x, y, z, v), w)

// 仅仅连接第一个参数和第二个参数
// UE_JOIN_FIRST(A,B,C,D) => AB, C, D
#define TE_JOIN_FIRST(Token, ...) TE_PRIVATE_JOIN_FIRST(Token, __VA_ARGS__)
#define TE_PRIVATE_JOIN_FIRST(Token, ...) Token##__VA_ARGS__
