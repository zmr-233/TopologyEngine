/****************************************
 * FILE: Engine/Source/Benchmark/graph_traversal/taskgraph.cpp
 ****************************************/

#include <chrono>
#include <iostream>
#include <print>
#include <vector>

#include "levelgraph.hpp"

// 这是你自定义的 TaskGraph 头文件/接口（需自行包含）
#include <algorithm>
#include <functional>
#include <mutex>
#include <print>
#include <utility>

#include "ENamedThreads.hpp"
#include "FTaskGraphInterface.hpp"
#include "Log.hpp"
#include "TGraphTask.hpp"

// static std::mutex gMutex;
using FGraphEventArray = std::vector<FGraphEventRef>;

struct TG {
    TG(LevelGraph& graph, unsigned num_threads)
        : tasks({}) {

        // LOG("Graph size: {} {}", graph.level(), graph.length());

        // graph.print_graph();
        // LOG("-------------------------")

        FTaskGraphInterface::Startup(num_threads);

        trigger = FFunctionGraphTask::CreateAndHold([]() {});

        tasks.resize(graph.level());
        std::for_each(tasks.begin(), tasks.end(), [&](auto& t) {
            t.resize(graph.length());
        });

        for (size_t i = 0; i < graph.length(); i++) {
            Node& n = graph.node_at(0, i);
            // LOG("Graph[{}][{}]::Node", n.level(), n.index());
            // n.print_node();
            tasks[0][i] =
                FFunctionGraphTask::CreateAndHold([&n]() {
                    // std::lock_guard lg(gMutex);
                    // n.print_node();
                    n.mark();
                },
                                                  trigger);
        }

        if (graph.level() <= 1) {
            return;
        }
        for (int i = 1; i < graph.level(); i++) {
            for (size_t j = 0; j < graph.length(); j++) {
                Node& n = graph.node_at(i, j);
                // LOG("Graph[{}][{}]::Node", n.level(), n.index());
                // n.print_node();

                if (n._in_edges.size() == 0) {
                    if (i <= graph.level() - 2) {
                        tasks[i][j] =
                            FFunctionGraphTask::CreateAndHold([&n]() {
                                // std::lock_guard lg(gMutex);
                                // n.print_node();
                                n.mark();
                            },
                                                              trigger);
                    }
                } else {
                    // 构建前驱依赖
                    FGraphEventArray prereqs;
                    for (const auto& edge : n._in_edges) {
                        int parent_index = edge.first;  // 也就是上一层节点的 index
                        // int parent_edge_idx = edge.second; // 其实对调度没用

                        // 父节点对应的任务对象
                        auto parent_task = tasks[i - 1][parent_index];
                        prereqs.push_back(parent_task);
                    }

                    // 构建当前节点的任务
                    tasks[i][j] = FFunctionGraphTask::CreateAndHold(
                        [&n]() {
                            // std::lock_guard lg(gMutex);
                            n.mark();
                        },
                        &prereqs);
                }
            }
        }
    }

    void run() {
        FFunctionGraphTask::Dispatch(trigger);
        FTaskGraphInterface::Get().WaitUntilTasksComplete(tasks.back());
        FTaskGraphInterface::Shutdown();
    }
    FGraphEventRef trigger;
    std::vector<std::vector<FGraphEventRef>> tasks;
};

void traverse_level_graph_taskgraph(LevelGraph& graph, unsigned num_threads) {
    TG tg(graph, num_threads);
    tg.run();
}

std::chrono::microseconds measure_time_taskgraph(LevelGraph& graph, unsigned num_threads) {
    TG tg(graph, num_threads);
    auto beg = std::chrono::high_resolution_clock::now();
    // traverse_level_graph_taskgraph(graph, num_threads);
    tg.run();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
}