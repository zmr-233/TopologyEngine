/****************************************
 * FILE: Engine/Include/Comps/CameraComp.hpp
 * 说明：
 ****************************************/

#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../ECS/EntityTypes.hpp"

// 这是一个简单的Camera数据组件
// 位置、方向 使用 TransformComp 来表示
// 也可以加入更多的配置，如视野、近远裁剪面等

struct CameraComp {
    Entity windowID;  // 绑定的窗口

    float fov       = 45.0f;  // 视野
    float nearPlane = 0.1f;
    float farPlane  = 100.0f;

    float movementSpeed    = 4.0f;
    float mouseSensitivity = 0.5f;

    // 这两个只是“相机的欧拉角”或“某些内部控制状态”
    // 也可以只在 CameraSystem 内部用，不必放到组件
    float yaw   = -90.0f;
    float pitch = 0.0f;

    // 视图矩阵可以直接通过 TransformComp 计算
    // 也可能存储一个临时的viewMatrix缓存

    // 反转Y轴
    bool InvertY = false;
};

/*Old Version:
struct CameraComp {
    //如下交由InputComp处理
    glm::vec3 Position{0.0f, 0.0f, 3.0f};
    glm::vec3 Front{0.0f, 0.0f, -1.0f};
    glm::vec3 Up{0.0f, 1.0f, 0.0f};
    glm::vec3 Right{1.0f, 0.0f, 0.0f};
    glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};

    // 欧拉角
    float Yaw   = -90.0f;
    float Pitch = 0.0f;

    // 其他相机配置
    float MovementSpeed    = 2.5f;
    float MouseSensitivity = 0.1f;
    float Zoom             = 45.0f;

    // 得到View矩阵
    glm::mat4 getViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }
};
*/