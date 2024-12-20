#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// 用GLM变换
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// 屏幕的设置
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

// 着色器加载工具
#include <fstream>
#include <sstream>
#include <string>

std::string loadShaderSource(const char* filePath) {
    std::ifstream file;
    std::stringstream buf;
    std::string ret = "";
    file.open(filePath);
    if (file.is_open()) {
        buf << file.rdbuf();
        ret = buf.str();
    } else {
        std::cout << "Could not open " << filePath << std::endl;
    }
    file.close();
    return ret;
}

GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    // 检查编译错误
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader Compilation Failed\n"
                  << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader   = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    // 创建程序
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 检查链接错误
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Program Linking Failed\n"
                  << infoLog << std::endl;
    }
    // 删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

// 定义立方体顶点
float cubeVertices[] = {
    // Positions          // Normals
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
    1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
    1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
    1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,

    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,

    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

    -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
    1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,

    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

// 定义地面顶点
float groundVertices[] = {
    // Positions          // Normals
    -10.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f,
    10.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f,
    10.0f, 0.0f, 10.0f, 0.0f, 1.0f, 0.0f,
    10.0f, 0.0f, 10.0f, 0.0f, 1.0f, 0.0f,
    -10.0f, 0.0f, 10.0f, 0.0f, 1.0f, 0.0f,
    -10.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f};

// 着色器源代码
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main(){
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal  = mat3(transpose(inverse(model))) * aNormal;  
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 objectColor;
uniform vec3 lightColor;

void main(){
    // 环境光
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
  	
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 镜面光
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

// 简单的阴影体着色器（仅用于生成阴影体几何体）
const char* shadowVertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

void main(){
    // 将顶点推远到无限远
    vec4 pos = projection * view * model * vec4(aPos, 1.0);
    gl_Position = pos;
}
)";

const char* shadowFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main(){
    // 阴影体颜色不重要，因为我们使用模板缓冲区
    FragColor = vec4(0.0);
}
)";

int main() {
    // 初始化GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置OpenGL版本 (3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shadow Volume Cube", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 配置OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);  // 默认不改变模板缓冲

    // 创建着色器程序
    GLuint shaderProgram       = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    GLuint shadowShaderProgram = createShaderProgram(shadowVertexShaderSource, shadowFragmentShaderSource);

    // 设置立方体VAO和VBO
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // 设置地面VAO和VBO
    GLuint groundVAO, groundVBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // 获取Uniform位置
    glUseProgram(shaderProgram);
    GLint modelLoc       = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc        = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc        = glGetUniformLocation(shaderProgram, "projection");
    GLint lightPosLoc    = glGetUniformLocation(shaderProgram, "lightPos");
    GLint viewPosLoc     = glGetUniformLocation(shaderProgram, "viewPos");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLint lightColorLoc  = glGetUniformLocation(shaderProgram, "lightColor");

    // 设置视图和投影矩阵
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view       = glm::lookAt(
        glm::vec3(-4.0f, 6.0f, -8.0f),  // 摄像机位置:放在物体侧面,稍微抬高一些
        glm::vec3(0.0f, 0.0f, 0.0f),  // 观察目标点:保持看向原点
        glm::vec3(0.0f, 1.0f, 0.0f)   // 上方向向量:保持不变
    );

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // 设置光源和视点位置
    glm::vec3 lightPos(10.0f, 10.0f, 10.0f);
    glm::vec3 viewPos(5.0f, 5.0f, 5.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // 清除颜色、深度和模板缓冲
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // 渲染地面
        glUseProgram(shaderProgram);
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(0.6f, 0.6f, 0.6f)));
        glBindVertexArray(groundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 渲染立方体
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));  // 悬浮在空中
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(0.2f, 0.7f, 0.3f)));
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // 1. 渲染阴影体
        // 设置模板缓冲区操作
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        glStencilMask(0xFF);
        glDepthMask(GL_FALSE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        // 使用阴影体着色器
        glUseProgram(shadowShaderProgram);
        GLint shadowModelLoc    = glGetUniformLocation(shadowShaderProgram, "model");
        GLint shadowViewLoc     = glGetUniformLocation(shadowShaderProgram, "view");
        GLint shadowProjLoc     = glGetUniformLocation(shadowShaderProgram, "projection");
        GLint shadowLightPosLoc = glGetUniformLocation(shadowShaderProgram, "lightPos");

        glUniformMatrix4fv(shadowViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shadowProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(shadowLightPosLoc, 1, glm::value_ptr(lightPos));

        // 计算阴影体
        // 对每个三角形，扩展到阴影体
        // 这里只是一个简单的示例，没有实现完整的阴影体几何体
        // 完整实现需要根据正方体的边缘和光源位置生成阴影体
        // 为了简化，下面的代码不会真正生成阴影体，只是展示模板缓冲的设置

        // 实际实现需要生成扩展的几何体，并绘制它们以更新模板缓冲

        // 恢复缓冲设置
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // 2. 渲染被阴影覆盖的区域
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        // 使用简单的着色器渲染阴影
        // 这里为了示例，使用一个简单的平面作为阴影
        // 实际上阴影体会根据光源和物体位置动态生成
        // 以下代码仅为示意，并不产生真实阴影效果

        // 重新启用深度测试和模板测试
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);

        // 渲染阴影（示意）
        // 实际阴影渲染需要根据模板缓冲区进行阴影颜色叠加

        // 交换缓冲区和轮询事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(shadowShaderProgram);

    glfwTerminate();
    return 0;
}
