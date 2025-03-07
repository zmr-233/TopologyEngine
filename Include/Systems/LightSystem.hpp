/****************************************
 * File: Include/Systems/LightSystem.hpp
 * 说明：
 ****************************************/

#pragma once
#include <glad/glad.h>  // 你需要保证先包含了OpenGL头

#include <string>

#include "../Components/LightComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../ECS/System.hpp"

class LightSystem : public System {
   public:
    void init() override {}
    // 传入一个指定的Shader ID，让此系统去设置对应Uniform
    // 如果你想把光数据写到多个shader中，也可以传多个ID，或保存在资源管理器中
    void setShaderProgram(unsigned int shaderID) {
        mShaderID = shaderID;
    }

    void update(float dt) override {
        // 假设我们要把若干盏灯写到 uniform: lights[i].xxx
        // 先遍历系统内所有entity
        // 你也可以只支持 "PointLight" 若这样写
        int lightIndex = 0;
        for (auto entity : mEntities) {
            // 取出 LightComponent
            auto &light = mECS->getComponent<LightComponent>(entity);

            // 如果这个光也带有 TransformComponent，就可以拿 transform 来更新 position
            // auto &transform = mECS->getComponent<TransformComponent>(entity);
            // light.position = transform.position;

            // 写入 uniform
            setLightUniform(lightIndex, light);
            lightIndex++;
            // 这里没有做 “超过多少盏灯” 的处理，真实项目中请加限制
        }
        // 如果需要DirectionalLight/SpotLight， 也可以写更多分支
    }

   private:
    unsigned int mShaderID = 0;

    // 为了简化，这里写一个用 index 匹配 uniform数组的示例
    void setLightUniform(int idx, const LightComponent &light) {
        // 形成 uniform的前缀： 例如 "lights[0]"
        std::string prefix = "lights[" + std::to_string(idx) + "]";

        glUseProgram(mShaderID);

        // 不同LightType可走不同逻辑，这里仅演示point
        // prefix + ".type"
        glUniform1i(glGetUniformLocation(mShaderID, (prefix + ".type").c_str()), (int)light.type);

        // prefix + ".ambient"
        glUniform3fv(glGetUniformLocation(mShaderID, (prefix + ".ambient").c_str()), 1, &light.ambient[0]);
        glUniform3fv(glGetUniformLocation(mShaderID, (prefix + ".diffuse").c_str()), 1, &light.diffuse[0]);
        glUniform3fv(glGetUniformLocation(mShaderID, (prefix + ".specular").c_str()), 1, &light.specular[0]);

        // 位置/方向
        glUniform3fv(glGetUniformLocation(mShaderID, (prefix + ".position").c_str()), 1, &light.position[0]);
        glUniform3fv(glGetUniformLocation(mShaderID, (prefix + ".direction").c_str()), 1, &light.direction[0]);

        // 衰减
        glUniform1f(glGetUniformLocation(mShaderID, (prefix + ".constant").c_str()), light.constant);
        glUniform1f(glGetUniformLocation(mShaderID, (prefix + ".linear").c_str()), light.linear);
        glUniform1f(glGetUniformLocation(mShaderID, (prefix + ".quadratic").c_str()), light.quadratic);

        // 聚光
        glUniform1f(glGetUniformLocation(mShaderID, (prefix + ".cutOff").c_str()), glm::cos(glm::radians(light.cutOff)));
        glUniform1f(glGetUniformLocation(mShaderID, (prefix + ".outerCutOff").c_str()), glm::cos(glm::radians(light.outerCutOff)));
    }
};
