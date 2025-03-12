/****************************************
 * FILE: Engine/Include/ECS/ECSCore.hpp
 ****************************************/
#pragma once

#include <memory>
#include <thread>

#include "CompManager.hpp"
#include "EntityManager.hpp"
#include "EntityTypes.hpp"
#include "JobManager.hpp"
#include "SystemManager.hpp"

class ECSCore : public std::enable_shared_from_this<ECSCore> {
   public:
    ECSCore();

    // =========== 实体相关 =============
    Entity createEntity();
    void destroyEntity(Entity entity);

    // =========== 组件相关 =============
    template <typename T, size_t N>
    void registerComp();

    template <typename T>
    void addComp(Entity entity, const T& component);

    template <typename T>
    void removeComp(Entity entity);

    template <typename T>
    auto readComp(Entity entity);

    template <typename T>
    auto writeComp(Entity entity);

    template <typename T>
    T& getComp(Entity entity);

    template <typename T>
    bool hasComp(Entity entity);

    // =========== 系统相关 =============
    template <typename T>
    std::shared_ptr<T> registerSystem();

    template <typename T>
    void setSystemSignature(Signature signature);

    template <typename T>
    std::shared_ptr<T> getSystem();

    // =========== 帮助函数 =============
    template <typename T>
    size_t getCompTypeIndex() const;

   private:
    mutable size_t mCompTypeCounter{0};
    std::unique_ptr<JobManager> mJobManager;
    std::unique_ptr<EntityManager> mEntityManager;
    std::unique_ptr<CompManager> mCompManager;
    std::unique_ptr<SystemManager> mSystemManager;
};

// ---- 在头文件末尾包含 inl 文件 ----
#include "ECSCore.inl"
