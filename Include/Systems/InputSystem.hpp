/****************************************
 * File: Include/Systems/InputSystem.hpp
 * 说明：
 ****************************************/
#pragma once
#include <GLFW/glfw3.h>

#include <cassert>
#include <queue>
#include <vector>

#include "../Components/InputComponent.hpp"
#include "../Components/WindowComponent.hpp"
#include "../ECS/System.hpp"
#include "../Events/EventBus.hpp"
#include "../Utils/Log.hpp"

// 键盘映射： 不应该由InputSystem管理，而是由InputMapping管理
// static KeyAction convertGLFWAction(int glfwAction) {
//     switch (glfwAction) {
//         case GLFW_PRESS:
//             return KeyAction::Press;
//         ...
//     }
// }

/**
 * @brief 订阅底层事件， 然后把它们写进一个临时缓冲（mNextInput）
 *
 * 双缓冲实现：
 * 在每帧 update() 的开始或结尾，把 mNextInput 拷贝/交换到 mCurrentInput 中，
 * 并清空 mNextInput，这样后续系统可以无锁地读取 mCurrentInput
 *
 * 最后，将 mCurrentInput 写回 ECS 中的 InputComponent（也可以只做一次拷贝
 * 其他系统再从 InputComponent 里读取即可。
 */
class InputSystem : public System {
   public:
    void initEventBus(std::shared_ptr<EventBus> evtBus) {
        mEventBus = evtBus;
        // DEBUG("InputSystem subscribed to EventBus.");
        // 订阅所有输入事件：KeyEvent, MouseMoveEvent, MouseButtonEvent, MouseScrollEvent });
        mEventBus->subscribe<KeyEvent>(
            [this](const KeyEvent& evt) { onKey(evt); });
        mEventBus->subscribe<MouseMoveEvent>(
            [this](const MouseMoveEvent& evt) { onMouseMove(evt); });
        mEventBus->subscribe<MouseScrollEvent>(
            [this](const MouseScrollEvent& evt) { onMouseScroll(evt); });
    }
    // 跟 WindowSystem 协作：在创建窗口后，需要设置回调函数
    void initWindow(Entity winID) {
        auto& winComp = mECS->getComponent<WindowComponent>(winID);
        auto window   = winComp.window;
        assert(window);
        mWindows.push_back(window);

        // 把 this + windowId 通过 userPointer 传给 GLFW 回调
        glfwSetWindowUserPointer(window, this);

        // 给每个 window 注册回调
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
            auto sys = static_cast<InputSystem*>(glfwGetWindowUserPointer(w));
            sys->pushKeyEvent(KeyEvent{w, key, scancode, action, mods});
        });
        glfwSetCursorPosCallback(window, [](GLFWwindow* w, double xpos, double ypos) {
            auto sys = static_cast<InputSystem*>(glfwGetWindowUserPointer(w));
            sys->pushMouseMoveEvent(MouseMoveEvent{w, xpos, ypos});
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            auto sys = static_cast<InputSystem*>(glfwGetWindowUserPointer(w));
            sys->pushMouseButtonEvent(MouseButtonEvent{w, button, action, mods});
        });
        glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset) {
            auto sys = static_cast<InputSystem*>(glfwGetWindowUserPointer(w));
            sys->pushMouseScrollEvent(MouseScrollEvent{w, xoffset, yoffset});
        });
    }
    void init() override {}

    // 每帧调用一次，内部做 glfwPollEvents()
    void update(float dt) override {
        // 收集所有窗口输入
        // glfwPollEvents(); // Q: 这里调用一次就够了吗？
        // A: 可以在 WindowSystem 里调用，或者在这里调用
        //    但是如果有多个窗口，可能需要在 WindowSystem 里调用

        // 1. 把上面通过 onKey/onMouseMove... 收集到的 mNextInput 拷贝到 mCurrentInput
        mCurrentInput = mNextInput;

        // 2. 清空 mNextInput，准备下一帧再写
        mNextInput.reset();

        // 3. 根据 keyStates 填充 axes (W->+1, S->-1...)，或者使用你原先的 InputMapping 也可以
        processKeyAxisMapping(mCurrentInput);

        // 4. 将结果写回 ECS 中的 InputComponent（假设只有 1 个全局输入实体）
        //    当然你也可以有多玩家 => 多个 entity，每个都一份 InputComponent
        // DEBUG("mEntities.size() = %d", mEntities.size());
        for (auto entity : mEntities) {
            // DEBUG("Writing InputComponent to entity: %d", entity);
            auto& inputComp = mECS->getComponent<InputComponent>(entity);
            inputComp       = mCurrentInput;
        }
    }

    // GLFW 回调中调用以下接口，把对应事件推入队列
    void pushKeyEvent(const KeyEvent& evt) {
        // DEBUG("Key event: key={%d}, action={%d}", evt.key, evt.action);
        mEventBus->publish(evt);
    }
    void pushMouseMoveEvent(const MouseMoveEvent& evt) {
        mEventBus->publish(evt);
    }
    void pushMouseButtonEvent(const MouseButtonEvent& evt) {
        mEventBus->publish(evt);
    }
    void pushMouseScrollEvent(const MouseScrollEvent& evt) {
        mEventBus->publish(evt);
    }

   private:
    void onKey(const KeyEvent& evt) {
        // DEBUG("onKey: key={%d}, action={%d}", evt.key, evt.action);
        // 示例：只处理GLFW_PRESS / GLFW_RELEASE
        // 假定 1=PRESS, 0=RELEASE
        auto& ks = mNextInput.keyStates[evt.key];
        if (evt.action == 1 /* PRESS */) {
            if (!ks.isDown) {
                ks.isDown        = true;
                ks.downThisFrame = true;
            }
        } else if (evt.action == 0 /* RELEASE */) {
            if (ks.isDown) {
                ks.isDown      = false;
                ks.upThisFrame = true;
            }
        }
    }

    void onMouseMove(const MouseMoveEvent& evt) {
        // 计算 delta
        double oldX       = mNextInput.mouseX;
        double oldY       = mNextInput.mouseY;
        mNextInput.mouseX = evt.x;
        mNextInput.mouseY = evt.y;

        mNextInput.mouseDeltaX = (evt.x - oldX);
        mNextInput.mouseDeltaY = (evt.y - oldY);
    }

    void onMouseScroll(const MouseScrollEvent& evt) {
        // 记录滚轮
        // 如果是FPS相机，也许只需要 yoffset
        mNextInput.scrollOffset += static_cast<float>(evt.yoffset);
    }

    // 如果你想在这里直接做“按键->轴值”的映射，也可以加一个映射表
    void processKeyAxisMapping(InputComponent& input) {
        // 举个例子：如果键盘W被按下 => MoveForward=+1.0, S => -1.0
        // 这里只是演示
        float forwardValue = 0.0f;
        if (input.keyStates[GLFW_KEY_W].isDown) {  // 87 假设 'W'
            forwardValue += 1.0f;
        }
        if (input.keyStates[GLFW_KEY_S].isDown) {  // 'S'
            forwardValue -= 1.0f;
        }
        input.axes["MoveForward"] = forwardValue;

        float rightValue = 0.0f;
        if (input.keyStates[GLFW_KEY_A].isDown) {  // 'A'
            rightValue -= 1.0f;
        }
        if (input.keyStates[GLFW_KEY_D].isDown) {  // 'D'
            rightValue += 1.0f;
        }
        input.axes["MoveRight"] = rightValue;

        float upValue = 0.0f;
        if (input.keyStates[GLFW_KEY_SPACE].isDown) {  // 'Space'
            upValue += 1.0f;
        }
        if (input.keyStates[GLFW_KEY_LEFT_SHIFT].isDown) {  // 'LShift'
            upValue -= 1.0f;
        }
        input.axes["MoveUp"] = upValue;

        if (input.keyStates[GLFW_KEY_ESCAPE].isDown) {  // 'ESC'
            exit(0);
        }
    }

   private:
    // 要管理的窗口
    std::vector<GLFWwindow*> mWindows;
    // TODO: 多个相同mWindows可以进行合并

    std::shared_ptr<EventBus> mEventBus;

    // 双缓冲：这一帧收集到的事件先写到 mNextInput
    // 在 update() 时再拷贝到 mCurrentInput
    InputComponent mNextInput;
    InputComponent mCurrentInput;

    // System::mEntities: 用于对输入感兴趣的系统
};
// 唯一的区别在之前的版本中 事件是仅仅通过订阅InputSystem实现的
// 而现在直接推到EventBus中，然后再自己订阅