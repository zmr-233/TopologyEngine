/******************************************************
 * @file Thread/ThreadPool_win.cpp
 * @brief
 *****************************************************/

#include "Thread/ThreadPool.hpp"

#ifdef _WIN32

#include <windows.h>

#include <atomic>
#include <functional>
#include <queue>
#include <stdexcept>
#include <vector>

struct ThreadPool::ThreadPoolImpl {
    int                 threadCount;
    std::vector<HANDLE> threads;

    // 任务队列及其保护
    std::queue<std::function<void()>> tasks;
    CRITICAL_SECTION                  lock;
    CONDITION_VARIABLE                cond;        // 通知工作线程有任务可执行
    CONDITION_VARIABLE                condAllDone; // 通知 waitAll() 所有任务执行完毕

    // 停止标志
    std::atomic<bool> stop;
    // 当前正在执行的任务计数
    std::atomic<int> activeCount;

    ThreadPoolImpl(int numThreads)
        : threadCount(numThreads), threads(numThreads), stop(false), activeCount(0) {
        InitializeCriticalSectionAndSpinCount(&lock, 4000);
        InitializeConditionVariable(&cond);
        InitializeConditionVariable(&condAllDone);

        // 创建线程
        for (int i = 0; i < threadCount; ++i) {
            HANDLE h = CreateThread(
                /* lpThreadAttributes = */ nullptr,
                /* dwStackSize = */ 0,
                /* lpStartAddress = */ &ThreadPoolImpl::threadEntry,
                /* lpParameter = */ this,
                /* dwCreationFlags = */ 0,
                /* lpThreadId = */ nullptr);
            if (!h) {
                // 创建线程失败，清理已创建的并抛异常
                stop = true;
                WakeAllConditionVariable(&cond);
                for (int j = 0; j < i; ++j) {
                    WaitForSingleObject(threads[j], INFINITE);
                    CloseHandle(threads[j]);
                }
                DeleteCriticalSection(&lock);
                throw std::runtime_error("CreateThread failed");
            }
            threads[i] = h;
        }
    }

    ~ThreadPoolImpl() {
        // 通知所有线程停止
        stop = true;
        WakeAllConditionVariable(&cond);

        // 等待所有线程退出
        for (HANDLE h: threads) {
            WaitForSingleObject(h, INFINITE);
            CloseHandle(h);
        }

        DeleteCriticalSection(&lock);
        // CONDITION_VARIABLE 在 Windows 下不需要显式销毁
    }

    static DWORD WINAPI threadEntry(LPVOID arg) {
        ThreadPoolImpl *impl = static_cast<ThreadPoolImpl *>(arg);
        impl->threadLoop();
        return 0;
    }

    void threadLoop() {
        for (;;) {
            std::function<void()> task;

            // 加锁取任务
            EnterCriticalSection(&lock);
            // 没任务且未 stop 时，睡眠等待
            while (!stop && tasks.empty()) {
                SleepConditionVariableCS(&cond, &lock, INFINITE);
            }
            // 如果 stop 并且任务队列空，则退出线程
            if (stop && tasks.empty()) {
                LeaveCriticalSection(&lock);
                break;
            }

            // 取出一个任务
            task = std::move(tasks.front());
            tasks.pop();
            // 取到任务后，活动数+1
            activeCount.fetch_add(1, std::memory_order_relaxed);

            LeaveCriticalSection(&lock);

            // 执行任务
            task();

            // 任务执行结束后，activeCount - 1
            int stillActive = activeCount.fetch_sub(1, std::memory_order_relaxed) - 1;

            // 如果此时没有活动任务了 (stillActive==0)，并且队列也空了，则可唤醒等待方
            EnterCriticalSection(&lock);
            if (stillActive == 0 && tasks.empty()) {
                WakeAllConditionVariable(&condAllDone);
            }
            LeaveCriticalSection(&lock);
        }
    }

    void enqueueTask(std::function<void()> f) {
        EnterCriticalSection(&lock);
        tasks.push(std::move(f));
        LeaveCriticalSection(&lock);
        // 唤醒一个工作线程
        WakeConditionVariable(&cond);
    }

    void waitAllTasksDone() {
        // 等待队列为空且没有正在执行的任务
        EnterCriticalSection(&lock);
        while (!tasks.empty() || activeCount.load(std::memory_order_relaxed) > 0) {
            SleepConditionVariableCS(&condAllDone, &lock, INFINITE);
        }
        LeaveCriticalSection(&lock);
    }
};

// ============== ThreadPool 对外接口实现 =============

ThreadPool::ThreadPool(int numThreads) {
    if (numThreads <= 0) {
        numThreads = 1;
    }
    impl_ = std::make_unique<ThreadPoolImpl>(numThreads);
}

ThreadPool::~ThreadPool() = default;

void ThreadPool::submit(std::function<void()> task) {
    impl_->enqueueTask(std::move(task));
}

void ThreadPool::waitAll() {
    impl_->waitAllTasksDone();
}