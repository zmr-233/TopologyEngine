/****************************************
 * File: Include/Systems/CameraSystem.hpp
 * 说明：
 ****************************************/
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>  // glm::toMat4, etc.

#include "../Components/CameraComponent.hpp"
#include "../Components/InputComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../ECS/System.hpp"

class CameraSystem : public System {
   public:
    void init() override {}

    void update(float dt) override {
        for (auto entity : mEntities) {
            auto& cam       = mECS->getComponent<CameraComponent>(entity);
            auto& transform = mECS->getComponent<TransformComponent>(entity);

            // 拿到输入实体 (假设它ID=someGlobalInputEntity)
            // 或者你也可以先把它放到CameraSystem的某个字段
            Entity inputEntity = getInputEntity();
            auto& inputComp    = mECS->getComponent<InputComponent>(inputEntity);

            // 1. 更新相机的 yaw/pitch
            float deltaX = static_cast<float>(inputComp.mouseDeltaX) * cam.mouseSensitivity;
            float deltaY = static_cast<float>(inputComp.mouseDeltaY) * cam.mouseSensitivity;
            // if (cam.InvertY) {
            //     deltaY = -deltaY;
            // }

            cam.yaw -= deltaX;
            cam.pitch -= deltaY;

            // 限制pitch避免翻转
            if (cam.pitch > 89.0f) cam.pitch = 89.0f;
            if (cam.pitch < -89.0f) cam.pitch = -89.0f;

            // 2. 把 (yaw, pitch) 转成四元数
            //    Yaw 是绕世界Y轴，Pitch是绕X轴
            //    这里演示的一种做法
            glm::quat qYaw        = glm::angleAxis(glm::radians(cam.yaw), glm::vec3(0, 1, 0));
            glm::quat qPitch      = glm::angleAxis(glm::radians(cam.pitch), glm::vec3(1, 0, 0));
            glm::quat orientation = qYaw * qPitch;

            // 3. 写回 transform.rotation
            transform.rotation = orientation;

            // 如果需要基于WASD前后移动，也可以：
            float forward = inputComp.axes["MoveForward"];
            float right   = inputComp.axes["MoveRight"];
            float up      = inputComp.axes["MoveUp"];
            // 先计算相机的“朝前”向量
            // 注意 transform.rotation 现在就是相机方向
            glm::vec3 front    = orientation * glm::vec3(0, 0, -1);
            glm::vec3 rightVec = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
            glm::vec3 moveDir  = glm::normalize(front) * forward + rightVec * right + glm::vec3(0, 1, 0) * up;
            if (glm::length(moveDir) > 0.001f) {
                moveDir = glm::normalize(moveDir);
            }
            transform.position += moveDir * (cam.movementSpeed * dt);

            // 如果需要滚轮来调整FOV
            cam.fov -= inputComp.scrollOffset;
            if (cam.fov < 1.0f) cam.fov = 1.0f;
            if (cam.fov > 90.0f) cam.fov = 90.0f;
        }
    }

    void initInputEntity(Entity inputEntity) {
        mInputEntity = inputEntity;
    }

   private:
    Entity getInputEntity() {
        return mInputEntity;
    }
    Entity mInputEntity;
};