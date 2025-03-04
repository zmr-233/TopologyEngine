#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "define.hpp"
#include "stb_image.h"

int main() {
    // 全局设置
    //-----------------------------------------------------------
    stbi_set_flip_vertically_on_load(true);  // 设置图片上下翻转

    // 初始化GLFW 并配置GLFW
    //-----------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 窗口对象
    //-----------------------------------------------------------
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
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

    // 着色器类
    //-----------------------------------------------------------
    std::string vFilePath = "res/shaders/shader.vert";
    std::string fFilePath = "res/shaders/shader.frag";
    Shader context(vFilePath, fFilePath);

    // 设置顶点数据
    //-----------------------------------------------------------
    // clang-format off
    float vertices[] = {
        //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
             0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
        };
    // clang-format on

    unsigned int indices[] = {
        0, 1, 3,  // 第一个三角形
        1, 2, 3   // 第二个三角形
    };

    // 顶点数组对象(Vertex Array Object, VAO) &
    // 顶点缓冲对象(Vertex Buffer Object, VBO) &
    // 索引缓冲对象(Element Buffer Object, EBO)
    uint VAO{}, VBO{}, EBO{};
    std::vector<std::pair<std::string, uint>> textures{
        {"res/textures/container.jpg", 0},
        {"res/textures/awesomeface.png", 0}};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 1.绑定VAO
    glBindVertexArray(VAO);

    // 2. 把顶点数组复制到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. 把索引数组到一个索引缓冲中，供OpenGL使用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 4. 绑定纹理
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

    // *. 设置顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理坐标属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑VAO
    glBindVertexArray(0);

    // *.特殊设置
    // -----------------------------------------------------------
    auto wireframeMode = []() {  // 线框模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    };
    auto fillMode = []() {  // 填充模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    };
    // wireframeMode();

    // 循环之前
    //-----------------------------------------------------------
    context.use();
    // 告诉OpenGL每个着色器采样器属于哪个纹理单元 -- 通常只需要设置一次即可
    std::for_each(textures.begin(), textures.end(), [&, i = 0](auto& texture) mutable {
        context.setInt(std::string("texture") + std::to_string(i + 1), {i});
        i++;
    });
    // 渲染循环(Render Loop)
    //-----------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // 输入
        //-----------------------------------------------------------
        // 检查用户是否按下了返回键(Esc)
        auto processInput = [](GLFWwindow* window) {
            /*没有按下，glfwGetKey将会返回GLFW_RELEASE */
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            } /* 把WindowShouldClose属性设置为 true来关闭GLFW */
        };
        processInput(window);

        // 渲染指令
        //-----------------------------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绑定纹理
        std::for_each(textures.begin(), textures.end(), [&, i = 0](auto& texture) mutable {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture.second);
            i++;
        });

        // 旋转物体
        glm::mat4 trans(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        // 绘制物体
        context.use();
        context.setMat4("transform", trans);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_POLYGON, 0, 4);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // 交换缓冲
        //-----------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    //-----------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // 终止GLFW
    //-----------------------------------------------------------
    glfwTerminate();
    return 0;
}