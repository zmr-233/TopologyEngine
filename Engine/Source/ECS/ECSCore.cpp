/****************************************
 * FILE: Engine/Source/ECS/ECSCore.cpp
 ****************************************/

#include "../../Include/ECS/ECSCore.hpp"

// ECSCore::ECSCore() {
//     auto threadCount = std::thread::hardware_concurrency();
//     mJobManager      = std::make_unique<JobManager>(threadCount);
//     mEntityManager   = std::make_unique<EntityManager>();
//     mCompManager     = std::make_unique<CompManager>();
//     mSystemManager   = std::make_unique<SystemManager>(*mJobManager, shared_from_this());
// }

// // =========== 实体相关 =============
// Entity ECSCore::createEntity() {
//     return mEntityManager->createEntity();
// }

// void ECSCore::destroyEntity(Entity entity) {
//     mEntityManager->destroyEntity(entity);
//     mCompManager->onEntityDestroyed(entity);
//     // mSystemManager->onEntityDestroyed(entity);
// }