/****************************************
 * FILE: Engine/Include/ECS/CompArray.hpp
 ****************************************/
#pragma once
#include <array>
#include <cassert>
#include <memory>
#include <unordered_map>

#include "CompCache.hpp"
#include "CompType.hpp"
#include "EntityTypes.hpp"

class CompArrayI {
   public:
    virtual ~CompArrayI() = default;
    // 当实体被销毁时，需要通知所有组件数组移除对应的组件数据
    virtual void onEntityDestroyed(Entity entity) = 0;
};

template <typename T, std::size_t N>
class CompArray : public CompArrayI {
   public:
    // 向实体添加一个组件(默认构造或从传入数据拷贝)
    void insertData(Entity entity, const T& component) {
        assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() &&
               "Comp added to the same entity more than once.");

        size_t newIndex             = mSize;
        mEntityToIndexMap[entity]   = newIndex;
        mIndexToEntityMap[newIndex] = entity;
        // 写入数据：使用 Cache 的 write()
        mCompCacheArray[newIndex].write(component);
        ++mSize;
    }

    // 移除
    void removeData(Entity entity) {
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() &&
               "Removing non-existent component.");

        size_t indexOfRemoved = mEntityToIndexMap[entity];
        size_t indexOfLast    = mSize - 1;

        // 用末尾的组件Cache覆盖要删除的位置
        mCompCacheArray[indexOfRemoved] = std::move(mCompCacheArray[indexOfLast]);

        // 更新映射
        Entity lastEntity                 = mIndexToEntityMap[indexOfLast];
        mEntityToIndexMap[lastEntity]     = indexOfRemoved;
        mIndexToEntityMap[indexOfRemoved] = lastEntity;

        mEntityToIndexMap.erase(entity);
        mIndexToEntityMap.erase(indexOfLast);
        --mSize;
    }

    // 获取读句柄
    typename CompCache<T, N>::ReadHandle read(Entity entity) {
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "No component found.");
        auto index = mEntityToIndexMap[entity];
        return mCompCacheArray[index].read();
    }

    // 获取写句柄
    typename CompCache<T, N>::WriteHandle write(Entity entity) {
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "No component found.");
        auto index = mEntityToIndexMap[entity];
        return mCompCacheArray[index].writeHandle();
    }

    // 是否有该组件
    bool hasData(Entity entity) const {
        return (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end());
    }

    // 当实体被销毁时，移除其组件数据
    void onEntityDestroyed(Entity entity) override {
        if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
            removeData(entity);
        }
    }

   private:
    // 多缓冲组件数组
    std::array<CompCache<T, N>, MAX_COMP_ARRAY> mCompCacheArray{};
    // entity -> arrayIndex
    std::unordered_map<Entity, std::size_t> mEntityToIndexMap{};
    // arrayIndex -> entity
    std::unordered_map<std::size_t, Entity> mIndexToEntityMap{};
    // 已使用的有效数据大小
    std::size_t mSize{0};
};
