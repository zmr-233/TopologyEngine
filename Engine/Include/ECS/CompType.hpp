/****************************************
 * FILE: Engine/Include/ECS/CompType.hpp
 * 说明：组件类型的定义
 ****************************************/

#include <concepts>
#include <type_traits>
#pragma once
// 暂时不启用
//  template <typename T>
//  concept CompTrait = requires(T& a, T& b) {
//      { a.merge(b) } -> std::same_as<T>;
//  };

constexpr std::size_t MAX_COMP_TYPES = 32;    // 最大支持32种组件
constexpr std::size_t MAX_COMP_ARRAY = 5000;  // 最大支持5000个组件数组