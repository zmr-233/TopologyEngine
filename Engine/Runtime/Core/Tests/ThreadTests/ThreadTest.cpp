/******************************************************
 * @file ThreadTests/ThreadTest.cpp
 * @brief
 *****************************************************/

#include "Thread/ThreadPool.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

static void spinForMS(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 测试1：最简单的单任务测试
TEST(ThreadPoolTest, SingleTask) {
    ThreadPool       pool(2);
    std::atomic<int> counter{ 0 };

    // 提交一个简单任务
    pool.submit([&]() {
        // 模拟一点工作量
        spinForMS(50);
        counter.fetch_add(1, std::memory_order_relaxed);
    });

    // 等待所有任务完成
    pool.waitAll();

    EXPECT_EQ(counter.load(), 1) << "单任务没有被执行或执行结果不正确";
}

// 测试2：多个任务并发执行
TEST(ThreadPoolTest, MultipleTasks) {
    ThreadPool       pool(4);
    std::atomic<int> counter{ 0 };

    constexpr int taskCount = 20;
    for (int i = 0; i < taskCount; ++i) {
        pool.submit([&]() {
            spinForMS(10);
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }

    pool.waitAll();
    EXPECT_EQ(counter.load(), taskCount) << "多个并发任务没有全部被执行完成";
}

// 测试3：测试 waitAll 的同步性
// 提交多个任务后，如果不 waitAll，就可能观察到 counter < taskCount
TEST(ThreadPoolTest, WaitAll) {
    ThreadPool       pool(3);
    std::atomic<int> counter{ 0 };

    constexpr int taskCount = 5;
    for (int i = 0; i < taskCount; ++i) {
        pool.submit([&counter]() {
            spinForMS(5);
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }

    // 此时还没waitAll，counter可能还没到5
    int beforeWait = counter.load(std::memory_order_relaxed);
    // 不做强断言，但可以观察
    ASSERT_LE(beforeWait, taskCount);

    // 等待所有任务
    pool.waitAll();

    // 现在counter一定达到5
    EXPECT_EQ(counter.load(std::memory_order_relaxed), taskCount);
}

// 测试4：测试不同捕获方式的 Lambda
TEST(ThreadPoolTest, CaptureLambda) {
    ThreadPool pool(2);

    int              localValue = 10;
    std::atomic<int> sum{ 0 };

    pool.submit([localValue, &sum]() {
        spinForMS(10);
        sum.fetch_add(localValue, std::memory_order_relaxed);
    });

    int anotherValue = 20;
    pool.submit([&anotherValue, &sum]() {
        spinForMS(5);
        sum.fetch_add(anotherValue, std::memory_order_relaxed);
        anotherValue = 100; // 修改它
    });

    pool.waitAll();
    // 第一个任务让 sum += 10，第二个任务让 sum += 20 => sum=30
    EXPECT_EQ(sum.load(), 30);

    // anotherValue 被改成了100
    EXPECT_EQ(anotherValue, 100);
}

// 测试5：0或负数线程 => 应退化到1个线程
TEST(ThreadPoolTest, ZeroOrNegativeThread) {
    {
        ThreadPool       pool(0); // 期望自动创建1个线程
        std::atomic<int> counter{ 0 };
        pool.submit([&] {
            spinForMS(5);
            counter.fetch_add(1, std::memory_order_relaxed);
        });
        pool.waitAll();
        EXPECT_EQ(counter.load(), 1);
    }
    {
        ThreadPool       pool(-5); // 同理，也退化到1个线程
        std::atomic<int> counter{ 0 };
        pool.submit([&] {
            spinForMS(5);
            counter.fetch_add(1, std::memory_order_relaxed);
        });
        pool.waitAll();
        EXPECT_EQ(counter.load(), 1);
    }
}

// 测试6：压力测试
TEST(ThreadPoolTest, StressTest) {
    ThreadPool       pool(4);
    std::atomic<int> counter{ 0 };

    constexpr int taskCount = 1000;
    for (int i = 0; i < taskCount; ++i) {
        pool.submit([&counter]() {
            // 做简单的累加
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }
    pool.waitAll();
    EXPECT_EQ(counter.load(), taskCount);
}

// 测试7：重复调用 waitAll 不应该有问题
TEST(ThreadPoolTest, MultipleWaitAllCalls) {
    ThreadPool       pool(2);
    std::atomic<int> counter{ 0 };

    // 第一次提交几个任务
    for (int i = 0; i < 5; ++i) {
        pool.submit([&counter]() {
            spinForMS(1);
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }
    pool.waitAll();

    // 再次提交一些任务
    for (int i = 0; i < 5; ++i) {
        pool.submit([&counter]() {
            spinForMS(2);
            counter.fetch_add(1, std::memory_order_relaxed);
        });
    }
    // 多次调用 waitAll
    pool.waitAll();
    pool.waitAll(); // 再调一次也应该立即返回

    EXPECT_EQ(counter.load(), 10);
}

// 测试8：析构前自动等任务（如果设计如此）
// 一些线程池在析构时会等所有任务跑完才退出线程
// 如果你实现是这样，就可测试，否则你可以屏蔽此测试
TEST(ThreadPoolTest, DestructWaitAll) {
    std::atomic<int> counter{ 0 };
    {
        ThreadPool pool(2);
        for (int i = 0; i < 5; ++i) {
            pool.submit([&counter]() {
                spinForMS(10);
                counter.fetch_add(1, std::memory_order_relaxed);
            });
        }
        // 不手动调用 waitAll，就直接退出作用域
        // 若析构时不等待，就可能 counter < 5
    }
    // 如果析构做了等待，这里应该等到 counter=5
    EXPECT_EQ(counter.load(), 5);
}
