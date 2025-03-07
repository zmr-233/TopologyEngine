/****************************************
 * File: Include/ECS/ComponentArray.hpp
 * 说明：真正存储某种组件类型的容器
 ****************************************/

#pragma once
#include <cassert>
#include <unordered_map>

#include "IComponentArray.hpp"

template <typename T>
class ComponentArray : public IComponentArray {
   public:
    void insertData(Entity entity, const T& component) {
        assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() &&
               "Component added to same entity more than once.");

        // 在末尾插入新的组件数据
        size_t newIndex             = mSize;
        mEntityToIndexMap[entity]   = newIndex;
        mIndexToEntityMap[newIndex] = entity;
        mComponentArray[newIndex]   = component;
        ++mSize;
    }

    void removeData(Entity entity) {
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() &&
               "Removing non-existent component.");

        // 用最后一个元素的数据覆盖被移除元素的位置
        size_t indexOfRemoved           = mEntityToIndexMap[entity];
        size_t indexOfLast              = mSize - 1;
        mComponentArray[indexOfRemoved] = mComponentArray[indexOfLast];

        // 更新映射
        Entity lastEntity                 = mIndexToEntityMap[indexOfLast];
        mEntityToIndexMap[lastEntity]     = indexOfRemoved;
        mIndexToEntityMap[indexOfRemoved] = lastEntity;

        mEntityToIndexMap.erase(entity);
        mIndexToEntityMap.erase(indexOfLast);

        --mSize;
    }

    T& getData(Entity entity) {
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() &&
               "Retrieving non-existent component.");

        return mComponentArray[mEntityToIndexMap[entity]];
    }

    bool hasData(Entity entity){
        return (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end());
    }

    void onEntityDestroyed(Entity entity) override {
        if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
            removeData(entity);
        }
    }

   private:
    // 固定长度数组（示例），大小与最大实体数一致
    std::array<T, 5000> mComponentArray{};
    // entity -> arrayIndex
    std::unordered_map<Entity, size_t> mEntityToIndexMap{};
    // arrayIndex -> entity
    std::unordered_map<size_t, Entity> mIndexToEntityMap{};
    // 已使用的有效数据大小
    size_t mSize{0};
};