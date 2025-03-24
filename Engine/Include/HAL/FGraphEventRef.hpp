/****************************************
 * FILE: Engine/Include/HAL/FGraphEventRef.hpp
 ****************************************/
#pragma once

#include <atomic>
#include <cassert>

/***************************************************
 * 1) 简易 TRefCountPtr 实现
 ***************************************************/
template <typename ReferencedType>
class TRefCountPtr {
   public:
    TRefCountPtr() : Reference(nullptr) {}

    TRefCountPtr(ReferencedType* InReference)
        : Reference(InReference) {
    }

    // 拷贝构造
    TRefCountPtr(const TRefCountPtr& Other)
        : Reference(Other.Reference) {
    }

    // 移动构造
    TRefCountPtr(TRefCountPtr&& Other) noexcept
        : Reference(Other.Reference) {
        Other.Reference = nullptr;
    }

    ~TRefCountPtr() {
        if (Reference) {
            Reference = nullptr;
        }
    }

    // 拷贝赋值
    TRefCountPtr& operator=(const TRefCountPtr& Other) {
        if (this != &Other) {
            Reference = Other.Reference;
        }
        return *this;
    }

    // 移动赋值
    TRefCountPtr& operator=(TRefCountPtr&& Other) noexcept {
        if (this != &Other) {
            Reference       = Other.Reference;
            Other.Reference = nullptr;
        }
        return *this;
    }

    bool operator==(std::nullptr_t) const { return Reference == nullptr; }
    bool operator!=(std::nullptr_t) const { return Reference != nullptr; }

    ReferencedType* GetReference() const { return Reference; }
    ReferencedType* operator->() const { return Reference; }
    operator bool() const { return (Reference != nullptr); }

   private:
    ReferencedType* Reference;
};

class FBaseGraphTask;
// 统一类型别名：和 UE 一致
using FGraphEvent    = FBaseGraphTask;
using FGraphEventRef = TRefCountPtr<FBaseGraphTask>;