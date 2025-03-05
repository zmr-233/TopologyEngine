#include "shader.hpp"

Shader::Shader(std::string& vertexPath, std::string& fragmentPath) : ID(0) {
    // 1. 从文件路径中获取顶点/片段着色器
    std::fstream vertexFile, fragmentFile;
    std::string vertexCode, fragmentCode;
    try {
        vertexFile.open(vertexPath);
        fragmentFile.open(fragmentPath);
        std::stringstream vertexStream, fragmentStream;
        vertexStream << vertexFile.rdbuf();
        fragmentStream << fragmentFile.rdbuf();
        vertexFile.close();
        fragmentFile.close();
        vertexCode   = vertexStream.str();
        fragmentCode = fragmentStream.str();
    } catch (std::ios_base::failure& e) {
        ERROR("Failed to open shader file: %s\n", e.what());
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. 编译着色器
    //  顶点着色器
    uint vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    CHECK_SHADER_COMPILE(vertexShader);
    // 片段着色器
    uint fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    CHECK_SHADER_COMPILE(fragmentShader);
    // 着色器程序 & 链接着色器
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    CHECK_PROGRAM_LINK(ID);
    // 删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, std::initializer_list<bool> vlist) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location < 0) return;

    auto it = vlist.begin();
    switch (vlist.size()) {
        case 1:
            glUniform1i(location, (*it) ? 1 : 0);
            break;
        case 2:
            glUniform2i(location, it[0] ? 1 : 0, it[1] ? 1 : 0);
            break;
        case 3:
            glUniform3i(location, it[0] ? 1 : 0, it[1] ? 1 : 0, it[2] ? 1 : 0);
            break;
        case 4:
            glUniform4i(location, it[0] ? 1 : 0, it[1] ? 1 : 0, it[2] ? 1 : 0, it[3] ? 1 : 0);
            break;
        default:
            ERROR("Unsupported float vector size: %d\n", vlist.size());
    }
}

void Shader::setInt(const std::string& name, std::initializer_list<int> vlist) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location < 0) return;

    auto it = vlist.begin();
    switch (vlist.size()) {
        case 1:
            glUniform1i(location, *it);
            break;
        case 2:
            glUniform2i(location, it[0], it[1]);
            break;
        case 3:
            glUniform3i(location, it[0], it[1], it[2]);
            break;
        case 4:
            glUniform4i(location, it[0], it[1], it[2], it[3]);
            break;
        default:
            ERROR("Unsupported float vector size: %d\n", vlist.size());
    }
}

void Shader::setFloat(const std::string& name, std::initializer_list<float> vlist) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location < 0) return;

    auto it = vlist.begin();
    switch (vlist.size()) {
        case 1:
            glUniform1f(location, *it);
            break;
        case 2:
            glUniform2f(location, it[0], it[1]);
            break;
        case 3:
            glUniform3f(location, it[0], it[1], it[2]);
            break;
        case 4:
            glUniform4f(location, it[0], it[1], it[2], it[3]);
            break;
        default:
            ERROR("Unsupported float vector size: %d\n", vlist.size());
    }
}

void Shader::setFloat(const std::string& name, glm::vec3& v) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location < 0) return;

    glUniform3f(location, v.x, v.y, v.z);
}

void Shader::setMat4(const std::string& name, glm::mat4 mat) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location < 0) return;

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}