/******************************************************
 * @file TypeUtils/Invoke.hpp
 * @brief
 *****************************************************/

#pragma once

#include <type_traits>

#include "MemberFuncOuter.hpp"

namespace TE::Core::TypeUtils {


template <typename T>
T&& Forward(std::remove_reference_t<T>& arg) {
    return static_cast<T&&>(arg);
}

namespace Private {
// 处理引用类型，不做解引用
template <typename OuterType, typename TargetType>
constexpr auto DereferenceIfNecessary(TargetType&& Target, const volatile OuterType* TargetPtr)
    -> decltype((TargetType&&)Target) {
    return (TargetType&&)Target;
}

// 对于指针类型，进行解引用
template <typename OuterType, typename TargetType>
constexpr auto DereferenceIfNecessary(TargetType&& Target, ...)
    -> decltype(*(TargetType&&)Target) {
    return *(TargetType&&)Target;
}
}  // namespace Private

template <typename FuncType, typename... ArgTypes>
auto Invoke(FuncType&& Func, ArgTypes&&... Args)
    -> decltype(Forward<FuncType>(Func)(Forward<ArgTypes>(Args)...)) {
    return Forward<FuncType>(Func)(Forward<ArgTypes>(Args)...);
}

template <typename ReturnType, typename ObjType, typename TargetType>
auto Invoke(ReturnType ObjType::* pdm, TargetType&& Target)
    -> decltype(Private::DereferenceIfNecessary<ObjType>(Forward<TargetType>(Target), &Target).*pdm) {
    return Private::DereferenceIfNecessary<ObjType>(Forward<TargetType>(Target), &Target).*pdm;
}

template <
    typename PtrMemFunType,
    typename TargetType,
    typename... ArgTypes,
    typename ObjType = TMemberFunctionPtrOuter_T<PtrMemFunType>  // typename std::remove_pointer<TargetType>::type>
    >
auto Invoke(PtrMemFunType PtrMemFun, TargetType&& Target, ArgTypes&&... Args)
    -> decltype((Private::DereferenceIfNecessary<ObjType>(Forward<TargetType>(Target), &Target).*PtrMemFun)(Forward<ArgTypes>(Args)...)) {
    return (Private::DereferenceIfNecessary<ObjType>(Forward<TargetType>(Target), &Target).*PtrMemFun)(Forward<ArgTypes>(Args)...);
}

// // 成员函数调用封装 (const版本)
// template <
//     typename ReturnType,
//     typename ClassType,
//     typename... PtrArgTypes,
//     typename TargetType,
//     typename... ArgTypes>
// auto Invoke(ReturnType (ClassType::*PtrMemFun)(PtrArgTypes...) const, TargetType&& Target, ArgTypes&&... Args)
//     -> decltype((Private::DereferenceIfNecessary<ClassType>(Forward<TargetType>(Target), &Target).*PtrMemFun)(Forward<ArgTypes>(Args)...)) {
//     return (Private::DereferenceIfNecessary<ClassType>(Forward<TargetType>(Target), &Target).*PtrMemFun)(Forward<ArgTypes>(Args)...);
// }

// 用于函数的投影操作
#define FUNC_PROJECTION(FuncName)                          \
    [](auto&&... Args) -> decltype(auto) {                 \
        return FuncName(Forward<decltype(Args)>(Args)...); \
    }

// 用于成员函数的投影操作
#define MEMBER_PROJECTION(Type, FuncName)                                                                                           \
    [](auto&& Obj, auto&&... Args) -> decltype(auto) {                                                                              \
        return Private::DereferenceIfNecessary<Type>(Forward<decltype(Obj)>(Obj), &Obj).FuncName(Forward<decltype(Args)>(Args)...); \
    }
}  // namespace TE::Core::TypeUtils
