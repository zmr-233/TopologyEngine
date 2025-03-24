/****************************************
 * FILE: Engine/Include/HAL/TGraphTask.hpp
 ****************************************/
#pragma once
#include <cassert>
#include <concepts>
#include <functional>
#include <new>  // for placement new
#include <print>
#include <utility>

#include "FBaseGraphTask.hpp"
#include "FTaskGraphInterface.hpp"
#include "Log.hpp"

struct FTaskGraphLambdaImpl {
    std::function<void()> Work;

    // 统一风格: DoTask
    void DoTask() {
        if (Work) Work();
    }
};

//-------------------------------------
// 1) TGraphTask: 模板封装
//-------------------------------------
template <typename TTaskType>
class TGraphTask : public FBaseGraphTask {
   public:
    TGraphTask(ENamedThreads::Type InThread)
        : DesiredThread(InThread) {
    }

    virtual void ExecuteTask() override {
        // 强转
        TTaskType* p = reinterpret_cast<TTaskType*>(&Storage);

        p->DoTask();
        // 析构
        p->~TTaskType();
        // done
        MarkAsComplete();
    }

   protected:
    // OnReadyToExecute => 把自己投递到 FTaskGraphInterface
    virtual void OnReadyToExecute() override {
        FTaskGraphInterface::Get().QueueTask(this, DesiredThread);
    }

   public:
    template <typename... Args>
    void ConstructTask(Args&&... a) {
        new (&Storage) TTaskType(std::forward<Args>(a)...);
    }

   private:
    ENamedThreads::Type DesiredThread;
    std::aligned_storage_t<sizeof(TTaskType), alignof(TTaskType)> Storage;
};

//-------------------------------------
// 2) FFunctionGraphTask
//-------------------------------------
class FFunctionGraphTask {
   public:
    /**
     * CreateAndDispatchWhenReady
     *
     * @param InFunction   你要执行的lambda
     * @param InPrereqs    (可选) 前置依赖
     * @param InThread     目标线程(AnyThread / GameThread / RenderThread ...)
     * @return 任务事件FGraphEventRef
     */
    static FGraphEventRef CreateAndDispatchWhenReady(std::function<void()> InFunction,
                                                     const std::vector<FGraphEventRef>* InPrereqs = nullptr,
                                                     ENamedThreads::Type InThread                 = ENamedThreads::AnyThread) {
        // 1) new出 TGraphTask<FTaskGraphLambdaImpl>
        auto* rawPtr = new TGraphTask<FTaskGraphLambdaImpl>(InThread);

        // 2) 若有前置依赖 => AddPrerequisite
        if (InPrereqs) {
            for (auto& p : *InPrereqs) {
                if (p) rawPtr->AddPrerequisite(p.GetReference());
            }
        }
        // 3) 构造
        rawPtr->ConstructTask(FTaskGraphLambdaImpl{InFunction});

        // 4) 交给 FGraphEventRef 管理
        FGraphEventRef ev(rawPtr);

        // 5) 直接 TryLaunch
        rawPtr->TryLaunch();

        return ev;
    }

    // 创建但不直接TryLaunch
    /**
     * @brief Create a And Hold object
     *
     * 注意: 必须要调用 FGraphEventRef::Dispatch() 才会执行
     *
     * @param InFunction
     * @param InPrereqs
     * @param InThread
     * @return FGraphEventRef
     */
    static FGraphEventRef CreateAndHold(std::function<void()> InFunction,
                                        const std::vector<FGraphEventRef>* InPrereqs = nullptr,
                                        ENamedThreads::Type InThread                 = ENamedThreads::AnyThread) {
        auto* rawPtr = new TGraphTask<FTaskGraphLambdaImpl>(InThread);

        // 先挂前置依赖
        if (InPrereqs) {
            for (auto& p : *InPrereqs) {
                if (p) {
                    rawPtr->AddPrerequisite(p.GetReference());
                }
            }
        }
        // 构造
        rawPtr->ConstructTask(FTaskGraphLambdaImpl{InFunction});

        // 返回引用计数封装
        return FGraphEventRef(rawPtr);
    }

    static FGraphEventRef CreateAndHold(std::function<void()> InFunction,
                                        FGraphEventRef InPrereq,
                                        ENamedThreads::Type InThread = ENamedThreads::AnyThread) {
        auto* rawPtr = new TGraphTask<FTaskGraphLambdaImpl>(InThread);

        rawPtr->AddPrerequisite(InPrereq.GetReference());

        // 构造
        rawPtr->ConstructTask(FTaskGraphLambdaImpl{InFunction});

        // 返回引用计数封装
        return FGraphEventRef(rawPtr);
    }

    static void Dispatch(const FGraphEventRef& Ev) {
        if (Ev) {
            Ev.GetReference()->TryLaunch();
        }
    }
};