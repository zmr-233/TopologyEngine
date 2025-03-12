/******************************************************
 * FILE: Engine/Include/Utils/ThreadPool.hpp
 ******************************************************/
#pragma once
#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>

/**
 * @brief 帮助进行“初始化依赖”的拓扑排序工具
 * 这里做最简单的：对若干节点(系统)做有向图，若出现环则抛异常
 */
struct DepGraph {
    // adjacencyList[node] = { list of node that node depends on } 也可反着存
    // 这里用 “outEdges[node] = { successors }” 或 “inEdges”都行
    std::unordered_map<std::string, std::vector<std::string>> adjacency;
    //TODO: 使用trait+type_index，来泛化DepGraph

    // 收集节点 => 方便后续做拓扑排序
    std::unordered_set<std::string> allNodes;
};

inline std::vector<std::string> topologicalSort(const DepGraph& graph) {
    // 计算 in-degree
    std::unordered_map<std::string, int> inDegree;
    for (auto& node : graph.allNodes) {
        inDegree[node] = 0;
    }
    // adjacency: node -> [neighbors]
    // 这里假设 neighbors 表示 node->neighbors 依赖方向(可以自己定义)
    for (auto& [node, neighbors] : graph.adjacency) {
        for (auto& neigh : neighbors) {
            inDegree[neigh]++;
        }
    }

    // 找到 in-degree=0 的节点队列
    std::queue<std::string> zeroQ;
    for (auto& [n, deg] : inDegree) {
        if (deg == 0) zeroQ.push(n);
    }

    std::vector<std::string> result;
    result.reserve(graph.allNodes.size());

    while (!zeroQ.empty()) {
        auto n = zeroQ.front();
        zeroQ.pop();
        result.push_back(n);

        // 对所有后继节点 in-degree-1
        if (graph.adjacency.find(n) != graph.adjacency.end()) {
            for (auto& neigh : graph.adjacency.at(n)) {
                inDegree[neigh]--;
                if (inDegree[neigh] == 0) {
                    zeroQ.push(neigh);
                }
            }
        }
    }

    if (result.size() != graph.allNodes.size()) {
        throw std::runtime_error("Cycle detected in system initialization dependencies!");
    }
    return result;
}
