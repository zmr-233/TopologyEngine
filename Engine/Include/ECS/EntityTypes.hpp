/***************************************
 * FILE: Engine/Include/ECS/EntityTypes.hpp
 * 说明：实体类型定义 & Signature定义
 ***************************************/
#pragma once

#include <bitset>
#include <cstdint>

constexpr std::size_t MAX_COMPONENTS = 32;  // 最大支持32种组件
constexpr std::size_t MAX_ENTITIES   = 5000;  // 最多同时存在5000个实体

using Entity    = std::uint32_t;
using Signature = std::bitset<MAX_COMPONENTS>;  // 组件签名
