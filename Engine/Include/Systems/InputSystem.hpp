/****************************************
 * FILE: Engine/Include/Systems/InputSystem.hpp
 ****************************************/
#pragma once
#include "../Comps/Sys/InputSysComp.hpp"
#include "../ECS/JobManagerProxy.hpp"
#include "../ECS/System.hpp"

class InputSystem final : public SystemBase {
   public:
    // 构造: 传入 ECSCore & 该系统的 SystemComp
    InputSystem(std::shared_ptr<ECSCore> ecs, const InputSysComp& sc)
        : SystemBase(ecs, sc) {
        // 把 SystemComp 的数据转存
        // mWindowHandles = sc.windows;
        // ...
    }

    void onInit() override {
        std::cout << "[InputSystem] onInit\n";
        // 这里执行一次性的初始化: 订阅EventBus, 设置回调等
        // ...
    }

    void onUpdate(float dt, JobManagerProxy& jmProxy) override {
        // **示例**：只做简单逻辑(无实体处理)
        // 如果有实体相关操作，可以把关心的实体 chunk 化

        // 1) 提交一个Job, 做 pollEvent / 采集输入
        auto job1 = jmProxy.submitJob("InputSystem_Poll", [dt]() {
            // 这里执行 pollEvents
            // std::cout << "[InputSystem] Polling...\n";
            // ...
        });
        // 2) 提交一个Job, 处理输入映射
        auto job2 = jmProxy.submitJob("InputSystem_Map", [dt]() {
            // std::cout << "[InputSystem] Key->Axes mapping...\n";
            // ...
        });

        // 如果你有 runDeps 需要强同步(比如依赖某AI系统的写入？)
        // 你可以在 SystemManager 或 JobManager 层自动生成 addDependency
        // 这里演示手动
        // jmProxy.addDependency(job1, job2);
    }

    void onDestroy() override {
        // 释放资源, 注销回调
        std::cout << "[InputSystem] onDestroy\n";
    }
};
