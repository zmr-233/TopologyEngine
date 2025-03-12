#pragma one
#include <print>

#include "Marco.hpp"

#define USE_LOG true

#define PRIVATE_LOG(Condition, fmt, ...)  \
    if (Condition) {                      \
        std::println(fmt, ##__VA_ARGS__); \
    }

#define LOG(fmt, ...) PRIVATE_LOG(USE_LOG, fmt, ##__VA_ARGS__)
