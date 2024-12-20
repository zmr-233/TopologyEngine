#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
// 用GLM变换
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 用STB Image来加载图片
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

// 屏幕的设置
const unsigned int SCR_WIDTH     = 800;
const unsigned int SCR_HEIGHT    = 600;
const char *vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos; //vertex position
layout (location = 1) in vec2 aTexCoord; //texture coordinate

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char *fragment_shader_source = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";

// 先把函数声明放这
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// 关于时间的变量
float deltaTime = 0.0f;  // 当前帧与上一帧之间的时间
float lastFrame = 0.0f;  // 上一帧的时间

// 机器人现在的状态
enum RobotState {
    IDLE,
    SHOOT,
    JUMP,
    WALK
};
RobotState currentState = IDLE;

// 移动用的变量
float posX      = SCR_WIDTH / 2.0f;  // 一开始在屏幕中间
float speed     = 250.0f;            // 移动速度 像素每秒
bool facingLeft = false;

// 动画相关的变量
float animationTimer = 0.0f;
float animationSpeed = 10.0f;  // 每秒多少帧
int walkFrame        = 0;

// 精灵的宽高
const float spriteWidth  = 100.0f;  // 精灵宽度
const float spriteHeight = 100.0f;  // 精灵高度

int main() {
    // 先初始化GLFW
    if (!glfwInit()) {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    // 设置OpenGL版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Robot Animation", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // 设回调函数，当窗口大小改变时用
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 再初始化GLEW
    glewExperimental = GL_TRUE;  // 开启现代OpenGL功能
    GLenum err       = glewInit();
    if (err != GLEW_OK) {
        cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << endl;
        return -1;
    }

    // 开始构建和编译着色器程序
    // 顶点着色器的部分
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertexShader);
    // 检查有没有编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
             << infoLog << endl;
    }
    // 片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragmentShader);
    // 再检查有没有编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
             << infoLog << endl;
    }
    // 链接着色器
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 检查有没有链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
             << infoLog << endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 设置顶点数据/缓冲并配置顶点属性
    float vertices[] = {
        // 位置              //纹理坐标
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,   // 左上
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,    // 右上
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,   // 右下
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // 左下
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f    // 左上
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 先绑定VAO，再绑定并设置顶点缓冲，然后配置顶点属性
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 位置属性 location = 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性 location = 1
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 把加载的纹理上下翻转
    stbi_set_flip_vertically_on_load(true);
    // 加载纹理
    unsigned int textures[9];
    glGenTextures(9, textures);

    for (int i = 0; i < 9; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        // 设置纹理环绕和过滤方式 在当前绑定的纹理对象上
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // 设置纹理环绕为GL_CLAMP_TO_EDGE
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 设置纹理过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 加载图片
        int width, height, nrChannels;
        std::string filename = "resources/p0" + std::to_string(i) + ".png";
        unsigned char *data  = stbi_load(filename.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (data) {
            // 生成纹理
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            cout << "Failed to load texture " << filename << endl;
        }
        stbi_image_free(data);
    }

    // 解绑VBO，VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 开启透明混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 投影矩阵
    glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);

    // 获取uniform位置
    glUseProgram(shaderProgram);
    int modelLoc   = glGetUniformLocation(shaderProgram, "model");
    int projLoc    = glGetUniformLocation(shaderProgram, "projection");
    int textureLoc = glGetUniformLocation(shaderProgram, "texture1");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // 主循环开始
    while (!glfwWindowShouldClose(window)) {
        // 每帧时间逻辑
        float currentFrame = glfwGetTime();
        deltaTime          = currentFrame - lastFrame;
        lastFrame          = currentFrame;

        // 处理输入
        processInput(window);

        // 更新动画计时器
        if (currentState == WALK) {
            animationTimer += deltaTime * animationSpeed;
            // 一共有6帧行走动画 p03.png到p08.png
            walkFrame = ((int)animationTimer % 6);  // 0到5
        } else {
            animationTimer = 0.0f;
            walkFrame      = 0;
        }

        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 使用着色器
        glUseProgram(shaderProgram);

        // 绑定纹理
        int textureIndex = 0;
        if (currentState == IDLE)
            textureIndex = 0;
        else if (currentState == SHOOT)
            textureIndex = 1;
        else if (currentState == JUMP)
            textureIndex = 2;
        else if (currentState == WALK)
            textureIndex = 3 + walkFrame;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);
        glUniform1i(textureLoc, 0);

        // 创建变换
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(posX, spriteHeight / 2.0f, 0.0f));  // 把精灵移动到posX，屏幕底部
        if (facingLeft) {
            model = glm::scale(model, glm::vec3(-spriteWidth, spriteHeight, 1.0f));
        } else {
            model = glm::scale(model, glm::vec3(spriteWidth, spriteHeight, 1.0f));
        }

        // 设置模型矩阵
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // 绘制容器
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 交换缓冲区并查询IO事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // 删除纹理
    glDeleteTextures(9, textures);

    // 退出GLFW
    glfwTerminate();
    return 0;
}

// 处理输入
void processInput(GLFWwindow *window) {
    // 按ESC键关闭窗口
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool W_Pressed     = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool SPACE_Pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool A_Pressed     = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool D_Pressed     = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    // 优先级：开火 > 跳跃 > 行走 > 静止
    if (W_Pressed) {
        currentState = SHOOT;
    } else if (SPACE_Pressed) {
        currentState = JUMP;
    } else if (A_Pressed || D_Pressed) {
        currentState = WALK;
        if (A_Pressed) {
            posX -= speed * deltaTime;
            facingLeft = true;
        }
        if (D_Pressed) {
            posX += speed * deltaTime;
            facingLeft = false;
        }

        // 把posX限制在屏幕范围内
        float halfSpriteWidth = spriteWidth / 2.0f;
        if (posX < halfSpriteWidth)
            posX = halfSpriteWidth;
        if (posX > SCR_WIDTH - halfSpriteWidth)
            posX = SCR_WIDTH - halfSpriteWidth;
    } else {
        currentState = IDLE;
    }
}

// GLFW: 当窗口大小改变 被操作系统或用户调整 时，这回调函数会被调用
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // 确保视口和新的窗口尺寸匹配
    glViewport(0, 0, width, height);
}