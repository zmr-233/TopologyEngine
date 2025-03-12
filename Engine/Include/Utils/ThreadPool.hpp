/******************************************************
 * FILE: Engine/Include/Utils/ThreadPool.hpp
 ******************************************************/

#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * @brief 一个最简单的线程池实现（可改进：自定义任务队列、优先级、工作窃取等）
 */
class ThreadPool {
   public:
    explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency()) {
        if (threadCount == 0) threadCount = 1;
        mStop = false;
        for (size_t i = 0; i < threadCount; ++i) {
            mWorkers.emplace_back([this] {
                // 工作线程函数
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mQueueMutex);
                        // 等待有任务或关闭
                        mCondition.wait(lock, [this] { return mStop || !mTasks.empty(); });
                        if (mStop && mTasks.empty()) {
                            return;  // 退出线程
                        }
                        task = std::move(mTasks.front());
                        mTasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mStop = true;
        }
        mCondition.notify_all();
        for (auto& w : mWorkers) {
            if (w.joinable()) {
                w.join();
            }
        }
    }

    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnT = std::invoke_result_t<F, Args...>;
        auto taskPtr  = std::make_shared<std::packaged_task<ReturnT()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mTasks.push([taskPtr]() { (*taskPtr)(); });
        }
        mCondition.notify_one();
        return taskPtr->get_future();
    }

   private:
    std::vector<std::thread> mWorkers;
    std::queue<std::function<void()>> mTasks;
    std::mutex mQueueMutex;
    std::condition_variable mCondition;
    bool mStop = false;
};
