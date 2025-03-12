/****************************************
 * FILE: Engine/Include/HAL/FEvent.hpp
 ****************************************/
#pragma once
#include <condition_variable>
#include <mutex>

/**
 * 一个简单的事件（信号）封装
 * - Wait()：等待事件触发
 * - Trigger()：触发事件，让 Wait 退出
 * - 适用于一次触发后就不会再“自动复位”的场景（相当于一个Latch
 */
class FEvent {
   public:
    FEvent(bool bInitiallySignaled = false)
        : bSignaled(bInitiallySignaled) {
    }

    // 等待事件触发
    void Wait() {
        std::unique_lock<std::mutex> lock(Mutex);
        Condition.wait(lock, [&] { return bSignaled; });
    }

    // 触发事件
    void Trigger() {
        {
            std::lock_guard<std::mutex> lock(Mutex);
            bSignaled = true;
        }
        Condition.notify_all();
    }

    // 重置事件
    void Reset() {
        std::lock_guard<std::mutex> lock(Mutex);
        bSignaled = false;
    }

   private:
    bool bSignaled;
    std::mutex Mutex;
    std::condition_variable Condition;
};
