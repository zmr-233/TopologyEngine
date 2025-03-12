/****************************************
 * FILE: Engine/Include/ECS/System.hpp
 ****************************************/
#pragma once
#include <memory>
#include <set>

#include "ECSCore.hpp"
#include "EntityTypes.hpp"
#include "JobManager.hpp"

// 前置声明
class JobManagerProxy;
class ECSCore;  // 你已有的

/**
 * @brief 运行时(或初始化时)系统依赖模式
 */
enum class DepMode {
    // 强依赖: 必须等待依赖完成再执行
    Strong,
    // 可容忍延迟: 可以并行执行, 使用旧数据
    Weak,
    // 无依赖(忽略对方)
    None
};

/**
 * @brief SystemComp：描述该系统在ECS中的组件数据
 *   - systemName: 唯一标识(如"InputSys", "CameraSys")
 *   - initDeps: 初始化阶段所依赖的其他系统名
 *   - runDeps: 运行时依赖(示例为简化，可改成更复杂结构)
 *   - ... 其他配置字段(比如渲染上下文句柄)
 */
struct SystemComp {
    std::string systemName;
    std::vector<std::string> initDeps;  // 初始化依赖
    // 这里做个简化：假设是 "系统名称" -> DepMode
    std::unordered_map<std::string, DepMode> runDeps;

    // 可以放更多配置信息，比如:
    // void* renderContext = nullptr;
    // int someSetting = 0;
    // ...
};

/**
 * @brief System基类：采用状态机模型 + chunk并行
 * - 每帧update不会直接处理所有实体，而是拆分为多个Job
 * - 需要与JobManager交互
 */
class ISystem {
   public:
    virtual ~ISystem() = default;

    // 在系统初始化阶段被调用(只执行一次)
    virtual void onInit() = 0;

    // 每帧(或多帧)调用: 由 SystemManager 或主循环发起
    // 这里并不直接处理全部实体，而是打包提交到 JobManager
    virtual void onUpdate(float dt, JobManagerProxy& jmProxy) = 0;

    // 在系统被卸载时
    virtual void onDestroy() {}

    // 方便调试: 返回系统名
    virtual std::string getName() const = 0;
};

/**
 * @brief 一个示例的“System”实现基类，封装了对 ECSCore 的引用，支持 chunk 并行
 * 注意：你可以继续细化
 */
class SystemBase : public ISystem {
   public:
    SystemBase(std::shared_ptr<ECSCore> ecs, const SystemComp& sc)
        : mECS(ecs), mSysComp(sc) {}

    std::string getName() const override {
        return mSysComp.systemName;
    }

   protected:
    std::shared_ptr<ECSCore> mECS;
    SystemComp mSysComp;
};

// class System {
//    public:
//     std::set<Entity> mEntities;     // 这个系统关心的实体列表
//     std::shared_ptr<ECSCore> mECS;  // 指向ECSCore的指针

//     virtual ~System()             = default;
//     virtual void init()           = 0;
//     virtual void update(float dt) = 0;
// };
