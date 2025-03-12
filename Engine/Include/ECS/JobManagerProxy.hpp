/****************************************
 * FILE: Engine/Include/ECS/JobManager.hpp
 ****************************************/
#pragma once

#include "JobManager.hpp"
#include "JobType.hpp"
#include "memory"

/**
 * @brief JobManagerProxy：在 onUpdate(...) 中辅助提交chunk任务，自动加运行时依赖
 *   - 简化演示：只演示"强依赖"->必须等待, "弱依赖"->不等待
 */
class JobManagerProxy {
   public:
    explicit JobManagerProxy(JobManager& jm)
        : mJobManager(jm) {}

    // 提交一个Job(不带运行时依赖)
    std::shared_ptr<Job> submitJob(const std::string& debugName, std::function<void()> fn) {
        auto j       = std::make_shared<Job>();
        j->work      = fn;
        j->debugName = debugName;
        j->dependencyCount.store(0);
        mJobManager.submitJob(j);
        return j;
    }

    // 在这个演示里, 如果要强依赖某个Job, 我们就把child的依赖计数+1
    void addDependency(std::shared_ptr<Job> parent, std::shared_ptr<Job> child) {
        mJobManager.addDependency(parent, child);
    }

   private:
    JobManager& mJobManager;
};
