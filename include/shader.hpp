#ifndef SHADER_H
#define SHADER_H

// clang-format off
#include <glad/glad.h>  //要在glfw前引入
#include <GLFW/glfw3.h>
// clang-format on

#include <algorithm>
#include <cmath>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <sstream>
#include <utility>
#include <vector>

#include "utils.hpp"

struct Shader {
    uint ID;  // 程序ID
    Shader(std::string &vertexPath, std::string &fragmentPath);

    void use();  // 使用/激活程序
    // uniform工具函数
    void setBool(const std::string &name, std::initializer_list<bool> vlist) const;
    void setInt(const std::string &name, std::initializer_list<int> vlist) const;
    void setFloat(const std::string &name, std::initializer_list<float> vlist) const;
    void setFloat(const std::string &name, glm::vec3 &v) const;
    void setMat4(const std::string &name, glm::mat4) const;
};

#endif
