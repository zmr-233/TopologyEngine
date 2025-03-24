/****************************************
 * FILE: Engine/Include/HAL/FNamedTaskThread.hpp
 ****************************************/

#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

#include "FBaseGraphTask.hpp"
#include "FRunnable.hpp"
#include "Log.hpp"
// #include "TGraphTask.hpp"

/**
 * FNamedTaskThread：代表一个“命名线程”，继承 FRunnable。
 * - 内部维护两个队列：MainQueue + LocalQueue
 * - 不断轮询，从队列里取 Task 执行
 * - 运行在一个单独的 FRunnableThread 上
 */
class FNamedTaskThread : public FRunnable {
   public:
    FNamedTaskThread(uint32_t InIndex, const std::string& InName)
        : ThreadIndex(InIndex), ThreadName(InName), bStop(false) {}
    virtual ~FNamedTaskThread() {}

    // FRunnable
    virtual bool Init() override {
        return true;
    }

    virtual uint32_t Run() override {
        while (!bStop.load()) {
            FBaseGraphTask* nextTask = WaitForTask();
            if (!nextTask)
                break;

            // 执行
            nextTask->ExecuteTask();
            nextTask->Release();
        }
        return 0;
    }

    virtual void Stop() override {
        bStop.store(true);
        QueueCV.notify_all();
    }

    virtual void Exit() override {
        // clean up
    }

    // 往此线程的队列里添加任务
    void Enqueue(FBaseGraphTask* InTask, bool bLocalQueue) {
        // 这里简单用一把锁保证队列安全
        InTask->AddRef();  // 存疑？
        std::lock_guard<std::mutex> lk(QueueMutex);
        if (bLocalQueue)
            LocalQueue.push(InTask);
        else
            MainQueue.push(InTask);
        QueueCV.notify_one();
    }

    uint32_t GetThreadIndex() const { return ThreadIndex; }

   private:
    FBaseGraphTask* WaitForTask() {
        std::unique_lock<std::mutex> lk(QueueMutex);

        QueueCV.wait(lk, [this] {
            return bStop.load() || !MainQueue.empty() || !LocalQueue.empty();
        });
        if (bStop.load())
            return nullptr;

        // 先尝试 LocalQueue，再尝试 MainQueue
        // 也可以你自己定义优先级策略
        if (!LocalQueue.empty()) {
            FBaseGraphTask* Task = LocalQueue.front();
            LocalQueue.pop();
            return Task;
        } else if (!MainQueue.empty()) {
            FBaseGraphTask* Task = MainQueue.front();
            MainQueue.pop();
            return Task;
        }
        return nullptr;
    }

   private:
    uint32_t ThreadIndex;
    std::string ThreadName;

    std::atomic<bool> bStop;
    std::mutex QueueMutex;
    std::condition_variable QueueCV;

    // 两个队列
    std::queue<FBaseGraphTask*> MainQueue;
    std::queue<FBaseGraphTask*> LocalQueue;
};
