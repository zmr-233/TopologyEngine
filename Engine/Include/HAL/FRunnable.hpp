/****************************************
 * FILE: Engine/Include/HAL/FRunnable.hpp
 ****************************************/
#pragma once

#include "CoreTypes.hpp"

/**
 * @brief 线程执行体
 *
 * 处理并行任务的基础类，需要继承并实现 Run 方法
 */
class FRunnable {
   public:
    virtual ~FRunnable() {}

    // false 表示初始化失败，线程就不进入 Run() 了
    virtual bool Init() {
        return true;
    }

    // 线程的主循环逻辑
    virtual uint32 Run() = 0;

    // 请求停止线程时（Kill 或外部 Stop）会被调用
    virtual void Stop() {}

    // 线程退出前，会调用 Exit() 做一些收尾
    virtual void Exit() {}
};