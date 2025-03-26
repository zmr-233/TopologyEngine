/******************************************************
 * @file TypeUtils/Invoke.hpp
 * @brief
 *****************************************************/

#pragma once

#include "TypeUtils/MemberFuncOuter.hpp"

#include <type_traits>

template <typename T> T &&Forward(std::remove_reference_t<T> &arg) {
    return static_cast<T &&>(arg);
}

namespace TE::Core::Private {
// 处理引用类型，不做解引用
template <typename OuterType, typename TargetType>
constexpr auto DereferenceIfNecessary(TargetType &&Target, const volatile OuterType *TargetPtr)
    -> decltype((TargetType &&)Target) {
    return (TargetType &&)Target;
}

// 对于指针类型，进行解引用
template <typename OuterType, typename TargetType>
constexpr auto DereferenceIfNecessary(TargetType &&Target, ...)
    -> decltype(*(TargetType &&)Target) {
    return *(TargetType &&)Target;
}
} // namespace TE::Core::Private

template <typename FuncType, typename... ArgTypes>
auto Invoke(FuncType &&Func, ArgTypes &&...Args)
    -> decltype(Forward<FuncType>(Func)(Forward<ArgTypes>(Args)...)) {
    return Forward<FuncType>(Func)(Forward<ArgTypes>(Args)...);
}

template <typename ReturnType, typename ObjType, typename TargetType>
auto Invoke(ReturnType ObjType::*pdm, TargetType &&Target)
    -> decltype(TE::Core::Private::DereferenceIfNecessary<ObjType>(Forward<TargetType>(Target),
                                                                   &Target).*
                pdm) {
    return TE::Core::Private::DereferenceIfNecessary<ObjType>(Forward<TargetType>(Target),
                                                              &Target).*
           pdm;
}

template <typename PtrMemFunType, typename TargetType, typename... ArgTypes,
          typename ObjType = TMemberFunctionPtrOuter_T<
              PtrMemFunType> // typename std::remove_pointer<TargetType>::type>
          >
auto Invoke(PtrMemFunType PtrMemFun, TargetType &&Target, ArgTypes &&...Args)
    -> decltype((TE::Core::Private::DereferenceIfNecessary<ObjType>(Forward<TargetType>(Target),
                                                                    &Target).*
                 PtrMemFun)(Forward<ArgTypes>(Args)...)) {
    return (
        TE::Core::Private::DereferenceIfNecessary<ObjType>(Forward<TargetType>(Target), &Target).*
        PtrMemFun)(Forward<ArgTypes>(Args)...);
}

// 用于函数的投影操作
#define FUNC_PROJECTION(FuncName) \
    [](auto &&...Args) -> decltype(auto) { return FuncName(Forward<decltype(Args)>(Args)...); }

// 用于成员函数的投影操作
#define MEMBER_PROJECTION(Type, FuncName)                                                         \
    [](auto &&Obj, auto &&...Args) -> decltype(auto) {                                            \
        return TE::Core::Private::DereferenceIfNecessary<Type>(Forward<decltype(Obj)>(Obj), &Obj) \
            .FuncName(Forward<decltype(Args)>(Args)...);                                          \
    }
namespace TE::Core::Private {
// 作用是在调用时无法匹配到具体的返回类型时使用
template <typename, typename FuncType, typename... ArgTypes> struct TInvokeResult_Impl {};

// 用来处理可调用对象能够被成功调用的情况
template <typename FuncType, typename... ArgTypes>
struct TInvokeResult_Impl<decltype((void)Invoke(std::declval<FuncType>(),
                                                std::declval<ArgTypes>()...)),
                          FuncType, ArgTypes...> {
    using Type = decltype(Invoke(std::declval<FuncType>(), std::declval<ArgTypes>()...));
};
} // namespace TE::Core::Private

template <typename FuncType, typename... ArgTypes>
struct TInvokeResult : TE::Core::Private::TInvokeResult_Impl<void, FuncType, ArgTypes...> {};

template <typename FuncType, typename... ArgTypes>
using TInvokeResult_T = typename TInvokeResult<FuncType, ArgTypes...>::Type;
