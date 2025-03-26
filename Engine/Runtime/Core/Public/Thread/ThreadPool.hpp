/******************************************************
 * @file Thread/ThreadPool.hpp
 * @brief
 *****************************************************/

#pragma once

#include <functional>
#include <memory>

class ThreadPool {
  public:
    // 创建指定数量线程
    explicit ThreadPool(int numThreads);

    // 禁止拷贝和赋值
    ThreadPool(const ThreadPool &)            = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    // 析构时需要清理线程资源
    ~ThreadPool();

    // 提交一个任务，任务是一个无参可调用对象
    void submit(std::function<void()> task);

    // 等待所有已经提交的任务执行完毕
    void waitAll();

  private:
    // 前向声明，不需要暴露实现细节到头文件
    struct ThreadPoolImpl;
    // 通过 unique_ptr 来管理内部实现
    std::unique_ptr<ThreadPoolImpl> impl_;
};
