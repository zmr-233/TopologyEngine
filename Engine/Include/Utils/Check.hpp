#ifndef UTILS_HPP
#define UTILS_HPP
#include "Settings.hpp"
#include "Marco.hpp"

#define CHECK_PTR(ptr)                  \
    if (ptr == nullptr) {               \
        ERROR("Source Nullptr Error!"); \
        glfwTerminate();                \
        exit(1);                        \
    }

#define CHECK_BOOL(bool_var)         \
    if (!bool_var) {                 \
        ERROR("Source Bool Error!"); \
        exit(1);                     \
    }

int* get_status();
char* get_status_char();
#define infoLogSize 512

#define CHECK_SHADER_COMPILE(shader_var)                                         \
    glGetShaderiv(shader_var, GL_COMPILE_STATUS, get_status());                  \
    if (!*get_status()) {                                                        \
        glGetShaderInfoLog(shader_var, infoLogSize, nullptr, get_status_char()); \
        ERROR("Shader Compilation Error: %s", get_status_char());                \
        exit(1);                                                                 \
    }

#define CHECK_PROGRAM_LINK(shader_var)                                            \
    glGetProgramiv(shader_var, GL_LINK_STATUS, get_status());                     \
    if (!*get_status()) {                                                         \
        glGetProgramInfoLog(shader_var, infoLogSize, nullptr, get_status_char()); \
        ERROR("Shader Link Error: %s", get_status_char());                        \
        exit(1);                                                                  \
    }
#endif