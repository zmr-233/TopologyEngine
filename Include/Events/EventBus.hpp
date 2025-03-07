
/****************************************
 * File: Include/Events/EventBus.hpp
 ****************************************/
#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Event.hpp"

/**
 * @brief 一个最简单的事件总线实现
 */
class EventBus : public std::enable_shared_from_this<EventBus> {
   public:
    /**
     * @brief 订阅事件类型 T 的回调
     */
    template <typename T>
    void subscribe(std::function<void(const T&)> callback) {
        // 根据 T 的 type_index 查找
        auto typeIdx = std::type_index(typeid(T));
        std::lock_guard<std::mutex> lock(mMutex);

        // 若还没有该类型的回调列表，先初始化一个空vector
        if (mSubscribers.find(typeIdx) == mSubscribers.end()) {
            // 注意，这里要存储的是能够接受 IEvent 的 function
            mSubscribers[typeIdx] = {};
        }

        // 包装函数：接受 const IEvent&，再用 std::get<T> 提取真正类型
        auto wrapper = [func = std::move(callback)](const IEvent& e) {
            // 如果 variant 确实是 T 类型，此处不会抛异常；若不是，会抛出 std::bad_variant_access
            // 一般引擎内，你会保证 publish<T> 对应 subscribe<T>，
            // 或者用 std::get_if<T>() 做一次 if 检测也行
            func(std::get<T>(e));
        };

        mSubscribers[typeIdx].push_back(std::move(wrapper));
    }

    /**
     * @brief 发布事件 T
     */
    template <typename T>
    void publish(const T& event) {
        auto typeIdx = std::type_index(typeid(T));
        std::lock_guard<std::mutex> lock(mMutex);

        // 如果没有订阅者，就直接返回
        if (mSubscribers.find(typeIdx) == mSubscribers.end()) {
            return;
        }

        // 调用所有订阅者
        for (auto& subscriber : mSubscribers[typeIdx]) {
            // subscriber 是 std::function<void(const IEvent&)>
            // 这里要把 event 转成 IEvent 再调用
            subscriber(static_cast<const IEvent&>(event));
        }
    }

   private:
    // key: 事件类型 type_index
    // value: 订阅该事件的回调列表（均是 `void(const IEvent&)`）
    std::unordered_map<std::type_index,
                       std::vector<std::function<void(const IEvent&)>>>
        mSubscribers;

    std::mutex mMutex;
};
