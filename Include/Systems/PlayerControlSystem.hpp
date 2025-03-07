/****************************************
 * File: Include/Systems/PlayerControlSystem.hpp
 * 说明：
 ****************************************/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../Components/InputComponent.hpp"
#include "../Components/PlayerControlComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../ECS/System.hpp"

// 一个简单的PlayerControlSystem，基于 InputComponent 的数据
// 来更新 PlayerControlComponent + TransformComponent。
class PlayerControlSystem : public System {
   public:
    void init() override {
        // 如果需要做某些初始化操作，可在这里做
    }

    void update(float dt) override {
        // 假设我们有且只有1个玩家，或者也可能是多个玩家实体
        // 这里演示循环所有持有 PlayerControlComponent 的实体
        for (auto entity : mEntities) {
            auto& playerCtrl = mECS->getComponent<PlayerControlComponent>(entity);
            auto& transform  = mECS->getComponent<TransformComponent>(entity);

            // 1) 找到存放全局输入的实体 (你在自己的框架里决定它是谁)
            //    然后取出 InputComponent
            Entity inputEntity = getInputEntity();
            auto& inputComp    = mECS->getComponent<InputComponent>(inputEntity);

            // 2) 计算平面移动 (X/Z)，根据 axes["MoveForward"] 和 axes["MoveRight"]
            float forwardValue = inputComp.axes["MoveForward"];  // -1 ~ +1
            float rightValue   = inputComp.axes["MoveRight"];    // -1 ~ +1

            // 当前玩家Transform可能也带有旋转:
            // 如果想“前后”=朝向transform的forward向量，则需要计算：
            //   forwardVec = transform.rotation * (0,0,-1)
            //   rightVec   = cross(forwardVec, up)
            // 这里先做一个最简单的，假设玩家面朝 -Z，就是传统FPS坐标。
            glm::vec3 forwardDir(0, 0, -1);
            glm::vec3 rightDir(1, 0, 0);

            // 如果玩家自身有朝向，可以改成:
            // glm::vec3 forwardDir = transform.rotation * glm::vec3(0,0,-1);
            // glm::vec3 rightDir   = glm::cross(forwardDir, glm::vec3(0,1,0));

            glm::vec3 moveDir = forwardDir * forwardValue + rightDir * rightValue;
            if (glm::length(moveDir) > 0.0001f) {
                moveDir = glm::normalize(moveDir);
            }

            // 3) 处理跳跃 (假设空格键)
            //    这里演示简化逻辑：如果isGrounded && space.downThisFrame => 起跳
            const int KEY_SPACE     = 32;  // GLFW_KEY_SPACE
            auto it                 = inputComp.keyStates.find(KEY_SPACE);
            bool spaceDownThisFrame = false;
            if (it != inputComp.keyStates.end()) {
                spaceDownThisFrame = it->second.downThisFrame;
            }

            if (playerCtrl.isGrounded && spaceDownThisFrame) {
                // 起跳
                playerCtrl.yVelocity  = playerCtrl.jumpForce;
                playerCtrl.isGrounded = false;
            }

            // 4) 重力与垂直移动 (简化)
            const float gravity = -9.8f;  // 伪装一下地球重力
            playerCtrl.yVelocity += gravity * dt;

            // 5) 更新位置
            //    先做水平方向
            float speed = playerCtrl.movementSpeed;
            transform.position += moveDir * speed * dt;

            //    再做垂直方向
            transform.position.y += playerCtrl.yVelocity * dt;

            // 6) 简易地面判定：若碰到y=0，就视为落地
            //    这儿只做一个最简单的写死判定
            if (transform.position.y <= 0.0f) {
                transform.position.y  = 0.0f;
                playerCtrl.yVelocity  = 0.0f;
                playerCtrl.isGrounded = true;
            }
        }
    }
    void initInputEntity(Entity inputEntity) {
        // 你可能需要在这里初始化一些输入实体的数据
        // 比如设置一些默认的键位映射
        // 也可以在外部初始化，然后传入
        mInputEntity = inputEntity;
    }

   private:
    // 你需要根据自己ECS框架的实际情况来获取“输入实体”
    // 这可能保存在某个全局ID或系统变量里
    Entity getInputEntity() {
        return mInputEntity;  // demo: 假设ID=1的实体是全局输入实体
    }
    Entity mInputEntity;
};
