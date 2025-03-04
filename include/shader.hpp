#ifndef SHADER_H
#define SHADER_H

// clang-format off
#include <glad/glad.h>  //要在glfw前引入
#include <GLFW/glfw3.h>
// clang-format on

#include <cmath>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <initializer_list>

#include "utils.hpp"

struct Shader {
    uint ID;  // 程序ID
    Shader(std::string& vertexPath, std::string& fragmentPath);

    void use();  // 使用/激活程序
    // uniform工具函数
    void setBool(const std::string &name, std::initializer_list<bool> vlist) const;
    void setInt(const std::string &name, std::initializer_list<int> vlist) const;
    void setFloat(const std::string &name, std::initializer_list<float> vlist) const;
};

#endif
