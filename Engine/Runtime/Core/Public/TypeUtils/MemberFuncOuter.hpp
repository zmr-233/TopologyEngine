/******************************************************
 * @file TypeUtils/MemberFuncOuter.hpp
 * @brief
 *****************************************************/

#pragma once

// clang-format off
// 类型萃取
template <typename T>
struct TMemberFunctionPtrOuter;

template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...)                 > { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...)               & > { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...)               &&> { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...) const           > { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...) const         & > { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...) const         &&> { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...)       volatile  > { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...)       volatile& > { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...)       volatile&&> { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...) const volatile  > { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...) const volatile& > { using Type = ObjectType; };
template <typename ReturnType, typename ObjectType, typename... ArgTypes> struct TMemberFunctionPtrOuter<ReturnType (ObjectType::*)(ArgTypes...) const volatile&&> { using Type = ObjectType; };

template <typename T>
using TMemberFunctionPtrOuter_T = typename TMemberFunctionPtrOuter<T>::Type;
// clang-format on
