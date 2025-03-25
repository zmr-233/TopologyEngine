/******************************************************
 * @file Thread/ThreadPool_linux.cpp
 * @brief
 *****************************************************/

#include "Thread/ThreadPool.hpp"

#ifdef __linux__

#include <pthread.h>
#include <unistd.h>

#include <atomic>
#include <functional>
#include <queue>
#include <stdexcept>
#include <vector>

struct ThreadPool::ThreadPoolImpl {
    int                               threadCount;
    std::vector<pthread_t>            threads;
    std::queue<std::function<void()>> tasks;

    pthread_mutex_t mutex;
    pthread_cond_t  cond;          // 用来唤醒工作线程
    pthread_cond_t  condAllDone;   // 用来等待所有任务完成

    std::atomic<bool> stop;
    std::atomic<int>  activeCount; // 当前正在执行的任务数

    ThreadPoolImpl(int numThreads)
        : threadCount(numThreads), threads(numThreads), stop(false), activeCount(0) {
        if (pthread_mutex_init(&mutex, nullptr) != 0) {
            throw std::runtime_error("pthread_mutex_init failed");
        }
        if (pthread_cond_init(&cond, nullptr) != 0) {
            pthread_mutex_destroy(&mutex);
            throw std::runtime_error("pthread_cond_init failed");
        }
        if (pthread_cond_init(&condAllDone, nullptr) != 0) {
            pthread_cond_destroy(&cond);
            pthread_mutex_destroy(&mutex);
            throw std::runtime_error("pthread_cond_init condAllDone failed");
        }

        // 创建线程
        for (int i = 0; i < threadCount; ++i) {
            if (pthread_create(&threads[i], nullptr, &ThreadPoolImpl::workerThread, this) != 0) {
                // 如果创建失败，需要清理
                stop = true;
                pthread_cond_broadcast(&cond);
                for (int j = 0; j < i; ++j) {
                    pthread_join(threads[j], nullptr);
                }
                pthread_cond_destroy(&condAllDone);
                pthread_cond_destroy(&cond);
                pthread_mutex_destroy(&mutex);
                throw std::runtime_error("pthread_create failed");
            }
        }
    }

    ~ThreadPoolImpl() {
        // 通知所有线程停止
        stop = true;
        pthread_cond_broadcast(&cond);

        // 等待所有线程结束
        for (int i = 0; i < threadCount; ++i) {
            pthread_join(threads[i], nullptr);
        }
        pthread_cond_destroy(&condAllDone);
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mutex);
    }

    static void *workerThread(void *arg) {
        ThreadPoolImpl *impl = static_cast<ThreadPoolImpl *>(arg);
        impl->threadLoop();
        return nullptr;
    }

    void threadLoop() {
        while (true) {
            std::function<void()> task;

            pthread_mutex_lock(&mutex);

            while (!stop && tasks.empty()) {
                pthread_cond_wait(&cond, &mutex);
            }

            // 如果停止了且没有任务，直接退出
            if (stop && tasks.empty()) {
                pthread_mutex_unlock(&mutex);
                break;
            }

            // 从队列取任务
            task = std::move(tasks.front());
            tasks.pop();
            // 取到任务后，activeCount++，表示我们开始执行一个任务
            activeCount.fetch_add(1, std::memory_order_relaxed);

            pthread_mutex_unlock(&mutex);

            // 执行任务
            task();

            // 任务执行完了，activeCount--
            int stillActive = activeCount.fetch_sub(1, std::memory_order_relaxed) - 1;

            // 如果此时 activeCount == 0，说明没有正在执行的任务了
            // 但也要考虑队列中还有没有剩余任务。不过，对于 waitAll 而言，
            // “执行完的任务”减少了 activeCount，就有机会唤醒 waitAll
            // 注意：这里要再次加锁再发signal，避免和 waitAll 的锁冲突
            pthread_mutex_lock(&mutex);
            if (stillActive == 0 && tasks.empty()) {
                // 所有任务都执行完了，可以唤醒 waitAll
                pthread_cond_broadcast(&condAllDone);
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    void enqueueTask(std::function<void()> f) {
        pthread_mutex_lock(&mutex);
        tasks.push(std::move(f));
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }

    void waitAllTasksDone() {
        // 等待“队列为空 且 activeCount == 0”
        pthread_mutex_lock(&mutex);
        while (!tasks.empty() || activeCount.load(std::memory_order_relaxed) > 0) {
            pthread_cond_wait(&condAllDone, &mutex);
            // 被唤醒后再检查是否真的都结束
        }
        pthread_mutex_unlock(&mutex);
    }
};

// ============== ThreadPool 对外接口实现 =============

ThreadPool::ThreadPool(int numThreads) {
    if (numThreads <= 0) {
        numThreads = 1;
    }
    impl_ = std::make_unique<ThreadPoolImpl>(numThreads);
}

ThreadPool::~ThreadPool() = default;

void ThreadPool::submit(std::function<void()> task) {
    impl_->enqueueTask(std::move(task));
}

void ThreadPool::waitAll() {
    impl_->waitAllTasksDone();
}

#endif // __linux__
