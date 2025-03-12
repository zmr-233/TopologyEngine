/****************************************
 * FILE: Engine/Include/Comps/Sys/CameraSysComp.hpp
 ****************************************/
#pragma once
#include "../../ECS/System.hpp"

// ------ 2) CameraSystem：带 chunk 并行示例 ------

struct CameraSysComp : public SystemComp {
    float mouseSensitivity = 0.1f;
    float movementSpeed    = 5.0f;
    // ...
};
