#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
// Transformations using GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Image loading with STB Image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

// Screen settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Shader sources
const char *vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos; // vertex position
layout (location = 1) in vec2 aTexCoord; // texture coordinate

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

// Function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// Time variables
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Robot states
enum RobotState
{
    IDLE,
    SHOOT,
    JUMP,
    WALK
};
RobotState currentState = IDLE;

// Movement variables
float posX = SCR_WIDTH / 2.0f; // Start at center of screen
float speed = 250.0f;          // Movement speed (pixels per second)
bool facingLeft = false;

// Animation variables
float animationTimer = 0.0f;
float animationSpeed = 10.0f; // frames per second
int walkFrame = 0;

// Sprite size
const float spriteWidth = 100.0f;  // desired sprite width
const float spriteHeight = 100.0f; // desired sprite height

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    // Set OpenGL version (3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Robot Animation", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Set callback for window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Enable modern OpenGL features
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << endl;
        return -1;
    }

    // Build and compile shader program
    // Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
             << infoLog << endl;
    }
    // Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMP d ILATION_FAILED\n"
             << infoLog << endl;
    }
    // Link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
             << infoLog << endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // positions         // texture coords
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // top left
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Flip loaded texture vertically
    stbi_set_flip_vertically_on_load(true);

    // Load textures
    unsigned int textures[9];
    glGenTextures(9, textures);

    for (int i = 0; i < 9; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        // Set texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Set texture wrapping to GL_CLAMP_TO_EDGE
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set texture filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image
        int width, height, nrChannels;
        std::string filename = "resources/p0" + std::to_string(i) + ".png";
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (data)
        {
            // Generate texture
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            cout << "Failed to load texture " << filename << endl;
        }
        stbi_image_free(data);
    }

    // Unbind VBO, VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Projection matrix
    glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);

    // Get uniform locations
    glUseProgram(shaderProgram);
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    int textureLoc = glGetUniformLocation(shaderProgram, "texture1");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Update animation timer
        if (currentState == WALK)
        {
            animationTimer += deltaTime * animationSpeed;
            // There are 6 walk frames (p03.png to p08.png)
            walkFrame = ((int)animationTimer % 6); // 0 to 5
        }
        else
        {
            animationTimer = 0.0f;
            walkFrame = 0;
        }

        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use shader
        glUseProgram(shaderProgram);

        // Bind texture
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

        // Create transformations
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(posX, spriteHeight / 2.0f, 0.0f)); // Move sprite to posX, bottom of screen
        if (facingLeft)
        {
            model = glm::scale(model, glm::vec3(-spriteWidth, spriteHeight, 1.0f));
        }
        else
        {
            model = glm::scale(model, glm::vec3(spriteWidth, spriteHeight, 1.0f));
        }

        // Set model matrix
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Render container
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Delete textures
    glDeleteTextures(9, textures);

    // Terminate GLFW
    glfwTerminate();
    return 0;
}

// Process input
void processInput(GLFWwindow *window)
{
    // ESC key to close the window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool W_Pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool SPACE_Pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool A_Pressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool D_Pressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    // Priority: SHOOT > JUMP > WALK > IDLE
    if (W_Pressed)
    {
        currentState = SHOOT;
    }
    else if (SPACE_Pressed)
    {
        currentState = JUMP;
    }
    else if (A_Pressed || D_Pressed)
    {
        currentState = WALK;
        if (A_Pressed)
        {
            posX -= speed * deltaTime;
            facingLeft = true;
        }
        if (D_Pressed)
        {
            posX += speed * deltaTime;
            facingLeft = false;
        }

        // Clamp posX to screen bounds
        float halfSpriteWidth = spriteWidth / 2.0f;
        if (posX < halfSpriteWidth)
            posX = halfSpriteWidth;
        if (posX > SCR_WIDTH - halfSpriteWidth)
            posX = SCR_WIDTH - halfSpriteWidth;
    }
    else
    {
        currentState = IDLE;
    }
}

// GLFW: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // Make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
}