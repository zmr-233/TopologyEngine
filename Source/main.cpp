/******************************************************
 * File: src/main.cpp
 ******************************************************/

// clang-format off
#include <glad/glad.h>  //要在glfw前引入
#include <GLFW/glfw3.h>
// clang-format on

#include <algorithm>
#include <iostream>
#include <memory>

// 包含你前面写好的 ECSCore
#include "../Include/ECS/ECSCore.hpp"

// 各系统
#include "../Include/Systems/CameraSystem.hpp"
#include "../Include/Systems/InputSystem.hpp"
#include "../Include/Systems/LightSystem.hpp"
#include "../Include/Systems/PlayerControlSystem.hpp"
#include "../Include/Systems/RenderSystem.hpp"
#include "../Include/Systems/WindowSystem.hpp"

// 各组件
#include "../Include/Components/CameraComponent.hpp"
#include "../Include/Components/InputComponent.hpp"
#include "../Include/Components/LightComponent.hpp"
#include "../Include/Components/MeshRenderComponent.hpp"
#include "../Include/Components/PlayerControlComponent.hpp"
#include "../Include/Components/TransformComponent.hpp"
#include "../Include/Components/WindowComponent.hpp"

// 事件总线
#include "../Include/Events/Event.hpp"
#include "../Include/Events/EventBus.hpp"

// 资源/工具
#include "../Include/Resources/Shader.hpp"
#include "../Include/Resources/TmpVertices.hpp"
#include "../Include/Utils/Check.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../Include/Utils/stb_image.h"
// ... 以及你需要的glad, glfw, stb_image等

int main() {
    // 1. 创建ECSCore
    std::shared_ptr<ECSCore> ecs = std::make_shared<ECSCore>();
    // 特殊: 创建IEventBus事件总线
    std::shared_ptr<EventBus> eventBus = std::make_shared<EventBus>();

    // 2. 注册组件
    ecs->registerComponent<CameraComponent>();
    ecs->registerComponent<InputComponent>();
    ecs->registerComponent<LightComponent>();
    ecs->registerComponent<MeshRenderComponent>();
    ecs->registerComponent<PlayerControlComponent>();
    ecs->registerComponent<TransformComponent>();
    ecs->registerComponent<WindowComponent>();

    // 3. 注册系统
    // - WindowSystem
    auto windowSys = ecs->registerSystem<WindowSystem>();
    {
        Signature sig;
        // 要求实体有 WindowComponent
        sig.set(ecs->getComponentTypeIndex<WindowComponent>(), true);
        ecs->setSystemSignature<WindowSystem>(sig);
    }
    // - InputSystem
    auto inputSys = ecs->registerSystem<InputSystem>();
    {
        Signature sig;
        // 要求实体有 InputComponent
        sig.set(ecs->getComponentTypeIndex<InputComponent>(), true);
        ecs->setSystemSignature<InputSystem>(sig);
    }

    // - PlayerControlSystem
    auto playerControlSys = ecs->registerSystem<PlayerControlSystem>();
    {
        Signature sig;
        // 要求实体有 PlayerControlComponent
        sig.set(ecs->getComponentTypeIndex<PlayerControlComponent>(), true);
        ecs->setSystemSignature<PlayerControlSystem>(sig);
    }

    // - CameraSystem
    auto cameraSys = ecs->registerSystem<CameraSystem>();
    {
        Signature sig;
        // 要求实体有 CameraComponent
        sig.set(ecs->getComponentTypeIndex<CameraComponent>(), true);
        ecs->setSystemSignature<CameraSystem>(sig);
    }

    // - LightSystem
    auto lightSys = ecs->registerSystem<LightSystem>();
    {
        Signature sig;
        // 要求实体有 LightComponent
        sig.set(ecs->getComponentTypeIndex<LightComponent>(), true);
        ecs->setSystemSignature<LightSystem>(sig);
    }

    // - RenderSystem
    auto renderSys = ecs->registerSystem<RenderSystem>();
    {
        Signature sig;
        // 要求实体有 MeshRendererComponent
        sig.set(ecs->getComponentTypeIndex<MeshRenderComponent>(), true);
        ecs->setSystemSignature<RenderSystem>(sig);
    }

    // =============== 加载OpenGL函数指针 ===============
    // 初始化GLFW & 初始化GLAD => 在WindowSystem中做了
    windowSys->init();
    inputSys->init();
    playerControlSys->init();
    cameraSys->init();
    lightSys->init();
    renderSys->init();

    // =============== 创建窗口实体 =================
    Entity windowEnt = ecs->createEntity();
    {
        WindowComponent windowComp{
            .width      = 1920,
            .height     = 1080,
            .title      = "LearnOpenGL ECS",
            .hideCursor = true,
        };
        ecs->addComponent<WindowComponent>(windowEnt, windowComp);
    }

    windowSys->initWindows();

    // =============== 创建InputSystem实体 =================
    Entity inputEnt = ecs->createEntity();
    {
        InputComponent inputComp;
        ecs->addComponent<InputComponent>(inputEnt, inputComp);
    }

    // =============== 创建相机实体 =================
    Entity cameraEnt = ecs->createEntity();
    {
        CameraComponent camComp{
            .windowID         = windowEnt,  // 绑定到窗口实体
            .fov              = 45.0f,
            .nearPlane        = 0.1f,
            .farPlane         = 100.0f,
            .movementSpeed    = 4.0f,
            .mouseSensitivity = 0.5f,
            .yaw              = -90.0f,
            .pitch            = 0.0f,
        };
        ecs->addComponent<CameraComponent>(cameraEnt, camComp);
        TransformComponent transComp{
            .position = {0.0f, 0.0f, 3.0f},
            .scale    = {1.0f, 1.0f, 1.0f},
        };
        ecs->addComponent<TransformComponent>(cameraEnt, transComp);
    }

    // =============== 创建光源实体 =================
    auto genDirectionComp = [](glm::vec3 direction) -> LightComponent {
        LightComponent lightComp = {
            .type      = LightType::Directional,
            .ambient   = {0.0f, 0.0f, 0.0f},
            .diffuse   = {0.5f, 0.5f, 0.5f},
            .specular  = {1.0f, 1.0f, 1.0f},
            .direction = direction,
        };
        return lightComp;
    };
    auto genPointComp = [](glm::vec3 position) -> LightComponent {
        LightComponent lightComp = {
            .type      = LightType::Point,
            .ambient   = {0.2f, 0.2f, 0.2f},  // 环境光
            .diffuse   = {0.5f, 0.5f, 0.5f},
            .specular  = {1.0f, 1.0f, 1.0f},
            .position  = position,
            .constant  = 1.0f,
            .linear    = 0.09f,
            .quadratic = 0.032f,
        };
        return lightComp;
    };
    auto genSpotComp = [](glm::vec3 position, glm::vec3 direction) -> LightComponent {
        LightComponent lightComp = {
            .type        = LightType::Spot,
            .ambient     = {0.0f, 0.0f, 0.0f},
            .diffuse     = {0.5f, 0.5f, 0.5f},
            .specular    = {1.0f, 1.0f, 1.0f},
            .position    = position,
            .constant    = 1.0f,
            .linear      = 0.09f,
            .quadratic   = 0.032f,
            .direction   = direction,
            .cutOff      = 12.5f,
            .outerCutOff = 17.5f,
        };
        return lightComp;
    };
    auto genFlashComp = [](Entity cameraEnt) -> LightComponent {
        LightComponent lightComp = {
            .type = LightType::Flashlight,
            // .position    = position, 无需设置
            // .direction   = direction,
            .ambient     = {0.0f, 0.0f, 0.0f},
            .diffuse     = {0.5f, 0.5f, 0.5f},
            .specular    = {1.0f, 1.0f, 1.0f},
            .constant    = 1.0f,
            .linear      = 0.09f,
            .quadratic   = 0.032f,
            .cutOff      = 12.5f,
            .outerCutOff = 17.5f,
            .bindCamera  = cameraEnt,  // 绑定到相机实体
        };
        return lightComp;
    };
    // Entity lightEnt = ecs->createEntity();
    std::vector<glm::vec3> lightPositions = {
        glm::vec3(4.2f, 4.0f, 3.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
    };
    std::vector<Entity> lightEnts(4);
    for (int i = 0; i < 4; i++) {
        lightEnts[i] = ecs->createEntity();
    }
    {
        // 方向光
        ecs->addComponent<LightComponent>(lightEnts[0], genDirectionComp({-0.2f, -1.0f, -0.3f}));
        // 点光源
        // 注意: 点光源允许有实体
        ecs->addComponent<LightComponent>(lightEnts[1], genPointComp(lightPositions[0]));
        // 聚光灯
        ecs->addComponent<LightComponent>(lightEnts[2], genSpotComp(lightPositions[1], {0.0f, -1.0f, 0.0f}));
        // 手电筒
        ecs->addComponent<LightComponent>(lightEnts[3], genFlashComp(cameraEnt));
    }

    // =============== 替代资源管理类的功能 =================
    // 全局设置
    //-----------------------------------------------------------
    stbi_set_flip_vertically_on_load(true);  // 设置图片上下翻转
    // =============== 初始化 Shader ===============
    std::string vFilePath = "Resources/Shaders/object.vert";
    std::string fFilePath = "Resources/Shaders/object.frag";
    std::string lFilePath = "Resources/Shaders/light.frag";
    Shader objectCTX(vFilePath, fFilePath);
    Shader lightCTX(vFilePath, lFilePath);

    // 注意: 没有编写任何的资源管理类，所以这里的VAO/VBO等需要自行准备
    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};
    unsigned int indices[] = {
        0, 1, 3,  // 第一个三角形
        1, 2, 3   // 第二个三角形
    };
    uint objectVAO{};
    uint lightVAO{};
    uint VBO{};
    uint EBO{};

    // =============
    // 生成 VAO 和 VBO（以及 EBO，如果需要）
    glGenVertexArrays(1, &objectVAO);
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // ====================================
    // object相关 -- 1. 绑定objectVAO
    // ====================================
    glBindVertexArray(objectVAO);

    // 2. 绑定 VBO 到 GL_ARRAY_BUFFER，并为其填充数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. 绑定 EBO 到 GL_ELEMENT_ARRAY_BUFFER，并为其填充数据
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 4. 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性 ...
    // 法向量属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理坐标属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 5. 解绑VAO
    glBindVertexArray(0);

    // ====================================
    // light相关 -- 1. 绑定lightVAO
    // ====================================
    glBindVertexArray(lightVAO);

    // 2. 绑定 VBO 到 GL_ARRAY_BUFFER，并为其填充数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 使用相同的VBO数据

    // 3. 绑定EB0到GL_ELEMENT_ARRAY_BUFFER
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // 使用相同的EBO数据

    // 4. 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性 ...
    // 纹理坐标属性 ...
    // 法向量属性... (忽略)

    // 5. 解绑VAO
    glBindVertexArray(0);

    // ====================================
    // * 后处理
    // ====================================

    // *解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // *纹理数据
    // -----------------------------------------------------------
    std::vector<std::pair<std::string, uint>> textures{
        {"Resources/Textures/container2.png", 0},
        {"Resources/Textures/container2_specular.png", 0},
        {"Resources/Textures/matrix.jpg", 0},
        {"Resources/Textures/container.jpg", 0},
        {"Resources/Textures/awesomeface.png", 0}};
    std::for_each(textures.begin(), textures.end(), [i = 0](auto& texture) mutable {
        glGenTextures(1, &texture.second);
        glBindTexture(GL_TEXTURE_2D, texture.second);
        // 设置纹理环绕方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 加载纹理
        int width, height, nrChannels;
        u_char* data = stbi_load(texture.first.c_str(), &width, &height, &nrChannels, 0);
        CHECK_PTR(data);
        // 根据通道数动态设置内部格式
        GLenum internalFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        GLenum format         = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        // 设置纹理数据
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        i++;
    });

    // *.特殊设置
    // -----------------------------------------------------------
    auto wireframeMode = []() {  // 线框模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    };
    auto fillMode = []() {  // 填充模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    };
    // wireframeMode();

    // *循环之前
    //-----------------------------------------------------------
    objectCTX.use();
    // 告诉OpenGL每个着色器采样器属于哪个纹理单元 -- 通常只需要设置一次即可
    std::for_each(textures.begin(), textures.end(), [&, i = 0](auto& texture) mutable {
        objectCTX.setInt(std::string("texture") + std::to_string(i + 1), {i});
        i++;
    });

    // =============== 创建一个有 MeshRenderer 的立方体实体 =================
    std::vector<Entity> cubeEnts;
    std::transform(cubePositions.begin(), cubePositions.end(), std::back_inserter(cubeEnts), [&](auto& pos) {
        Entity cubeEnt = ecs->createEntity();
        // 加一个Transform
        TransformComponent trans{
            .position = pos,
        };
        ecs->addComponent<TransformComponent>(cubeEnt, trans);
        // 加一个MeshRenderer
        MeshRenderComponent mesh{
            .VAO         = objectVAO,
            .VBO         = VBO,
            .useIndex    = false,
            .vertexCount = 36,
            .textures    = {textures[0].second, textures[1].second},
            .shaderID    = objectCTX.ID,
            .shininess   = 0.5f,
        };
        ecs->addComponent<MeshRenderComponent>(cubeEnt, mesh);
        return cubeEnt;
    });
    // 点光源实体
    Entity plightCubeEnt = lightEnts[1];
    {
        TransformComponent trans{
            .position = lightPositions[0],
            .scale    = {0.3f, 0.3f, 0.3f},
        };
        ecs->addComponent<TransformComponent>(plightCubeEnt, trans);
        MeshRenderComponent mesh{
            .VAO         = lightVAO,
            .VBO         = VBO,
            .useIndex    = false,
            .vertexCount = 36,
            .color       = {1.0f, 1.0f, 1.0f},
            .shaderID    = lightCTX.ID,  // 使用light的shader
            .useLight    = false,        // 自发光
        };
        ecs->addComponent<MeshRenderComponent>(plightCubeEnt, mesh);
    }

    // =============== 最后的System额外初始化 =================
    inputSys->initEventBus(eventBus);
    inputSys->initWindow(windowEnt);
    cameraSys->initInputEntity(inputEnt);
    playerControlSys->initInputEntity(inputEnt);
    //^^^^^^^^^^^这个inputEntity是用来接收输入的s
    renderSys->initLightCameraEntities({cameraEnt}, lightEnts);
    // TODO: 这里要怎么才能不要这么麻烦的传入参数呢？^^^^^^^^^^^^^^^^

    // =============== 游戏循环 =================
    while (true) {
        // INFO("Game Loop");
        // 计算dt
        static float lastTime = 0.0f;
        float currentTime     = (float)glfwGetTime();
        float dt              = currentTime - lastTime;
        lastTime              = currentTime;

        // 清屏 => 交由RenderSystem处理
        // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 更新各系统
        windowSys->update(dt);
        inputSys->update(dt);
        cameraSys->update(dt);
        lightSys->update(dt);
        renderSys->update(dt);

        auto& camComp  = ecs->getComponent<CameraComponent>(cameraEnt);
        auto& camTrans = ecs->getComponent<TransformComponent>(cameraEnt);
        // DEBUG("Camera Position: (%f, %f, %f)", camTrans.position.x, camTrans.position.y, camTrans.position.z);
        // DEBUG("Camera front: (%f, %f, %f)", camComp.front.x, camComp.front.y, camComp.front.z);

        // 交换缓冲 => 交由RenderSystem处理
        // glfwSwapBuffers(windowSys->getWindow());
    }

    // 退出
    windowSys->shutdown();
    return 0;
}
