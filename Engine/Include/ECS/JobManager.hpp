/****************************************
 * FILE: Engine/Include/ECS/JobManager.hpp
 ****************************************/
#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "../Utils/ThreadPool.hpp"

struct Job;

/**
 * @brief JobManager：管理Job的提交、依赖解析、调度到ThreadPool
 */
class JobManager {
   public:
    explicit JobManager(size_t threadCount);

    /**
     * @brief 提交一个不带任何依赖的Job
     */
    void submitJob(std::shared_ptr<Job> job);

    /**
     * @brief 对两个Job建立依赖关系： childJob 依赖 parentJob
     */
    void addDependency(std::shared_ptr<Job> parent, std::shared_ptr<Job> child);

    /**
     * @brief 等待所有Job结束（简单实现）
     */
    void waitAll();

   private:
    void scheduleJob(std::shared_ptr<Job> job);

   private:
    ThreadPool mPool;
    std::mutex mMutex;

    // 那些还未准备好(依赖未满足)的Job
    std::vector<std::shared_ptr<Job>> mPendingJobs;
};