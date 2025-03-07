/****************************************
 * File: Include/Systems/RenderSystem.hpp
 ****************************************/

#pragma once
// clang-format off
 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "../Components/CameraComponent.hpp"
#include "../Components/LightComponent.hpp"
#include "../Components/MeshRenderComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/WindowComponent.hpp"
#include "../ECS/System.hpp"

// 这里假设我们也需要 Shader 资源（可能通过 shaderID 索引到某个全局管理里）
// 或者你有一个专门的 ShaderManager
#include "../Resources/Shader.hpp"
#include "../Include/Utils/Log.hpp"

class RenderSystem : public System {
   public:
    void init() override {
        // 若需要做一次性的初始化，可在此进行
    }

    // 指定场景里有哪些摄像机、有哪些灯光。你也可以用别的方法管理
    void initLightCameraEntities(const std::vector<Entity>& cameras, const std::vector<Entity>& lights) {
        mCameras = cameras;
        mLights  = lights;
    }

    void update(float dt) override {
        // 如果你需要一次只渲染主相机，也可只取 mCameras[0]
        for (auto camEnt : mCameras) {
            auto& cam          = mECS->getComponent<CameraComponent>(camEnt);
            auto& windowComp   = mECS->getComponent<WindowComponent>(cam.windowID);
            GLFWwindow* window = windowComp.window;

            // 1. 切换 OpenGL 上下文
            glfwMakeContextCurrent(window);
            // 开启深度测试
            glEnable(GL_DEPTH_TEST);

            // 2. 清屏
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            // DEBUG("Clearing screen...");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 3. 计算相机的 view / projection 矩阵
            //    （假设相机位置、朝向由其 TransformComponent 或 yaw/pitch 来决定）
            glm::mat4 view       = calculateViewMatrix(camEnt);
            glm::mat4 projection = glm::perspective(glm::radians(cam.fov),
                                                    (float)windowComp.width / (float)windowComp.height,
                                                    cam.nearPlane, cam.farPlane);
            // 观察者位置（EyePos）
            glm::vec3 cameraPos = getCameraWorldPosition(camEnt);

            // 4. 组装要传给着色器的光源信息（方法B：一次性传数组）
            //    - 先把 CPU 端各 LightComponent 整理好
            //    - 如果是 Flashlight，需要把 position/direction = 相机位置/前方向
            std::vector<LightComponent> validLights;
            validLights.reserve(mLights.size());
            for (auto lightEnt : mLights) {
                auto& lightComp = mECS->getComponent<LightComponent>(lightEnt);

                // 如果要让灯光的位置跟随它的 TransformComponent，可在此更新
                // （如果本就存储在 LightComponent.position，则可忽略）
                // tryUpdateLightPosFromTransform(lightEnt, lightComp);

                // 如果是手电筒(Flashlight)，更新为相机信息
                if (lightComp.type == LightType::Flashlight) {
                    lightComp.position = cameraPos;
                    // 这里假设相机的朝向在 CameraSystem 或别的地方已经算好
                    // 你也可用 yaw/pitch 计算方向
                    glm::vec3 frontDir  = getCameraFrontDirection(camEnt);
                    lightComp.direction = frontDir;
                }

                validLights.push_back(lightComp);
            }

            // 5. 绘制场景中的可渲染实体
            for (auto entity : mEntities) {
                auto& mesh = mECS->getComponent<MeshRenderComponent>(entity);

                // 5.1 获取并计算 model 矩阵
                glm::mat4 model(1.0f);
                if (mECS->hasComponent<TransformComponent>(entity)) {
                    auto& trans = mECS->getComponent<TransformComponent>(entity);
                    model       = calculateModelMatrix(trans);
                }

                // 5.2 激活对应的着色器
                glUseProgram(mesh.shaderID);

                // 5.3 设置常规 uniform（model / view / projection / cameraPos）
                setMat4(mesh.shaderID, "model", model);
                setMat4(mesh.shaderID, "view", view);
                setMat4(mesh.shaderID, "projection", projection);
                setVec3(mesh.shaderID, "viewPos", cameraPos);

                // 5.4 设置光源相关 uniform
                //     这里假设你的 object.frag 改成：
                //     uniform int numLights;
                //     uniform Light lights[MAX_LIGHTS];
                //     struct Light { int type; vec3 position; ... };
                if (mesh.useLight) {
                    setInt(mesh.shaderID, "numLights", (int)validLights.size());
                    for (int i = 0; i < (int)validLights.size(); i++) {
                        auto& L            = validLights[i];
                        std::string prefix = "lights[" + std::to_string(i) + "]";

                        // 传递 type（0=Directional,1=Point,2=Spot,3=Flashlight）
                        setInt(mesh.shaderID, prefix + ".type", (int)L.type);

                        setVec3(mesh.shaderID, prefix + ".position", L.position);
                        setVec3(mesh.shaderID, prefix + ".direction", L.direction);

                        setFloat(mesh.shaderID, prefix + ".cutOff", glm::cos(glm::radians(L.cutOff)));
                        setFloat(mesh.shaderID, prefix + ".outerCutOff", glm::cos(glm::radians(L.outerCutOff)));

                        setVec3(mesh.shaderID, prefix + ".ambient", L.ambient);
                        setVec3(mesh.shaderID, prefix + ".diffuse", L.diffuse);
                        setVec3(mesh.shaderID, prefix + ".specular", L.specular);

                        setFloat(mesh.shaderID, prefix + ".constant", L.constant);
                        setFloat(mesh.shaderID, prefix + ".linear", L.linear);
                        setFloat(mesh.shaderID, prefix + ".quadratic", L.quadratic);
                    }
                }

                // 5.5 设置材质相关 uniform（diffuse / specular / shininess），并绑定纹理
                //     比如你的 object.frag 里可能定义：
                //        struct Material { sampler2D diffuse; sampler2D specular; float shininess; };
                //        uniform Material material;
                //     那么就要:
                //        glUniform1i(glGetUniformLocation(shaderID, "material.diffuse"), 0);
                //        glUniform1i(glGetUniformLocation(shaderID, "material.specular"),1);
                //        glUniform1f(... "material.shininess", mesh.shininess*128.0f)  // 与frag对应
                //     这里仅示例，具体细节随你项目
                if (mesh.textures.size() > 0) {
                    setInt(mesh.shaderID, "material.diffuse", 0);   // 纹理单元0
                    setInt(mesh.shaderID, "material.specular", 1);  // 纹理单元1
                    setFloat(mesh.shaderID, "material.shininess", mesh.shininess);

                    // 绑定实际纹理到纹理单元
                    // 若 mesh.textures[0] 是 diffuse, mesh.textures[1] 是 specular
                    // 则:
                    if (mesh.textures.size() > 0) {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, mesh.textures[0]);
                    }
                    if (mesh.textures.size() > 1) {
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, mesh.textures[1]);
                    }
                } else {
                    // 6. 若要渲染“灯立方体”（例如 PointLight/SpotLight）：
                    // 如果没有纹理，就用颜色代替
                    setVec3(mesh.shaderID, "color", mesh.color);
                }

                // 5.6 绘制
                glBindVertexArray(mesh.VAO);
                if (mesh.useIndex) {
                    glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indexCount, GL_UNSIGNED_INT, 0);
                } else {
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mesh.vertexCount);
                }
                glBindVertexArray(0);
            }

            // 7. 交换前后台缓冲
            glfwSwapBuffers(window);
        }
    }

   private:
    //==== 工具函数，你可放在别的地方，也可写lambda =====//

    // 计算模型矩阵
    glm::mat4 calculateModelMatrix(const TransformComponent& trans) const {
        glm::mat4 model(1.0f);
        // 位移
        model = glm::translate(model, trans.position);
        // 旋转（四元数）
        model *= glm::mat4_cast(trans.rotation);
        // 缩放
        model = glm::scale(model, trans.scale);
        return model;
    }

    // 计算相机的view矩阵（以CameraComponent + TransformComponent为例）
    glm::mat4 calculateViewMatrix(Entity camEnt) {
        // 可以在CameraSystem里计算后存到cam.viewMatrix，这里直接获取也行
        // 简易示例：假设相机Transform即为 position & rotation
        auto& camTrans = mECS->getComponent<TransformComponent>(camEnt);

        // 取相机的朝向方向：假设 forward = (0,0,-1) 通过 四元数旋转
        glm::vec3 forward = camTrans.rotation * glm::vec3(0, 0, -1);
        glm::vec3 up      = camTrans.rotation * glm::vec3(0, 1, 0);

        return glm::lookAt(camTrans.position, camTrans.position + forward, up);
    }

    glm::vec3 getCameraWorldPosition(Entity camEnt) {
        auto& camTrans = mECS->getComponent<TransformComponent>(camEnt);
        return camTrans.position;
    }

    // 如果你用 yaw/pitch 管理相机朝向，那么可以写一个函数来把 yaw/pitch -> 前方向
    glm::vec3 getCameraFrontDirection(Entity camEnt) {
        auto& cam = mECS->getComponent<CameraComponent>(camEnt);
        // 由 yaw, pitch 计算前方向
        float yawRad   = glm::radians(cam.yaw);
        float pitchRad = glm::radians(cam.pitch);
        glm::vec3 front;
        front.x = cosf(yawRad) * cosf(pitchRad);
        front.y = sinf(pitchRad);
        front.z = sinf(yawRad) * cosf(pitchRad);
        return glm::normalize(front);
    }

    // 下面是一些简易的 uniform 设置函数
    // 实际项目可用更好的封装（例如封装到 Shader 类）
    void setInt(unsigned int shaderID, const std::string& name, int value) {
        GLint loc = glGetUniformLocation(shaderID, name.c_str());
        if (loc >= 0) glUniform1i(loc, value);
    }
    void setFloat(unsigned int shaderID, const std::string& name, float value) {
        GLint loc = glGetUniformLocation(shaderID, name.c_str());
        if (loc >= 0) glUniform1f(loc, value);
    }
    void setVec3(unsigned int shaderID, const std::string& name, const glm::vec3& vec) {
        GLint loc = glGetUniformLocation(shaderID, name.c_str());
        if (loc >= 0) glUniform3f(loc, vec.x, vec.y, vec.z);
    }
    void setMat4(unsigned int shaderID, const std::string& name, const glm::mat4& mat) {
        GLint loc = glGetUniformLocation(shaderID, name.c_str());
        if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }

   private:
    // 需要渲染的相机和光源实体
    std::vector<Entity> mCameras;
    std::vector<Entity> mLights;
};
