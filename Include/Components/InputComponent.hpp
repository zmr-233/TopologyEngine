/****************************************
 * File: Include/Components/CameraComponent.hpp
 * 说明： 所有需要访问鼠标/按键的通用组件
 ****************************************/

#pragma once
#include <string>
#include <unordered_map>

// 键盘按键状态
struct KeyState {
    bool isDown        = false;  // 当前是否在按着
    bool downThisFrame = false;  // 本帧是否刚刚按下
    bool upThisFrame   = false;  // 本帧是否刚刚抬起
};

/**
 * @brief 这个组件可以视作“当前帧的输入汇总”
 * 你可以用一个全局“输入实体”（比如 ID = 0 或某个专门的 entity）
 * 只要本地玩家和所有需要读取输入的系统都知道去 ECS 里
 * 拿这个实体的 InputComponent 即可
 */
struct InputComponent {
    // 名义上的“轴” (Axis)，比如 "MoveForward", "MoveRight" 等
    std::unordered_map<std::string, float> axes = {
        {"MoveForward", 0.0f},
        {"MoveRight", 0.0f},
        {"MoveUp", 0.0f},
    };
    // 万一还没定义就访问了怎么办 => 目前暂时依赖于reset()方法

    // 记录具体按键 -> 状态
    std::unordered_map<int, KeyState> keyStates;

    // 鼠标位置和增量
    double mouseX      = 0.0;
    double mouseY      = 0.0;
    double mouseDeltaX = 0.0;
    double mouseDeltaY = 0.0;

    // 鼠标滚轮
    float scrollOffset = 0.0f;

    // 可以有一个重置的方法
    void reset() {
        // axes.clear(); => 不要清空，保持默认值
        // axes["MoveForward"] = 0.0f;
        // axes["MoveRight"]   = 0.0f;
        // axes["MoveUp"]      = 0.0f;
        // keyStates.clear();

        mouseDeltaX  = 0.0;
        mouseDeltaY  = 0.0;
        scrollOffset = 0.0f;
        // 保持 mouseX,mouseY 不重置也行，看需求
    }
};
