/****************************************
 * FILE: Engine/Include/HAL/ENamedThreads.hpp
 ****************************************/

#include <cassert>
#include <cstdint>

#pragma once

/**
 * 简化版 ENamedThreads::Type
 * 说明：
 * - 前 8 位存储“线程下标”，0..4 分别表示 GameThread, RenderThread, RHIThread, AudioThread, StatsThread
 * - 0xff 表示 AnyThread（后台线程池）
 * - 第 8~9 位用于 QueueIndex (MainQueue=0, LocalQueue=1)
 */
namespace ENamedThreads {
enum Type : uint32_t {
    // 低 8 bits: 线程 index
    GameThread   = 0,
    RenderThread = 1,
    RHIThread    = 2,
    AudioThread  = 3,
    StatsThread  = 4,

    // 这里仅作示例，可扩展
    AnyThread = 0xff,

    // queue index
    MainQueueMask  = 0x000,
    LocalQueueMask = 0x100,

    // 方便的组合
    GameThread_Local   = GameThread | LocalQueueMask,
    RenderThread_Local = RenderThread | LocalQueueMask,
    RHIThread_Local    = RHIThread | LocalQueueMask,
    AudioThread_Local  = AudioThread | LocalQueueMask,
    StatsThread_Local  = StatsThread | LocalQueueMask,

    AnyThread_Main  = AnyThread | MainQueueMask,
    AnyThread_Local = AnyThread | LocalQueueMask,
};

// 提取真实的“线程下标”
inline uint32_t GetThreadIndex(Type in) {
    return (uint32_t)(in & 0xff);
}
// 提取 queueIndex
inline bool IsLocalQueue(Type in) {
    return (in & 0x100) != 0;
}
inline bool IsAnyThread(Type in) {
    return (GetThreadIndex(in) == 0xff);
}
}  // namespace ENamedThreads
