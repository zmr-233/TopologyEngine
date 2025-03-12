/****************************************
 * FILE: Engine/Include/HAL/QueuedThreadPool.hpp
 ****************************************/

#pragma once

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "IQueuedWork.hpp"
#include "FRunnable.hpp"
#include "FRunnableThread.hpp"

/**
 * 一个工作线程，内含一个 “取任务循环”
 */
class FQueuedThread : public FRunnable {
   public:
    FQueuedThread()
        : OwnerPool(nullptr), bStop(false) {}

    virtual ~FQueuedThread() {}

    // 初始化此工作线程，OwnerPool就是宿主的线程池
    bool Create(class FQueuedThreadPool* InPool, const std::string& ThreadName) {
        OwnerPool = InPool;
        return ThreadImpl.Create(this, ThreadName);
    }

    // 通知线程停止
    void StopThread() {
        bStop.store(true);
    }

    // FRunnable interface
    virtual bool Init() override {
        return true;
    }

    virtual uint32_t Run() override {
        // 不断从队列里取任务执行
        while (!bStop.load()) {
            IQueuedWork* Work = OwnerPool->WaitForWork();
            if (Work) {
                Work->DoThreadedWork();
            }
        }
        return 0;
    }

    virtual void Stop() override {
        // 当外部 Kill 时，会调用到这里
        bStop.store(true);
    }

    virtual void Exit() override {
    }

   private:
    // 用来启动此 Runnable 的实际线程对象
    FRunnableThread ThreadImpl;

    // 指向宿主线程池
    class FQueuedThreadPool* OwnerPool;

    // 用原子来标记是否停止
    std::atomic<bool> bStop;
};

/**
 * 一个简单的队列线程池
 * - 可以指定 NumThreads
 * - 提供 AddQueuedWork() 往队列塞任务
 * - 内部每个 FQueuedThread 都执行 WaitForWork()，阻塞直到有新工作
 */
class FQueuedThreadPool {
   public:
    FQueuedThreadPool()
        : bIsStopping(false) {}

    ~FQueuedThreadPool() {
        Destroy();
    }

    // 初始化并创建指定数量的工作线程
    bool Create(uint32_t NumThreads, const std::string& Name = "MyThreadPool") {
        std::lock_guard<std::mutex> lock(QueueMutex);
        bIsStopping = false;

        Threads.reserve(NumThreads);
        for (uint32_t i = 0; i < NumThreads; i++) {
            // 创建一个工作线程
            std::unique_ptr<FQueuedThread> Worker(new FQueuedThread());
            std::string ThreadName = Name + "_" + std::to_string(i);

            if (!Worker->Create(this, ThreadName)) {
                std::cerr << "[FQueuedThreadPool] Failed to create thread: " << ThreadName << std::endl;
                return false;
            }
            Threads.push_back(std::move(Worker));
        }
        return true;
    }

    // 销毁线程池
    void Destroy() {
        {
            std::lock_guard<std::mutex> lock(QueueMutex);
            if (bIsStopping) {
                return;
            }
            bIsStopping = true;
        }

        // 通知所有线程停止
        for (auto& ThreadPtr : Threads) {
            ThreadPtr->StopThread();
        }

        // 广播一下，防止等待卡住
        WorkAvailable.notify_all();

        // 等待所有线程结束
        for (auto& ThreadPtr : Threads) {
            // 调用 FRunnableThread::WaitForCompletion()
            // （在 FQueuedThread 的析构或者Stop后，都要join）
        }
        Threads.clear();

        // 清理队列里剩余未执行的任务
        // 这些任务的 Abandon() 会被调用
        FlushQueue();
    }

    // 往队列里塞任务
    void AddQueuedWork(IQueuedWork* Work) {
        std::lock_guard<std::mutex> lock(QueueMutex);
        if (!bIsStopping) {
            WorkQueue.push(Work);
            WorkAvailable.notify_one();
        } else {
            // 如果正在停止，则不再接受新任务
            // 也可以立即调用 Work->Abandon();
            Work->Abandon();
        }
    }

    // 工作线程在此等待获取一个任务
    IQueuedWork* WaitForWork() {
        std::unique_lock<std::mutex> lock(QueueMutex);
        WorkAvailable.wait(lock, [this] {
            return bIsStopping || !WorkQueue.empty();
        });
        if (bIsStopping) {
            // 若在停止阶段，返回 nullptr
            return nullptr;
        }

        // 从队列弹出一个任务
        IQueuedWork* Work = WorkQueue.front();
        WorkQueue.pop();
        return Work;
    }

   private:
    // 清理队列中剩余的任务
    void FlushQueue() {
        while (!WorkQueue.empty()) {
            IQueuedWork* Work = WorkQueue.front();
            WorkQueue.pop();
            Work->Abandon();
        }
    }

   private:
    std::vector<std::unique_ptr<FQueuedThread>> Threads;
    std::queue<IQueuedWork*> WorkQueue;

    std::mutex QueueMutex;
    std::condition_variable WorkAvailable;
    bool bIsStopping;
};
