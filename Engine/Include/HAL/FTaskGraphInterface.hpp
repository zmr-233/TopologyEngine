/****************************************
 * FILE: Engine/Include/HAL/FTaskGraphInterface.hpp
 ****************************************/

#pragma once
#include <memory>
#include <vector>

#include "ENamedThreads.hpp"
#include "FBaseGraphTask.hpp"
#include "FGraphEventRef.hpp"
#include "FNamedTaskThread.hpp"

// 供外部使用
class FTaskGraphInterface {
   public:
    virtual ~FTaskGraphInterface() {}

    static FTaskGraphInterface& Get();

    static void Startup(int WorkerCount = 4);
    static void Shutdown();
    static bool IsRunning();

    // 把任务投递到某线程
    virtual void QueueTask(FBaseGraphTask* Task, ENamedThreads::Type ThreadToExecuteOn) = 0;
    virtual bool IsProcessingTasks() const                                              = 0;

    // 等待
    virtual void WaitUntilTaskCompletes(const FGraphEventRef& Task)               = 0;
    virtual void WaitUntilTasksComplete(const std::vector<FGraphEventRef>& Tasks) = 0;
};

// 具体实现
class FTaskGraphImplementation : public FTaskGraphInterface {
   public:
    FTaskGraphImplementation() : bStopping(false) {}
    virtual ~FTaskGraphImplementation() {
        ShutdownAll();
    }

    void StartupAll(int WorkerCount) {
        // 建立命名线程(仅演示GameThread=0,RenderThread=1,AudioThread=2)
        // NamedThreads.emplace(0, new FNamedTaskThread(0, "GameThread"));
        // NamedThreads.emplace(1, new FNamedTaskThread(1, "RenderThread"));
        // NamedThreads.emplace(2, new FNamedTaskThread(2, "AudioThread"));

        // 启动
        // for (auto& kv : NamedThreads) {
        //     ThreadHandles.emplace_back(std::thread([=]() {
        //         kv.second->Run();
        //     }));
        // }

        // 后台
        StopWorkers = false;
        for (int i = 0; i < WorkerCount; i++) {
            WorkerThreads.emplace_back([this]() {
                WorkerLoop();
            });
        }
    }

    void ShutdownAll() {
        if (bStopping.exchange(true)) return;

        // stop named
        for (auto& kv : NamedThreads) {
            kv.second->Stop();
        }
        for (auto& t : ThreadHandles) {
            if (t.joinable()) t.join();
        }
        for (auto& kv : NamedThreads) {
            delete kv.second;
        }
        NamedThreads.clear();
        ThreadHandles.clear();

        // stop worker
        {
            std::lock_guard<std::mutex> lk(WorkerMtx);
            StopWorkers = true;
            // LOG("StopWorkers = true");
        }
        WorkerCv.notify_all();
        for (auto& wt : WorkerThreads) {
            if (wt.joinable()) wt.join();
        }
        WorkerThreads.clear();
    }

    // ========== override =============
    virtual void QueueTask(FBaseGraphTask* Task, ENamedThreads::Type ThreadToExecuteOn) override {
        if (!Task) return;

        if (ENamedThreads::IsAnyThread(ThreadToExecuteOn)) {
            // 后台
            std::lock_guard<std::mutex> lk(WorkerMtx);
            // Task->AddRef();  // hold 1
            WorkerQueue.push(Task);
            WorkerCv.notify_one();
        } else {
            // Named
            bool local   = ENamedThreads::IsLocalQueue(ThreadToExecuteOn);
            uint32_t idx = ENamedThreads::GetThreadIndex(ThreadToExecuteOn);
            auto it      = NamedThreads.find(idx);
            if (it != NamedThreads.end()) {
                it->second->Enqueue(Task, local);
            } else {
                // fallback: 同步执行
                Task->ExecuteTask();
            }
        }
    }
    virtual bool IsProcessingTasks() const override {
        return !WorkerQueue.empty();
    }

    virtual void WaitUntilTaskCompletes(const FGraphEventRef& Task) override {
        if (Task) Task->Wait();
    }
    virtual void WaitUntilTasksComplete(const std::vector<FGraphEventRef>& Tasks) override {
        int i = 0;
        for (auto& e : Tasks) {
            // LOG("WaitUntilTasksComplete CNT: {}", i++);
            if (e) {
                e->Wait();
            }
        }
    }

   private:
    void WorkerLoop() {
        while (true) {
            // LOG("WorkerLoop");
            FBaseGraphTask* front = nullptr;
            {
                std::unique_lock<std::mutex> lk(WorkerMtx);
                WorkerCv.wait(lk, [this] {
                    return StopWorkers || !WorkerQueue.empty() || noSubseq.load();
                });

                // ERROR: 当只有一个工作线程 就会容易锁死
                if (!StopWorkers && noSubseq.load() && !WorkerQueue.empty()) {
                    noSubseq.store(false);
                    continue;
                    ;
                } else if (noSubseq.load() && WorkerQueue.empty()) {
                    assert(false);
                }
                if (StopWorkers && WorkerQueue.empty())
                    return;
                front = WorkerQueue.front();
                WorkerQueue.pop();
            }
            // LOG("WorkerLoop: ExecuteTask");
            // LOG("[Workerloop::ExecuteTask] ID={}", front->Meta());
            front->ExecuteTask();
            // LOG("WorkerLoop: Release");
            front->Release();
        }
    }

    //    private:
    std::atomic<bool> bStopping;

    // Named
    std::unordered_map<uint32_t, FNamedTaskThread*> NamedThreads;
    std::vector<std::thread> ThreadHandles;

    // Worker
    bool StopWorkers = false;
    std::vector<std::thread> WorkerThreads;
    std::queue<FBaseGraphTask*> WorkerQueue;
    std::mutex WorkerMtx;
    std::condition_variable WorkerCv;
};

// 全局静态
static FTaskGraphImplementation* GImpl = nullptr;
static bool GRunning                   = false;