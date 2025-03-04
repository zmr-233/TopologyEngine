// clang-format off
#include <glad/glad.h>  //要在glfw前引入
#include <GLFW/glfw3.h>
// clang-format on

#include <iostream>
#include <ostream>
#include <cmath>

#include "utils.hpp"

const std::string vertexShaderSource = R"#==#(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 vertexColor; // 为片段着色器指定一个颜色输出
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // 把输出变量设置为暗红色
}
)#==#";

const std::string fragmentShaderSource = R"#==#(
#version 330 core

in vec4 vertexColor; // 从顶点着色器传来的输入变量（名称相同、类型相同）
out vec4 FragColor;
uniform vec4 ourColor; // 在OpenGL程序代码中设定这个变量
void main()
{
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = ourColor;
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
        0.5f, 0.5f, 0.0f,    // 右上角
        0.5f, -0.5f, 0.0f,   // 右下角
        -0.5f, -0.5f, 0.0f,  // 左下角
        -0.5f, 0.5f, 0.0f    // 左上角
    };

    unsigned int indices[] = {
        // 注意索引从0开始!
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3,  // 第一个三角形
        1, 2, 3   // 第二个三角形
    };

    // 顶点数组对象(Vertex Array Object, VAO) &
    // 顶点缓冲对象(Vertex Buffer Object, VBO) &
    // 索引缓冲对象(Element Buffer Object, EBO)
    uint VAO{}, VBO{}, EBO{};
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
    // 4. 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑VAO
    glBindVertexArray(0);

    auto wireframeMode = []() {  // 线框模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    };
    auto fillMode = []() {  // 填充模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    };
    // wireframeMode();

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
        float timeValue = glfwGetTime();
        float greenValue = (std::sin(timeValue) / 2.0f) + 0.5f;
        float redValue = (std::sin(timeValue) + 12.0f/ 3.0f) + 7.5f;
        float blueValue = (std::sin(timeValue) - 10.0f/ 4.0f) - 5.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");        
        glUseProgram(shaderProgram);
        glUniform4f(vertexColorLocation, redValue, greenValue, blueValue, 1.0f);

        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3); 不再使用
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
    glDeleteProgram(shaderProgram);

    // 终止GLFW
    //-----------------------------------------------------------
    glfwTerminate();
    return 0;
}