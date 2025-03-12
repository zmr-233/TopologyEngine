/****************************************
 * FILE: Engine/Include/HAL/FBaseGraphTask.hpp
 ****************************************/

#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>

#include "FTaskGraphInterface.hpp"
#include "Log.hpp"

/***************************************************
 * 2) FBaseGraphTask (青春版，最简依赖管理)
 ***************************************************/
/**
 *  一个“新前端”风格的基础任务对象，用来替代老的FGraphEvent。
 *  - 引用计数管理（AddRef/Release）
 *  - 依赖计数 / 嵌套任务
 *  - 完成后可 Wait() 等待
 *
 * Engine/Source/Runtime/Core/Public/Tasks/TaskPrivate.h:167
 */

#include <atomic>

// ERROR: 当只有一个工作线程 就会容易锁死
static std::atomic<bool> noSubseq(false);

class FBaseGraphTask {
   public:
    // 打印元信息
    // virtual int Meta() {
    //     return -1;
    // }

    // 默认构造：给它一个初始引用计数=1
    FBaseGraphTask()
        : RefCount(1), bCompleted(false), PrerequisitesOutstanding(0) {}

    virtual ~FBaseGraphTask() {
        // 任务析构时，必须已经 completed，且 RefCount==0
        // (不会做 runtime check，但这是逻辑保证)
    }

    /*------------------ 引用计数 -------------------*/
    void AddRef() {
        RefCount.fetch_add(1, std::memory_order_relaxed);
    }
    void Release() {
        int old = RefCount.fetch_sub(1, std::memory_order_acq_rel);
        if (old == 1) {
            SetCompeted();
            // delete this;
        }
        // LOG("cur ref= {}", old - 1);
    }
    uint32_t GetRefCount() const { return RefCount.load(std::memory_order_relaxed); }

    /*------------------ 依赖管理 -------------------*/
    /**
     * 当我依赖某个前置任务InPrereq时:
     *   1) 我自己的 PrerequisitesOutstanding++
     *   2) InPrereq->AddSubsequent(this), 让前置在完成时能通知到我
     *   3) 我记录 InPrereq 以便在完成时 Release() 它
     */
    void AddPrerequisite(FBaseGraphTask* InPrereq) {
        if (!InPrereq) return;
        PrerequisitesOutstanding.fetch_add(1, std::memory_order_relaxed);
        InPrereq->AddSubsequent(this);
        InPrereq->AddRef();  // hold a reference
        Prerequisites.push_back(InPrereq);
    }

    void AddSubsequent(FBaseGraphTask* InSub) {
        std::lock_guard<std::mutex> lk(Mutex);
        InSub->AddRef();  // 父任务持有后置任务一份引用
        SubsequentTasks.push_back(InSub);
        // 在 caller 里 AddRef(InSub) or 由InSub->AddPrerequisite()调用
    }

    /**
     * 当前置任务完成时，会调用我(后置)的 DependencyResolved()
     * => 依赖数 -1; 若变0 => TryLaunch().
     */
    void DependencyResolved() {
        int old = PrerequisitesOutstanding.fetch_sub(1, std::memory_order_acq_rel);
        if (old == 1) {
            // =1 -> 0
            TryLaunch();
        }
    }

    /**
     * 依赖都满足后(或者FireAndForget)，就可以投递到 线程池/命名线程
     * 这里写成虚函数
     */
    virtual void TryLaunch() {
        // Default: just call OnReadyToExecute() if no prerequisites left
        if (PrerequisitesOutstanding.load(std::memory_order_acquire) == 0) {
            OnReadyToExecute();
        }
    }

    /**
     * 真正执行任务
     */
    virtual void ExecuteTask() = 0;

    /**
     * 执行完后需调用 MarkAsComplete()
     */
    void MarkAsComplete() {
        {
            std::lock_guard<std::mutex> lk(Mutex);
            bCompleted.store(true, std::memory_order_release);

            // 通知后置
            bool hasOne = false;
            for (auto* Sub : SubsequentTasks) {
                if (Sub->PrerequisitesOutstanding.load(std::memory_order_acquire) == 1) {
                    hasOne = false;
                }
                // LOG("Task completed, notify subsequent task.");
                Sub->DependencyResolved();
                // 我对后置也有一次 AddRef => Release
                // LOG("[MarkAsComplete] Release Subsequent");
                Sub->Release();

                // ERROR: 通知检测是否发生了锁死
                if (!hasOne && !SubsequentTasks.empty()) noSubseq.store(false);
            }
            SubsequentTasks.clear();

            // 释放前置
            for (auto* Pre : Prerequisites) {
                // LOG("[MarkAsComplete] Release Prerequisites");
                Pre->Release();
            }
            Prerequisites.clear();
        }
        CV.notify_all();
    }

    /**
     * 若需要同步等任务完成
     */
    void Wait() {
        // LOG("[Wait] ID={}", Meta());
        std::unique_lock<std::mutex> lk(Mutex);
        CV.wait(lk, [&] { return bCompleted.load(std::memory_order_acquire); });
    }

    bool IsCompleted() const {
        return bCompleted.load(std::memory_order_acquire);
    }

    bool SetCompeted() {
        // LOG("[SetCompeted] ID={}", Meta());
        bCompleted.store(true, std::memory_order_release);
        return true;
    }

   protected:
    // 当可以执行时，子类/外部可重写
    virtual void OnReadyToExecute() {
        ExecuteTask();
    }

   private:
    // refcount
    std::atomic<int> RefCount;

    // completed?
    std::atomic<bool> bCompleted;

    // 记录依赖数量
    std::atomic<int> PrerequisitesOutstanding;

    // 记录前置任务(用于完成时释放它们)
    std::vector<FBaseGraphTask*> Prerequisites;
    // 记录后置任务(完成后要通知)
    std::vector<FBaseGraphTask*> SubsequentTasks;

    // for Wait()
    std::mutex Mutex;
    std::condition_variable CV;
};
