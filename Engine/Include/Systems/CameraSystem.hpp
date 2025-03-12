/****************************************
 * FILE: Engine/Include/Systems/CameraSystem.hpp
 ****************************************/
#pragma once
#include "../Comps/CameraComp.hpp"
#include "../Comps/InputComp.hpp"
#include "../Comps/Sys/CameraSysComp.hpp"
#include "../Comps/TransformComp.hpp"
#include "../ECS/JobManagerProxy.hpp"
#include "../ECS/System.hpp"

/**
 * @brief 分块并行的示例：
 *   - 假设系统关心的实体都带有 TransformComp / CameraComp
 *   - 我们先收集相关实体，分成 chunks，再提交Job
 */
class CameraSystem final : public SystemBase {
   public:
    CameraSystem(std::shared_ptr<ECSCore> ecs, const CameraSysComp& comp)
        : SystemBase(ecs, comp) {}

    void onInit() override {
        std::cout << "[CameraSystem] onInit\n";
        // 做一些初始化操作，比如找到“输入实体ID”，记录到mInputEntity等
    }

    void onUpdate(float dt, JobManagerProxy& jmProxy) override {
        // 1) 找到所有带 CameraComp, TransformComp 的实体(示例)
        std::vector<Entity> cameras;
        for (Entity e = 1; e < 5000; ++e) {
            if (mECS->hasComp<CameraComp>(e) && mECS->hasComp<TransformComp>(e)) {
                cameras.push_back(e);
            }
        }

        // 2) 分块(比如每块处理 128 个Entity)
        size_t chunkSize = 128;
        size_t total     = cameras.size();
        for (size_t start = 0; start < total; start += chunkSize) {
            size_t end = std::min(start + chunkSize, total);
            auto job   = jmProxy.submitJob("CameraSystem_UpdateChunk",
                                           [this, dt, start, end, &cameras]() {
                                             // 处理这一块中的实体
                                             for (size_t i = start; i < end; ++i) {
                                                 Entity e        = cameras[i];
                                                 auto& cam       = mECS->getComp<CameraComp>(e);
                                                 auto& transform = mECS->getComp<TransformComp>(e);

                                                 // 读Input(可能只有一个实体)
                                                 // entity写死也行, 或从SystemComp拿到
                                                 // 这里只是演示
                                                 Entity inputE = 10;  // 假设
                                                 auto& input   = mECS->getComp<InputComp>(inputE);

                                                 // ... 做相机 yaw/pitch/pos 更新 ...
                                                 float deltaX = static_cast<float>(input.mouseDeltaX) * cam.mouseSensitivity;
                                                 float deltaY = static_cast<float>(input.mouseDeltaY) * cam.mouseSensitivity;
                                                 cam.yaw -= deltaX;
                                                 cam.pitch -= deltaY;
                                                 // ...
                                             }
                                         });
            // 如果需要强依赖(等待 InputSystem 写完?),
            // 可以 jmProxy.addDependency(inputJob, job) 之类
        }
    }

    void onDestroy() override {
        std::cout << "[CameraSystem] onDestroy\n";
    }
};
