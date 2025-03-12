#include <chrono>
#include <iomanip>
#include <iostream>
#include <print>

#include "CLI11.hpp"

// 包含 "levelgraph.hpp" 里声明的 measure_time_xxx 等
#include "levelgraph.hpp"

int main(int argc, char* argv[]) {

    CLI::App app{"Graph Traversal Comparison"};

    unsigned num_threads{1};
    app.add_option("-t,--num_threads", num_threads, "number of threads (default=1)");

    unsigned num_rounds{1};
    app.add_option("-r,--num_rounds", num_rounds, "number of rounds (default=1)");

    // 新增一个 "tg" (TaskGraph) 模式
    std::string model = "tf";
    app.add_option("-m,--model", model, "model name tbb|omp|tf|tg (default=tf)")
        ->check([](const std::string& m) {
            if (m != "tbb" && m != "omp" && m != "tf" && m != "tg") {
                return "model name should be \"tbb\", \"omp\", \"tf\", or \"tg\"";
            }
            return "";
        });

    CLI11_PARSE(app, argc, argv);

    std::cout << "model=" << model << ' '
              << "num_threads=" << num_threads << ' '
              << "num_rounds=" << num_rounds << ' '
              << std::endl;

    // 打印表头
    std::cout << std::setw(12) << "|V|+|E|"  // 图规模
              << std::setw(12) << "Runtime"  // 耗时
              << '\n';

    // 让图的层数与每层节点数都从小到大增加
    for (int i = 1; i <= 451; i += 15) {
    // for (int i = 7; i <= 451; i += 2) {

        double runtime{0.0};

        LevelGraph graph(i, i);  // i x i 大小
        // std::println("Current ------------ Graph ({}, {})", i, i);
        

        // 重复多轮
        for (unsigned j = 0; j < num_rounds; ++j) {
            if (model == "tf") {
                runtime += measure_time_taskflow(graph, num_threads).count();
            } else if (model == "tbb") {
                runtime += measure_time_tbb(graph, num_threads).count();
            } else if (model == "omp") {
                runtime += measure_time_omp(graph, num_threads).count();
            } else if (model == "tg") {
                // ★ 调用我们新的 measure_time_taskgraph
                runtime += measure_time_taskgraph(graph, num_threads).count();
            }
            // 清除状态
            graph.clear_graph();
        }

        // 打印平均耗时(毫秒)
        double avg_us = runtime / num_rounds;
        double ms     = avg_us / 1e3;

        std::cout << std::setw(12) << graph.graph_size()
                  << std::setw(12) << ms
                  << std::endl;
    }

    return 0;
}
