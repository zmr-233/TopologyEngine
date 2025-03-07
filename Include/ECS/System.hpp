/****************************************
 * File: Include/ECS/System.hpp
 * 说明：System 的基类
 ****************************************/

#pragma once
#include <set>

#include "../Events/EventBus.hpp"
#include "ECSCore.hpp"
#include "EntityTypes.hpp"

class ECSCore;
class System {
   public:
    std::set<Entity> mEntities;     // 这个系统关心的实体列表
    std::shared_ptr<ECSCore> mECS;  // 指向ECSCore的指针
    // std::shared_ptr<EventBus> mEventBus;
    //^^^^倾向于分拆出去 因为可能会有各种不同的事件总线
    // TODO: 增加用于检查是否初始化完成的标志
    // int mInitFlag = 0;

    virtual ~System()             = default;
    virtual void init()           = 0;
    virtual void update(float dt) = 0;
};
