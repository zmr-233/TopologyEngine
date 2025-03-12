/****************************************
 * FILE: Include/ECS/SystemManager.hpp
 ****************************************/

#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>

class ECSCore;
class ISystem;
class JobManager;

/**
 * @brief SystemManager：管理所有系统(作为Entity+SystemComp)
 *   - 负责初始化依赖分析(拓扑排序)
 *   - 负责构造对应的 ISystem 实例(可使用 SystemFactory)
 *   - 每帧 update 时，依次调用 system->onUpdate(...)
 */
struct SystemComp;
class SystemManager {
   public:
    // 传入一个JobManager的引用, 供系统在 update 时提交并行任务
    SystemManager(JobManager& jm, std::shared_ptr<ECSCore> ecs)
        : mJobManager(jm), mECS(ecs) {}

    /**
     * @brief 注册一个“系统类型”对应的工厂函数
     *  比如 "InputSys" -> lambda(...) { return std::make_shared<InputSystem>(...); }
     */
    void registerSystemFactory(const std::string& systemName,
                               std::function<std::shared_ptr<ISystem>(const SystemComp&)> factory);

    /**
     * @brief 初始化所有System (扫描 ECS 里所有拥有 SystemComp 的实体, 生成系统实例)
     *  - 读取 SystemComp -> systemName, initDeps
     *  - 构建依赖图 -> 拓扑排序
     *  - 按顺序调用 onInit()
     */
    void initAllSystems();
    
    /**
     * @brief 每帧更新系统
     *   - 暂时简单写: 强行按init顺序调用 onUpdate()
     *   - onUpdate会自己向 JobManager 提交并行任务
     */
    void updateAllSystems(float dt);

    /**
     * @brief 结束时: onDestroy
     */
    void destroyAllSystems();

   private:
    JobManager& mJobManager;
    std::shared_ptr<ECSCore> mECS;

    // SystemFactory: systemName -> lambda(SystemComp) -> ISystem
    std::unordered_map<std::string, std::function<std::shared_ptr<ISystem>(const SystemComp&)>> mFactories;

    // 按拓扑顺序存储系统实例
    std::vector<std::shared_ptr<ISystem>> mSystems;
};
