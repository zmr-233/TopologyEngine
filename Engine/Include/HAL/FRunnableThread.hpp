/****************************************
 * FILE: Engine/Include/HAL/RunnableThread.hpp
 ****************************************/
#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "CoreTypes.hpp"
#include "FRunnable.hpp"

/**
 * @brief 线程基类
 *
 * -- 本身不含有线程，但是可以根据平台API创建线程 --
 * 是对线程进行创建和生命周期的管理
 */

class FRunnableThread {
   public:
    FRunnableThread();
    virtual ~FRunnableThread();

    // 创建线程并运行
    // 等价于 UE 的 CreateInternal() + PostCreate()
    bool Create(FRunnable* InRunnable, const std::string& InThreadName);

    // 请求停止线程
    bool Kill(bool bShouldWait = false);

    // 等待线程结束
    void WaitForCompletion();

    // 获取线程 ID
    uint32 GetThreadID() const {
        return ThreadID;
    }

    // 获取线程名称
    const std::string& GetThreadName() const {
        return ThreadName;
    }

    // 是否在运行
    bool IsRunning() const {
        return bRunning;
    }

    // 未实现函数:
    //设置线程优先级
    void SetThreadPriority(/* EThreadPriority prio */) {}
    //暂停/恢复线程
    void Suspend(bool bPause) {}

   private:
    // 真正的线程函数
    void RunThread();

   private:
    // 关联的 Runnable
    FRunnable* Runnable;

    // 使用的 std::thread 对象
    std::thread WorkerThread;

    // 线程 ID
    uint32 ThreadID;
    std::string ThreadName;

    // 是否在运行
    std::atomic<bool> bRunning;
};

/**
 * @brief FRunnableThreadPThread
 *
 * 注意到FRunnableThread已经作为了线程基类，
 * 并且接下来是各个平台相关代码实现这个线程基类，比如
 * class FRunnableThreadWin	: public FRunnableThread
 * class FRunnableThreadUnix : public FRunnableThreadPThread
 * class FRunnableThreadAndroid : public FRunnableThreadPThread
 *
 * Engine/Source/Runtime/Core/Public/HAL/RunnableThread.h
 * Engine/Source/Runtime/Core/Private/HAL/PThreadRunnableThread.h
 *
 * Engine/Source/Runtime/Core/Public/Unix/UnixPlatformRunnableThread.h
 * Engine/Source/Runtime/Core/Private/Windows/WindowsRunnableThread.h
 * Engine/Source/Runtime/Core/Private/Microsoft/MicrosoftRunnableThread.h
 *
 * 在FRunnableThread和具体平台实现之间，还隔了一层FRunnableThreadPThread
 *
 * UE 对线程有一些更“底层”的需求：
 *
 * 1. 自定义线程堆栈大小：std::thread 并没有跨平台地直接支持“在创建线程时指定堆栈大小、创建标记”等。
 * 2. 设置线程优先级：std::thread 也没有标准 API 去做操作系统级别的优先级设置；通常需要平台特定的系统调用 (如 pthread_setschedparam 或 WinAPI SetThreadPriority)。
 * 3. 设置线程亲和度：std::thread 也没有标准 API，只能调用 pthread_setaffinity_np / WinAPI SetThreadAffinityMask。
 * 4. 线程命名：C++ 直到较新的标准也没有给出统一跨平台的线程命名方式（有的库用 pthread_setname_np，有的用 SetThreadDescription 等），所以 UE 需要自己封装。
 * 5. UE 的内存调试、Profiler、CrashHandler 等也会对线程进行特殊处理，比如在引擎里登记/反登记，配合各种调试工具。
 *
 *
 */