/****************************************
 * FILE: Engine/Include/ECS/JobType.hpp
 ****************************************/
#pragma once

#include <atomic>
#include <functional>
#include <string>
/**
 * @brief Job结构：描述一次要执行的工作
 * 这里用一个 lambda/function + 可选的依赖计数 来代表最小示例
 */
struct Job {
    std::function<void()> work;
    // 如果需要在JobManager里做简单的“依赖计数”，可以加一个计数器
    // 当依赖计数==0时才能执行
    std::atomic<int> dependencyCount{0};

    // 可视化或调试用的名字
    std::string debugName;
};