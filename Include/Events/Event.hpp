/****************************************
 * File: Include/Events/Event.hpp
 * 说明：简单的数据容器（POD，Plain Old Data）
 * 用来存储输入事件中需要的信息：哪个窗口产生、当前键值/鼠标按钮/位置/滚轮等
 ****************************************/

#pragma once

#include <variant>

// 事件不会处理KeyAction这样的细节，只关心按下/抬起/移动等
// enum class KeyAction {
//     Press,
//     Release,
//     Repeat
//     // ...
// };

// 这里可以是一个空壳或者作为标记用，不一定必须
// struct IEvent {
//     virtual ~IEvent() = default;
// };

// 键盘事件：按下/抬起
struct KeyEvent {
    void* windowHandle;  // 哪个窗口触发？

    int key;       // GLFW_KEY_W, GLFW_KEY_S 等
    int scancode;  // 一般用不着，可省
    int action;    // GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
    int mods;      // Ctrl / Shift / Alt 等组合键
};

// 鼠标移动事件
struct MouseMoveEvent {
    void* windowHandle;  // 哪个窗口触发

    double x;
    double y;
};

// 鼠标按钮事件
struct MouseButtonEvent {
    void* windowHandle;  // 哪个窗口触发

    int button;  // GLFW_MOUSE_BUTTON_LEFT, etc
    int action;
    int mods;
};

// 鼠标滚轮事件
struct MouseScrollEvent {
    void* windowHandle;  // 哪个窗口触发

    double xoffset;
    double yoffset;
};

// 其他鼠标按钮事件也可以加
// struct MouseButtonEvent : public IEvent { ... };

// 输入事件
using InputEvent = std::variant<
    KeyEvent,
    MouseMoveEvent,
    MouseButtonEvent,
    MouseScrollEvent>;

// “引擎中通用的事件接口”目前就等价于“输入事件的 variant”
using IEvent = InputEvent;
