/****************************************
 * File: Include/Systems/WindowSystem.hpp
 * 说明：只负责管理窗口的创建、隐藏鼠标等，与渲染逻辑无关。
 ****************************************/

#pragma once

// clang-format off
#include <glad/glad.h>  //要在glfw前引入
#include <GLFW/glfw3.h>
// clang-format on

#include <iostream>
#include <memory>

#include "../Components/WindowComponent.hpp"
#include "../ECS/System.hpp"
#include "../Utils/Check.hpp"
#include "../Utils/Log.hpp"

/**
 * @brief
 *
 * 易错点：最好设置一个主窗口，其余是附属窗口
 * 只调用 glfwInit() 并不代表已经有可用的 OpenGL 上下文
 * 必须 glfwCreateWindow + glfwMakeContextCurrent 后，OpenGL API 才能正常工作
 *
 * 多窗口想共享资源，要在 glfwCreateWindow 第五个参数写入主窗口的 GLFWwindow* 指针
 *
 * 最好：不要直接取消上下文
 */
class WindowSystem : public System {
   public:
    WindowSystem() : mGLFWInitialized(false) {}

    // 仅在此处调用 glfwInit()，确保只初始化一次
    void init() override {
        if (!mGLFWInitialized) {
            if (!glfwInit()) {
                ERROR("Failed to initialize GLFW!");
                return;
            }
            mGLFWInitialized = true;
            INFO("GLFW initialized successfully.");
        }
    }
    void initEventBus(std::shared_ptr<EventBus> evtBus) {
        mEventBus = evtBus;
    }
    // 为所有拥有 WindowComponent 的实体创建窗口。
    void initWindows() {
        if (!mGLFWInitialized) {
            ERROR("GLFW not initialized. Cannot create windows.");
            return;
        }

        // 假设第一个实体（或你指定的那一个）就是主窗口
        // 当然你也可以先遍历，找到一个“主窗口”标签的实体
        bool firstWindow       = true;
        GLFWwindow* mainWindow = nullptr;

        for (auto entity : mEntities) {
            auto& windowComp = mECS->getComponent<WindowComponent>(entity);

            // 设置窗口属性：OpenGL 版本、核心模式等
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            // 如果这是主窗口，就用 nullptr 创建一个独立上下文
            // 如果不是，就在第五个参数里传 mainWindow 来“共享”上下文。
            GLFWwindow* shareWindow = (firstWindow ? nullptr : mainWindow);

            // 创建窗口
            GLFWwindow* window = glfwCreateWindow(
                windowComp.width,
                windowComp.height,
                windowComp.title.c_str(),
                nullptr,
                shareWindow  // ——> 关键：如果要共享资源，传入已经存在的主窗口
            );

            if (!window) {
                ERROR("Failed to create GLFW window!");
                glfwTerminate();
                mGLFWInitialized = false;
                return;  // 直接返回，防止后续重复创建或使用
            }

            // 存储窗口指针
            windowComp.window = window;

            // 如果用户想在创建时就隐藏光标
            if (windowComp.hideCursor) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            // 如果这是第一个创建的(主)窗口
            if (firstWindow) {
                // 让它成为当前上下文
                glfwMakeContextCurrent(window);
                // 记下指针
                mainWindow  = window;
                firstWindow = false;
                INFO("Main window created.");
                // CHECK_BOOL(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));  // 将GLFW的函数指针加载到GLAD中
                if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                    ERROR("Failed to initialize GLAD");
                }
            } else {
                // 对于附属窗口，如果此时并不打算在其上立刻做 OpenGL 操作，可以先不 make current
                // glfwMakeContextCurrent(nullptr);
            }
        }
    }

    // 每帧更新窗口事件状态；如果只做窗口管理，可以不做其他操作。
    void update(float /*dt*/) override {
        if (!mGLFWInitialized) {
            return;
        }

        // 一般情况下，所有窗口共用一个事件循环，只需要在这里调用一次
        glfwPollEvents();

        // 例如，如果需要在这里对每个窗口做一些窗口层面的操作，可以加在下面
        for (auto entity : mEntities) {
            auto& windowComp   = mECS->getComponent<WindowComponent>(entity);
            GLFWwindow* window = windowComp.window;
            if (!window) {
                continue;  // 未成功创建的窗口跳过
            }

            // 若需要在运行时切换光标状态，可以在此处设置
            if (windowComp.hideCursor) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            // 其他仅和窗口相关的逻辑（非渲染）可以放在这里
        }
    }

    // 释放所有已创建的窗口，并终止 GLFW。
    void shutdown() {
        if (!mGLFWInitialized) {
            return;
        }

        // 销毁所有窗口
        for (auto entity : mEntities) {
            auto& windowComp = mECS->getComponent<WindowComponent>(entity);
            if (windowComp.window) {
                glfwDestroyWindow(windowComp.window);
                windowComp.window = nullptr;
            }
        }

        glfwTerminate();
        mGLFWInitialized = false;
        INFO("GLFW terminated and all windows destroyed.");
    }

   private:
    bool mGLFWInitialized;  // 标记是否已经调用 glfwInit()

    std::shared_ptr<EventBus> mEventBus;
};
