/****************************************
 * File: Include/ECS/SystemManager.hpp
 ****************************************/

#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "../Utils/Log.hpp"
#include "ECSCore.hpp"
#include "System.hpp"

class SystemManager {
   public:
    template <typename T>
    std::shared_ptr<T> registerSystem(std::shared_ptr<ECSCore> mECS) {
        const std::type_index typeName = typeid(T);
        // 确保还未注册过
        assert(mSystems.find(typeName) == mSystems.end() &&
               "Registering system more than once.");

        auto system        = std::make_shared<T>();
        system->mECS       = mECS;
        mSystems[typeName] = system;
        return system;
    }

    // 当一个实体被销毁时，让系统移除对它的跟踪
    void onEntityDestroyed(Entity entity) {
        for (auto const& pair : mSystems) {
            auto const& system = pair.second;
            system->mEntities.erase(entity);
        }
    }

    // 当一个实体的组件签名改变时，决定它是否应该被加入/移出系统
    // 这里假设系统自己保存一个 "requiredSignature" 来匹配实体
    void onEntitySignatureChanged(Entity entity, Signature entitySignature) {
        for (auto const& pair : mSystems) {
            auto const& system  = pair.second;
            auto const& typeIdx = pair.first;

            auto const& sysReqSig = mSystemSignatures[typeIdx];
            bool matches          = (entitySignature & sysReqSig) == sysReqSig;
            if (matches) {
                system->mEntities.insert(entity);
                // DEBUG("SystemManager: Entity {%d} added to System", entity);
            } else {
                system->mEntities.erase(entity);
            }
        }
    }

    // 设置某个System所需的Signature
    template <typename T>
    void setSignature(Signature signature) {
        const std::type_index typeName = typeid(T);
        mSystemSignatures[typeName]    = signature;
    }

    // 获取已注册的系统
    template <typename T>
    std::shared_ptr<T> getSystem() {
        const std::type_index typeName = typeid(T);
        assert(mSystems.find(typeName) != mSystems.end() &&
               "System not registered.");
        return std::static_pointer_cast<T>(mSystems[typeName]);
    }

   private:
    // System类型 -> System实例
    std::unordered_map<std::type_index, std::shared_ptr<System>> mSystems{};

    // System类型 -> 该System所需组件的Signature
    std::unordered_map<std::type_index, Signature> mSystemSignatures{};
};
