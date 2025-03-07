/****************************************
 * File: Include/ECS/ECSCore.hpp
 * 说明：统一对外的 ECS 接口封装
 ****************************************/

#pragma once
#include <memory>

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"

class ECSCore : public std::enable_shared_from_this<ECSCore> {
   public:
    ECSCore() {
        mEntityManager    = std::make_unique<EntityManager>();
        mComponentManager = std::make_unique<ComponentManager>();
        mSystemManager    = std::make_unique<SystemManager>();
    }

    // =========== 实体相关 =============
    Entity createEntity() {
        return mEntityManager->createEntity();
    }
    void destroyEntity(Entity entity) {
        mEntityManager->destroyEntity(entity);
        mComponentManager->onEntityDestroyed(entity);
        mSystemManager->onEntityDestroyed(entity);
    }

    // =========== 组件相关 =============
    template <typename T>
    void registerComponent() {
        mComponentManager->registerComponent<T>();
    }

    template <typename T>
    void addComponent(Entity entity, const T& component) {
        mComponentManager->addComponent<T>(entity, component);

        // 更新实体的Signature
        Signature signature = mEntityManager->getSignature(entity);
        signature.set(getComponentTypeIndex<T>(), true);
        mEntityManager->setSignature(entity, signature);

        // 通知 SystemManager
        mSystemManager->onEntitySignatureChanged(entity, signature);
    }

    template <typename T>
    void removeComponent(Entity entity) {
        mComponentManager->removeComponent<T>(entity);

        // 更新Signature
        Signature signature = mEntityManager->getSignature(entity);
        signature.set(getComponentTypeIndex<T>(), false);
        mEntityManager->setSignature(entity, signature);

        // 通知 SystemManager
        mSystemManager->onEntitySignatureChanged(entity, signature);
    }

    template <typename T>
    T& getComponent(Entity entity) {
        return mComponentManager->getComponent<T>(entity);
    }

    template <typename T>
    bool hasComponent(Entity entity){
        return mComponentManager->getComponentArray<T>()->hasData(entity);
    }

    // =========== 系统相关 =============
    template <typename T>
    std::shared_ptr<T> registerSystem() {
        return mSystemManager->registerSystem<T>(shared_from_this());
    }

    template <typename T>
    void setSystemSignature(Signature signature) {
        mSystemManager->setSignature<T>(signature);
    }

    template <typename T>
    std::shared_ptr<T> getSystem() {
        return mSystemManager->getSystem<T>();
    }

    // =========== 帮助函数 =============
    // 这里以简化的方式，将typeid(T).hash_code()的值作为组件类型索引
    // 也可以维护一个静态的计数器，让每个组件类型映射一个固定ID
    template <typename T>
    size_t getComponentTypeIndex() const {
        static const size_t typeIndex = mComponentTypeCounter++;
        return typeIndex;
    }

   private:
    mutable size_t mComponentTypeCounter{0};

    // 内部管理器
    std::unique_ptr<EntityManager> mEntityManager;
    std::unique_ptr<ComponentManager> mComponentManager;
    std::unique_ptr<SystemManager> mSystemManager;
};