/****************************************
 * File: Include/ECS/EntityManager.hpp
 ****************************************/
#pragma once

#include <array>
#include <queue>

#include "EntityTypes.hpp"

class EntityManager {
   public:
    EntityManager() {
        // 在初始化时，将所有可用的实体ID（0 ~ 最大数量-1）放进空闲队列
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
            mAvailableEntities.push(entity);
        }
    }

    // 创建一个新的实体ID
    Entity createEntity() {
        // 如果没有可用ID了，你可以考虑扩容或抛异常，这里简化处理
        if (mLivingEntityCount >= MAX_ENTITIES) {
            // 这里简单返回 0（假设0会被视为无效），生产中可以断言或抛异常
            return 0;
        }

        Entity id = mAvailableEntities.front();
        mAvailableEntities.pop();
        ++mLivingEntityCount;
        return id;
    }

    // 销毁实体
    void destroyEntity(Entity entity) {
        // 让该ID可以被重用
        mSignatures[entity].reset();  // 清空它的组件签名
        mAvailableEntities.push(entity);
        --mLivingEntityCount;
    }

    // 为某实体设置新的组件签名
    void setSignature(Entity entity, Signature signature) {
        mSignatures[entity] = signature;
    }

    // 获取某实体的组件签名
    Signature getSignature(Entity entity) const {
        return mSignatures[entity];
    }

   private:
    static constexpr std::size_t MAX_ENTITIES = 5000;  // 最多同时存在5000个实体
    std::queue<Entity> mAvailableEntities{};
    std::array<Signature, MAX_ENTITIES> mSignatures{};
    std::size_t mLivingEntityCount{0};
};