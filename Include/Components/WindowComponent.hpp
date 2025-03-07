/****************************************
 * File: Include/Components/WindowComponent.hpp
 * 说明：
 ****************************************/
#pragma once

#include <GLFW/glfw3.h>

#include <string>
struct WindowComponent {
    GLFWwindow* window;
    int width;
    int height;
    std::string title;

    //是否隐藏鼠标
    bool hideCursor;
};