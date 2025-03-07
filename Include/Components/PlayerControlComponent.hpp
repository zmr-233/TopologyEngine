/****************************************
 * File: Include/Components/PlayerControlComponent.hpp
 * 说明：
 ****************************************/
#pragma once

struct PlayerControlComponent {
    float movementSpeed = 5.0f;
    float jumpForce     = 8.0f;
    bool isGrounded     = true;
    float yVelocity     = 0.0f;  // 当前垂直方向速度 (简化示例)
};
