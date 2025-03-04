/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#ifndef __MACRO_HPP__
#define __MACRO_HPP__

#include <cstdint>
#include <cstdio>
#include <cstring>

//======================================================================================
// 1) 字符串相关宏
//======================================================================================

// macro stringizing
#define str_temp(x) #x
#define str(x) str_temp(x)

// strlen() for string constant
#define STRLEN(CONST_STR) (sizeof(CONST_STR) - 1)

// calculate the length of an array
#define ARRLEN(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

// macro concatenation
#define conself_temp(x) x
#define conself(x) conself_temp(x)

#define concat_temp(x, y) x##y
#define concat(x, y) concat_temp(x, y)
#define concat3(x, y, z) concat(concat(x, y), z)
#define concat4(x, y, z, w) concat3(concat(x, y), z, w)
#define concat5(x, y, z, v, w) concat4(concat(x, y), z, v, w)

//======================================================================================
// 2) 用于判断宏是否定义的通用机制
//======================================================================================

// macro testing
#define CHOOSE2nd(a, b, ...) b
#define MUX_WITH_COMMA(contain_comma, a, b) CHOOSE2nd(contain_comma a, b)
#define MUX_MACRO_PROPERTY(p, macro, a, b) MUX_WITH_COMMA(concat(p, macro), a, b)
// define placeholders for some property
#define __P_DEF_0 X,
#define __P_DEF_1 X,
#define __P_ONE_1 X,
#define __P_ZERO_0 X,
// define some selection functions based on the properties of BOOLEAN macro
#define MUXDEF(macro, X, Y) MUX_MACRO_PROPERTY(__P_DEF_, macro, X, Y)
#define MUXNDEF(macro, X, Y) MUX_MACRO_PROPERTY(__P_DEF_, macro, Y, X)
#define MUXONE(macro, X, Y) MUX_MACRO_PROPERTY(__P_ONE_, macro, X, Y)
#define MUXZERO(macro, X, Y) MUX_MACRO_PROPERTY(__P_ZERO_, macro, X, Y)

// test if a boolean macro is defined
#define ISDEF(macro) MUXDEF(macro, 1, 0)
// test if a boolean macro is undefined
#define ISNDEF(macro) MUXNDEF(macro, 1, 0)
// test if a boolean macro is defined to 1
#define ISONE(macro) MUXONE(macro, 1, 0)
// test if a boolean macro is defined to 0
#define ISZERO(macro) MUXZERO(macro, 1, 0)

// test if a macro of ANY type is defined
// NOTE1: it ONLY works inside a function, since it calls `strcmp()`
// NOTE2: macros defined to themselves (#define A A) will get wrong results
#define isdef(macro) (std::strcmp("" #macro, "" str(macro)) != 0)

//======================================================================================
// 3) 简化条件编译的辅助宏
//======================================================================================
#define __IGNORE(...)
#define __KEEP(...) __VA_ARGS__
// keep the code if a boolean macro is defined
#define IFDEF(macro, ...) MUXDEF(macro, __KEEP, __IGNORE)(__VA_ARGS__)
// keep the code if a boolean macro is undefined
#define IFNDEF(macro, ...) MUXNDEF(macro, __KEEP, __IGNORE)(__VA_ARGS__)
// keep the code if a boolean macro is defined to 1
#define IFONE(macro, ...) MUXONE(macro, __KEEP, __IGNORE)(__VA_ARGS__)
// keep the code if a boolean macro is defined to 0
#define IFZERO(macro, ...) MUXZERO(macro, __KEEP, __IGNORE)(__VA_ARGS__)

// functional-programming-like macro (X-macro)
// apply the function `f` to each element in the container `c`
#define MAP(c, f) c(f)

//======================================================================================
// 4) 位操作、对齐操作等工具宏 (C++ 版本中尽量避免 GNU 扩展)
//======================================================================================

#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1))  // x[hi:lo] in Verilog

//----- 替换原先的 GNU 扩展形式 SEXT(x, len) -----
/*
// 原先宏：
// #define SEXT(x, len) ({ struct { int64_t n : len; } __x = { .n = x }; (uint64_t)__x.n; })
//
// 在标准 C++ 中，位域大小必须是编译期常量，且不能用 GNU statement expression。
// 可以使用下列 inline 函数实现动态位宽的 sign extension。
*/
inline uint64_t sext64(uint64_t x, int len) {
    // 将 len 位的数进行符号扩展到 64 位
    // 常见做法：先左移 (64 - len)，再进行算术右移
    const int shift = 64 - len;
    return static_cast<uint64_t>((static_cast<int64_t>(x << shift)) >> shift);
}

#define SEXT(x, len) (sext64(static_cast<uint64_t>(x), static_cast<int>(len)))

//----- ROUNDUP / ROUNDDOWN 保持简单的宏即可 -----
#define ROUNDUP(a, sz) ((((uintptr_t)(a)) + (sz) - 1) & ~((sz) - 1))
#define ROUNDDOWN(a, sz) ((((uintptr_t)(a))) & ~((sz) - 1))

//----- 原先使用 __attribute((aligned(4096)))，这里改用 alignas(4096) -----
#define PG_ALIGN alignas(4096)

//======================================================================================
// 5) 分支预测辅助 (若已有就不再定义)
//======================================================================================
#if !defined(likely)
#define likely(cond) __builtin_expect(!!(cond), 1)
#define unlikely(cond) __builtin_expect(!!(cond), 0)
#endif

//======================================================================================
// 6) io_read / io_write 的宏在 C++ 中去除 GNU 扩展 ({...})
//    采用立即执行 lambda 的形式来返回参数
//    依赖 C++11 特性，若环境不支持则需其它改写
//======================================================================================
/*
   假设原先 C 里有:
   #define io_read(reg) \
     ({ reg##_T __io_param; \
        ioe_read(reg, &__io_param); \
        __io_param; })

   在标准 C++ 中无法直接使用 ({...}) 语法，可用 lambda 代替：
*/
#define io_read(reg) ([&]() {   \
    reg##_T __io_param;         \
    ioe_read(reg, &__io_param); \
    return __io_param;          \
}())

/*
   #define io_write(reg, ...) \
     ({ reg##_T __io_param = (reg##_T) { __VA_ARGS__ }; \
        ioe_write(reg, &__io_param); })

   改为：
*/
#define io_write(reg, ...) ([&]() {     \
    reg##_T __io_param = {__VA_ARGS__}; \
    ioe_write(reg, &__io_param);        \
}())

//======================================================================================
// 7) 颜色打印相关宏
//======================================================================================
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define RED "\033[0;31m"
#define RED_BOLD "\033[1;31m"
#define YELLOW "\033[0;33m"
#define YELLOW_BOLD "\033[1;33m"
#define GREEN "\033[0;32m"
#define GREEN_BOLD "\033[1;32m"
#define BLUE "\033[0;34m"
#define BLUE_BOLD "\033[1;34m"
#define GREY "\033[0;37m"
#define CYAN_BOLD "\033[1;36m"
#define RESET "\033[0m"

//======================================================================================
// 8) 带行号/errno 的打印宏
//======================================================================================
#define nECHO(color, fmt, ...) std::printf(color fmt RESET, ##__VA_ARGS__)

#ifdef SHOWLINE
#ifdef SHOWERRNO
// 显示 errno 信息
#include <errno.h>
#define strERRNO() (errno == 0 ? "None" : std::strerror(errno))
#define ECHO(color, fmt, ...) std::printf(                                                               \
    color fmt RESET "    (" GREY "%s" RESET ":" CYAN_BOLD "%d" RESET "," YELLOW "ERRNO: %s" RESET ")\n", \
    ##__VA_ARGS__, __FILE__, __LINE__, strERRNO())
#else
// 只显示行号
#define ECHO(color, fmt, ...) std::printf(                                  \
    color fmt RESET "    (" GREY "%s" RESET ":" CYAN_BOLD "%d" RESET ")\n", \
    ##__VA_ARGS__, __FILE__, __LINE__)
#endif
#else
// 不显示行号
#define ECHO(color, fmt, ...) std::printf(color fmt RESET "\n", ##__VA_ARGS__)
#endif

#define INFO(fmt, ...) ECHO(GREEN_BOLD, "[INFO] " fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) ECHO(YELLOW_BOLD, "[WARNING] " fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) ECHO(RED_BOLD, "[ERROR] " fmt, ##__VA_ARGS__)
#define SUCCESS(fmt, ...) ECHO(GREEN_BOLD, "[SUCCESS] " fmt, ##__VA_ARGS__)
#define NOTE(fmt, ...) ECHO(BLUE_BOLD, "[NOTE] " fmt, ##__VA_ARGS__)
#define INPUT(fmt, ...) ECHO(CYAN_BOLD, "==INPUT== " fmt, ##__VA_ARGS__)
#define ABORT(fmt, ...) ECHO(RED_BOLD, "[ABORT] " fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) ECHO(YELLOW, "[DEBUG] " fmt, ##__VA_ARGS__)

#endif  // __MACRO_HPP__
