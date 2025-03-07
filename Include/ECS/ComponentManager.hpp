/****************************************
 * File: Include/ECS/ComponentManager.hpp
 ****************************************/

#pragma once
#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "ComponentArray.hpp"
#include "EntityManager.hpp"
#include "IComponentArray.hpp"

class ComponentManager {
   public:
    template <typename T>
    void registerComponent() {
        const std::type_index typeName = typeid(T);

        // 确保尚未注册过该组件
        assert(mComponentArrays.find(typeName) == mComponentArrays.end() &&
               "Registering component type more than once.");

        // 创建并存储 ComponentArray
        mComponentArrays[typeName] = std::make_shared<ComponentArray<T>>();
    }

    template <typename T>
    void addComponent(Entity entity, const T& component) {
        // 根据类型拿到对应的 ComponentArray，然后插入数据
        getComponentArray<T>()->insertData(entity, component);
    }

    template <typename T>
    void removeComponent(Entity entity) {
        getComponentArray<T>()->removeData(entity);
    }

    template <typename T>
    T& getComponent(Entity entity) {
        return getComponentArray<T>()->getData(entity);
    }

    template <typename T>
    bool hasComponent(Entity entity) {
        return getComponentArray<T>()->hasData(entity);
    }

    // 当实体被销毁时，通知所有组件数组
    void onEntityDestroyed(Entity entity) {
        for (auto const& pair : mComponentArrays) {
            auto const& componentArray = pair.second;
            componentArray->onEntityDestroyed(entity);
        }
    }

    template <typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray() {
        const std::type_index typeName = typeid(T);
        assert(mComponentArrays.find(typeName) != mComponentArrays.end() &&
               "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
    }

   private:
    // 组件类型 -> 对应的IComponentArray
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> mComponentArrays{};
};