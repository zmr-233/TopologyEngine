/****************************************
 * File: Include/Components/TransformComponent.hpp
 ****************************************/

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    // 可以加一个获取4x4矩阵的函数（懒得写也行）
    // ...
};
