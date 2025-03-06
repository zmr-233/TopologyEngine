#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "camera.hpp"
#include "define.hpp"
#include "material.hpp"
#include "stb_image.h"
#include "utils.hpp"

int main() {
    // 初始化GLFW 并配置GLFW
    //-----------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 窗口对象
    //-----------------------------------------------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    CHECK_PTR(window);
    glfwMakeContextCurrent(window);  // 将当前窗口的上下文设置为当前线程的主上下文

    // 初始化GLAD
    //-----------------------------------------------------------
    CHECK_BOOL(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0);  // 将GLFW的函数指针加载到GLAD中

    // 视口(Viewport)
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);  // 使用回调函数，确保窗口大小改变时，视口也会被调整
    });
    // 注册鼠标回调函数
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        static bool firstMouse = true;
        static float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;
        if (firstMouse) {
            // glfwSetCursorPos(window, lastX, lastY);
            lastX      = xpos;
            lastY      = ypos;
            firstMouse = false;
        } else {
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            // reversed since y-coordinates go from bottom to top
            lastX = xpos;
            lastY = ypos;
            Camera::getCamera().ProcessMouseMovement(xoffset, yoffset);
        }
    });
    // 注册滚轮回调函数
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        Camera::getCamera().ProcessMouseScroll(static_cast<float>(yoffset));
    });

    // 全局设置
    //-----------------------------------------------------------
    stbi_set_flip_vertically_on_load(true);                       // 设置图片上下翻转
    glEnable(GL_DEPTH_TEST);                                      // 启用深度测试
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // 隐藏鼠标

    // 着色器类
    //-----------------------------------------------------------
    std::string vFilePath = "res/shaders/object.vert";
    std::string fFilePath = "res/shaders/object.frag";
    std::string lFilePath = "res/shaders/light.frag";
    Shader objectCTX(vFilePath, fFilePath);
    Shader lightCTX(vFilePath, lFilePath);

    // 设置顶点数据
    //-----------------------------------------------------------
    // clang-format off
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // clang-format on
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
    std::vector<glm::vec3> lightPositions = {
        glm::vec3(1.2f, 1.0f, 2.0f),
    };

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
        {"res/textures/container2.png", 0},
        {"res/textures/container2_specular.png", 0},
        {"res/textures/matrix.jpg", 0},
        {"res/textures/container.jpg", 0},
        {"res/textures/awesomeface.png", 0}};
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

    // 渲染循环(Render Loop)
    //-----------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // 输入
        //-----------------------------------------------------------
        auto get_deltaTime = []() {
            static float deltaTime = 0.0f;
            static float lastFrame = 0.0f;
            float currentFrame     = glfwGetTime();
            deltaTime              = currentFrame - lastFrame;
            lastFrame              = currentFrame;
            return deltaTime;
        };
        auto processInput = [&]() {
            auto deltaTime = get_deltaTime();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                Camera::getCamera().ProcessKeyboard(FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                Camera::getCamera().ProcessKeyboard(BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                Camera::getCamera().ProcessKeyboard(LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                Camera::getCamera().ProcessKeyboard(RIGHT, deltaTime);
        };
        processInput();

        // 渲染指令
        //-----------------------------------------------------------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清除颜色缓冲和深度缓冲

        // 绑定纹理
        std::for_each(textures.begin(), textures.end(), [&, i = 0](auto& texture) mutable {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture.second);
            i++;
        });

        // 设置模型、视图和投影矩阵uniform
        //-----------------------------------------------------------
        auto set_view = [](Shader& context) {
            glm::mat4 view(1.0f);
            view = Camera::getCamera().GetViewMatrix();
            context.setMat4("view", view);
        };
        auto set_projection = [](Shader& context) {
            glm::mat4 projection(1.0f);
            projection = glm::perspective(glm::radians(Camera::getCamera().Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            context.setMat4("projection", projection);
        };
        /*易错：已经包含了DrawArrays 应该最后调用*/
        auto set_model = [](
                             std::vector<glm::vec3>& poses,
                             Shader& context,
                             auto extra = [](glm::mat4& model) {}) { ;
            std::for_each(poses.begin(), poses.end(), [&context, &extra](auto& pos) {
                float angle = 20.0f;
                glm::mat4 model(1.0f);
                model = glm::translate(model, pos);
                extra(model);
                model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                context.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }); };

        // 设置材质
        auto set_material = [](std::string name, Shader& context) {
            materials[name](context);
        };
        auto set_material_texture = [](Shader& context) {
            context.setInt("material.diffuse", 0);
            context.setInt("material.specular", 1);
            context.setFloat("material.shininess", 0.5f);
        };
        // 带衰减光照强度 -- 适用于自行设定参数
        auto set_light = [](glm::vec3 lightColor, glm::vec3& lightPos, Shader& context) {
            glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);    // 降低影响
            glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);  // 很低的影响

            context.setFloat("light.ambient", ambientColor);
            context.setFloat("light.diffuse", diffuseColor);
            context.setFloat("light.specular", {1.0f, 1.0f, 1.0f});

            // 衰减
            context.setFloat("light.constant", 1.0f);
            context.setFloat("light.linear", 0.09f);
            context.setFloat("light.quadratic", 0.032f);

            //位置 & 方向
            context.setFloat("light.position", Camera::getCamera().Position);
            context.setFloat("light.direction", Camera::getCamera().Front);

            // 聚光
            context.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
            context.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        };

        auto cal_lightColor = []() {
            glm::vec3 lightColor;
            lightColor.x = std::max((float)sin(glfwGetTime() * 2.0f), 0.3f);
            lightColor.y = std::max((float)sin(glfwGetTime() * 0.7f), 0.5f);
            lightColor.z = std::max((float)sin(glfwGetTime() * 1.3f), 0.4f);
            return lightColor;
        };
        auto rotate_light = [](glm::vec3 lightPos) {
            glm::mat4 model(1.0f);
            float angle = 5.0f;
            model       = glm::rotate(model, glm::radians(angle) / 20.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            lightPos    = glm::vec3(model * glm::vec4(lightPos, 1.0f));
            return lightPos;
        };

        // 旋转光源
        // lightPositions[0] = rotate_light(lightPositions[0]);
        // 光源颜色
        auto lightColor = glm::vec3(1.0f, 1.0f, 1.0f);  // cal_lightColor();

        // ====================================
        // object相关
        // ====================================
        objectCTX.use();
        glBindVertexArray(objectVAO);
        set_view(objectCTX);
        set_projection(objectCTX);
        set_material_texture(objectCTX);
        set_light(lightColor, lightPositions[0], objectCTX);
        objectCTX.setFloat("viewPos", Camera::getCamera().Position);

        set_model(cubePositions, objectCTX, [](glm::mat4& model) {});

        // ====================================
        // light相关
        // ====================================
        lightCTX.use();
        glBindVertexArray(lightVAO);
        set_view(lightCTX);
        set_projection(lightCTX);
        lightCTX.setFloat("lightColor", lightColor);

        set_model(lightPositions, lightCTX, [](glm::mat4& model) {
            model = glm::scale(model, glm::vec3(0.2f));
        });

        // 交换缓冲
        //-----------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    //-----------------------------------------------------------
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // 终止GLFW
    //-----------------------------------------------------------
    glfwTerminate();
    return 0;
}