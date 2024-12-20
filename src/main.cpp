#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// 用GLM进行变换
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

// ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// 屏幕的设置
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

// 顶点结构
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
};

// 立方体的顶点数据
std::vector<Vertex> cubeVertices = {
    // 前面
    {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    // 后面
    {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    // 左面
    {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
    // 右面
    {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
    {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
    {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
    {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
    // 上面
    {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
    {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
    {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
    {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
    // 下面
    {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
};

// 立方体的索引数据
std::vector<GLuint> cubeIndices = {
    // 前面
    0, 1, 2, 2, 3, 0,
    // 后面
    4, 5, 6, 6, 7, 4,
    // 左面
    8, 9, 10, 10, 11, 8,
    // 右面
    12, 13, 14, 14, 15, 12,
    // 上面
    16, 17, 18, 18, 19, 16,
    // 下面
    20, 21, 22, 22, 23, 20};

// 地面的顶点数据
std::vector<Vertex> groundVertices = {
    {glm::vec3(-10.0f, -1.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
    {glm::vec3(10.0f, -1.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
    {glm::vec3(10.0f, -1.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
    {glm::vec3(-10.0f, -1.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
};

// 地面的索引数据
std::vector<GLuint> groundIndices = {
    0, 1, 2, 2, 3, 0};

// 着色器编译工具函数
GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    // 检查编译错误
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::string shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cerr << shaderType << " SHADER COMPILATION FAILED\n"
                  << infoLog << std::endl;
    }
    return shader;
}

// 着色器链接工具函数
GLuint createProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader   = compileShader(vertexSrc, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSrc, GL_FRAGMENT_SHADER);
    GLuint program        = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    // 检查链接错误
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "SHADER PROGRAM LINKING FAILED\n"
                  << infoLog << std::endl;
    }
    // 删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

// 主函数
int main() {
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // 创建窗口
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shadow Volume Example", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    // 配置OpenGL全局状态
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // 设置视口
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // 编写着色器代码
    const char* vertexShaderSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 FragPos;
    out vec3 Normal;

    void main(){
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
    )";

    const char* fragmentShaderSrc = R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 FragPos;
    in vec3 Normal;

    uniform vec3 lightPos;
    uniform vec3 viewPos;
    uniform vec3 objectColor;
    uniform vec3 lightColor;

    void main(){
        // 环境光
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        // 漫反射
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // 镜面反射
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;

        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
    }
    )";

    // 创建着色器程序
    GLuint shaderProgram = createProgram(vertexShaderSrc, fragmentShaderSrc);

    // 着色器用于渲染阴影体
    const char* shadowVertexShaderSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    uniform mat4 lightSpaceMatrix;

    void main(){
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
    )";

    const char* shadowFragmentShaderSrc = R"(
    #version 330 core
    void main(){
        // 空着色器，仅写入模板缓冲区
    }
    )";

    GLuint shadowShaderProgram = createProgram(shadowVertexShaderSrc, shadowFragmentShaderSrc);

    // 立方体VAO和VBO
    GLuint cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(Vertex), &cubeVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(GLuint), &cubeIndices[0], GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 地面VAO和VBO
    GLuint groundVAO, groundVBO, groundEBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);

    glBindVertexArray(groundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, groundVertices.size() * sizeof(Vertex), &groundVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, groundIndices.size() * sizeof(GLuint), &groundIndices[0], GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // ImGui 初始化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 光源位置
    glm::vec3 lightPos(5.0f, 5.0f, 5.0f);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        glfwPollEvents();

        // 开始新的ImGui帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui窗口
        {
            ImGui::Begin("Shadow Settings");
            ImGui::SliderFloat3("Light Position", glm::value_ptr(lightPos), -10.0f, 10.0f);
            ImGui::End();
        }

        // 渲染
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // 设置视图和投影矩阵
        glm::mat4 view = glm::lookAt(
            glm::vec3(-4.0f, 6.0f, -8.0f),  // 摄像机位置:放在物体侧面,稍微抬高一些
            glm::vec3(0.0f, 0.0f, 0.0f),    // 观察目标点:保持看向原点
            glm::vec3(0.0f, 1.0f, 0.0f)     // 上方向向量:保持不变
        );
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // 使用着色器程序
        glUseProgram(shaderProgram);

        // 传递统一变量
        GLint modelLoc       = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc        = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc        = glGetUniformLocation(shaderProgram, "projection");
        GLint lightPosLoc    = glGetUniformLocation(shaderProgram, "lightPos");
        GLint viewPosLoc     = glGetUniformLocation(shaderProgram, "viewPos");
        GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        GLint lightColorLoc  = glGetUniformLocation(shaderProgram, "lightColor");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(glm::vec3(7.0f, 7.0f, 7.0f)));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f)));

        // 渲染地面
        glm::mat4 groundModel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(groundModel));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(0.3f, 0.3f, 0.3f)));
        glBindVertexArray(groundVAO);
        glDrawElements(GL_TRIANGLES, groundIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 渲染立方体
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel           = glm::translate(cubeModel, glm::vec3(0.0f, 1.0f, 0.0f));
        cubeModel           = glm::rotate(cubeModel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeModel));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(0.8f, 0.1f, 0.1f)));
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲区
        glfwSwapBuffers(window);
    }

    // 清理资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &groundEBO);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(shadowShaderProgram);

    glfwTerminate();
    return 0;
}
