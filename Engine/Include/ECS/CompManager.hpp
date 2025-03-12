/****************************************
 * FILE: Engine/Include/ECS/CompManager.hpp
 ****************************************/
#pragma once
#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "CompArray.hpp"
#include "EntityManager.hpp"

class CompManager {
public:
    template <typename T, std::size_t N>
    void registerComp() {
        const std::type_index typeName = typeid(T);
        assert(mCompArrays.find(typeName) == mCompArrays.end() &&
               "Registering component type more than once.");

        // 创建并存储
        auto newArray = std::make_shared<CompArray<T, N>>();
        mCompArrays[typeName] = newArray;
    }

    template <typename T>
    void addComp(Entity entity, const T& component) {
        getCompArray<T>()->insertData(entity, component);
    }

    template <typename T>
    void removeComp(Entity entity) {
        getCompArray<T>()->removeData(entity);
    }

    // 常规 ECS里是 getData(entity)，现在可以改成返回“读/写句柄”
    // 这里给出一个示例：获取读句柄
    template <typename T>
    auto readComp(Entity entity) {
        return getCompArray<T>()->read(entity);
    }

    // 获取写句柄
    template <typename T>
    auto writeComp(Entity entity) {
        return getCompArray<T>()->write(entity);
    }

    // 是否有组件
    template <typename T>
    bool hasComp(Entity entity) {
        return getCompArray<T>()->hasData(entity);
    }

    // 当实体销毁时
    void onEntityDestroyed(Entity entity) {
        for (auto const& [typeIdx, compArray] : mCompArrays) {
            compArray->onEntityDestroyed(entity);
        }
    }

private:
    template <typename T, std::size_t N = 2>
    std::shared_ptr<CompArray<T, N>> getCompArray() {
        const std::type_index typeName = typeid(T);
        assert(mCompArrays.find(typeName) != mCompArrays.end() &&
               "Comp not registered before use.");
        return std::static_pointer_cast<CompArray<T, N>>(mCompArrays[typeName]);
    }

private:
    // 组件类型 -> CompArrayI
    std::unordered_map<std::type_index, std::shared_ptr<CompArrayI>> mCompArrays{};
};
