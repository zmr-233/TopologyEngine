/****************************************
 * FILE: Engine/Include/Comps/Sys/InputSysComp.hpp
 ****************************************/
#pragma once
#include "../../ECS/System.hpp"

// ------ 1) 输入系统：InputSystem ------
// 现做成“System + SystemComp”的风格
struct InputSysComp : public SystemComp {
    // 这里可以加一些“专属字段”
    // 例如：多个窗口句柄，鼠标灵敏度等
};