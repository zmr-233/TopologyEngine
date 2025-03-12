/****************************************
 * FILE: Engine/Source/ECS/SystemManager.cpp
 ****************************************/
// #include "../../Include/ECS/SystemManager.hpp"

// #include <memory>
// #include <typeindex>
// #include <unordered_map>

// #include "../../Include/ECS/ECSCore.hpp"
// #include "../../Include/ECS/JobManagerProxy.hpp"
// #include "../../Include/ECS/System.hpp"
// #include "../../Include/Utils/DepGraph.hpp"
// #include "../../Include/Utils/Log.hpp"

// // 传入一个JobManager的引用, 供系统在 update 时提交并行任务
// SystemManager::SystemManager(JobManager& jm, std::shared_ptr<ECSCore> ecs)
//     : mJobManager(jm), mECS(ecs) {}

// /**
//  * @brief 注册一个“系统类型”对应的工厂函数
//  *  比如 "InputSys" -> lambda(...) { return std::make_shared<InputSystem>(...); }
//  */
// void SystemManager::registerSystemFactory(const std::string& systemName,
//                                           std::function<std::shared_ptr<ISystem>(const SystemComp&)> factory) {
//     mFactories[systemName] = factory;
// }

// /**
//  * @brief 初始化所有System (扫描 ECS 里所有拥有 SystemComp 的实体, 生成系统实例)
//  *  - 读取 SystemComp -> systemName, initDeps
//  *  - 构建依赖图 -> 拓扑排序
//  *  - 按顺序调用 onInit()
//  */
// void SystemManager::initAllSystems() {
//     // 1. 收集所有 SystemComp
//     std::vector<Entity> systemEntities;
//     // 这里假设 ECSCore 有个 getAllEntities()，或你自己做法
//     // 下面是示例:
//     for (Entity e = 1; e < 5000; ++e) {
//         if (mECS->hasComp<SystemComp>(e)) {
//             systemEntities.push_back(e);
//         }
//     }
//     // TODO: 太弱智了 这里想办法优化

//     // 2. 构建 DepGraph
//     DepGraph depGraph;
//     for (auto e : systemEntities) {
//         auto& sc = mECS->getComp<SystemComp>(e);
//         depGraph.allNodes.insert(sc.systemName);
//     }
//     for (auto e : systemEntities) {
//         auto& sc = mECS->getComp<SystemComp>(e);
//         // sc.initDeps 表示 "我依赖的系统列表"
//         // 即 sc.initDeps -> sc.systemName
//         // graph 里： adjacency[dep] += sc.systemName
//         for (auto& dep : sc.initDeps) {
//             depGraph.adjacency[dep].push_back(sc.systemName);
//         }
//     }

//     std::vector<std::string> initOrder = topologicalSort(depGraph);

//     // 3. 按拓扑顺序依次创建 ISystem 实例 & onInit()
//     //    先把 "systemName->entity" 建一个映射
//     std::unordered_map<std::string, Entity> sysNameToEntity;
//     for (auto e : systemEntities) {
//         auto& sc                       = mECS->getComp<SystemComp>(e);
//         sysNameToEntity[sc.systemName] = e;
//     }

//     // 清空 mSystems
//     mSystems.clear();

//     for (auto& sysName : initOrder) {
//         auto e   = sysNameToEntity[sysName];
//         auto& sc = mECS->getComp<SystemComp>(e);

//         // 找到对应的 factory
//         auto it = mFactories.find(sc.systemName);
//         if (it == mFactories.end()) {
//             // 如果没注册 factory, 可能抛异常/警告
//             std::cerr << "No factory for system: " << sc.systemName << "\n";
//             continue;
//         }
//         auto sys = it->second(sc);  // 创建系统实例
//         sys->onInit();
//         // 存下来
//         mSystems.push_back(sys);
//     }

//     std::cout << "SystemManager::initAllSystems done! Created " << mSystems.size() << " systems.\n";
// }

// /**
//  * @brief 每帧更新系统
//  *   - 暂时简单写: 强行按init顺序调用 onUpdate()
//  *   - onUpdate会自己向 JobManager 提交并行任务
//  */
// void SystemManager::updateAllSystems(float dt) {
//     JobManagerProxy proxy(mJobManager);
//     for (auto& sys : mSystems) {
//         sys->onUpdate(dt, proxy);
//     }
//     // 如果需要在这一帧必须等待所有job完成，可以这里加:
//     // mJobManager.waitAll();
// }

// /**
//  * @brief 结束时: onDestroy
//  */
// void SystemManager::destroyAllSystems() {
//     for (auto& sys : mSystems) {
//         sys->onDestroy();
//     }
//     mSystems.clear();
// }