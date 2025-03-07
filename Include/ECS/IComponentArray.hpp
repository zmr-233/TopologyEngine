/****************************************
 * File: Include/ECS/IComponentArray.hpp
 * 说明：抽象基类，用于在 ComponentManager
 *      中统一管理不同类型的组件数组
 ****************************************/
#pragma once
#include "EntityTypes.hpp"

class IComponentArray {
   public:
    virtual ~IComponentArray() = default;
    // 当实体被销毁时，需要通知所有组件数组移除对应的组件数据
    virtual void onEntityDestroyed(Entity entity) = 0;
};