/****************************************
 * FILE: Engine/Include/HAL/IQueuedWork.hpp
 ****************************************/

#pragma once

/**
 * 类似 IQueuedWork，用于线程池执行
 */
class IQueuedWork {
   public:
    virtual ~IQueuedWork() {}

    // 由线程池的工作线程调用
    virtual void DoThreadedWork() = 0;

    // 如果该任务在执行前就放弃（被从队列中移除），可以在这里做资源清理
    virtual void Abandon() {}
};
