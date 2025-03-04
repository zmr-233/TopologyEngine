// clang-format off
#include <glad/glad.h>  //要在glfw前引入
#include <GLFW/glfw3.h>
// clang-format on

#include <iostream>
#include <ostream>

#include "utils.hpp"

const std::string vertexShaderSource = R"#==#(
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)#==#";

const std::string fragmentShaderSource = R"#==#(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)#==#";

int main() {
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

    // 编译着色器
    //-----------------------------------------------------------
    // 顶点着色器
    uint vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    const char* shaderCode = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &shaderCode, nullptr);
    glCompileShader(vertexShader);
    CHECK_SHADER_COMPILE(vertexShader);

    // 片段着色器
    uint fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
    shaderCode = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &shaderCode, nullptr);
    glCompileShader(fragmentShader);
    CHECK_SHADER_COMPILE(fragmentShader);

    // 着色器程序 & 链接着色器
    uint shaderProgram{glCreateProgram()};
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    CHECK_SHADER_LINK(shaderProgram);
    // 激活着色器程序
    glUseProgram(shaderProgram);
    // 删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 设置顶点数据
    //-----------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};
    // 顶点数组对象(Vertex Array Object, VAO) &
    // 顶点缓冲对象(Vertex Buffer Object, VBO)
    uint VAO{}, VBO{};
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // 1.绑定VAO
    glBindVertexArray(VAO);
    // 2. 把顶点数组复制到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑VAO
    glBindVertexArray(0);

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
        // 绘制三角形
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // 交换缓冲
        //-----------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    //-----------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // 终止GLFW
    //-----------------------------------------------------------
    glfwTerminate();
    return 0;
}