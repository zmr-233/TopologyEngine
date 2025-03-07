/****************************************
 * File: Include/Components/LightComponent.hpp
 * 说明：
 ****************************************/
#pragma once

#include <glm/glm.hpp>

#include "../ECS/EntityTypes.hpp"

// 参考你写的各种光源结构，这里简化成一个可扩展的LightComponent
enum class LightType {
    Directional,
    Point,
    Spot,
    Flashlight
};

struct LightComponent {
    LightType type = LightType::Point;

    // 基础光照参数
    glm::vec3 ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 diffuse{0.5f, 0.5f, 0.5f};
    glm::vec3 specular{1.0f, 1.0f, 1.0f};

    // 点/聚光需要的位置、衰减参数
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    // TODO: 可以设置position是否跟随其绑定的TransformComponent
    float constant  = 1.0f;
    float linear    = 0.09f;
    float quadratic = 0.032f;

    // 方向光/聚光需要的方向
    glm::vec3 direction{0.0f, -1.0f, 0.0f};

    // 聚光角度
    float cutOff      = 12.5f;  // 内角度
    float outerCutOff = 17.5f;  // 外角度

    // 手电筒模式需要绑定的相机实体
    Entity bindCamera;
};
