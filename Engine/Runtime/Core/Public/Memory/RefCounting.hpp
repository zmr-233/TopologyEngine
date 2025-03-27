/******************************************************
 * @file Memory/RefCounting.hpp
 * @brief
 *****************************************************/

#pragma once

#include "DebugUtils/CoreDebug.hpp"
#include "TypeUtils/CoreType.hpp"

#include <concepts>

template <typename T>
concept RefCountable = requires(T *t) {
  { t->AddRef() } -> std::same_as<void>;
  { t->Release() } -> std::same_as<void>;
  { t->GetRefCount() } -> std::same_as<uint32>;
};

// 智能指针
// 但是引用计数是在对象内部维护的 参考:
// Engine/Source/Runtime/Core/Public/Templates/RefCounting.h
template <RefCountable ReferencedType> class TRefCountPtr {
public:
  FORCEINLINE TRefCountPtr() : Reference(nullptr) {}

  // 自身持有一个计数+1
  TRefCountPtr(ReferencedType *InReference, bool bAddRef = true) {
    Reference = InReference;
    if (Reference && bAddRef) {
      Reference->AddRef();
    }
  }

  TRefCountPtr(const TRefCountPtr &Copy) {
    Reference = Copy.Reference;
    if (Reference) {
      Reference->AddRef();
    }
  }

  FORCEINLINE TRefCountPtr(TRefCountPtr &&Move) {
    Reference = Move.Reference;
    Move.Reference = nullptr;
  }

  ~TRefCountPtr() {
    if (Reference) {
      Reference->Release();
    }
  }

public:
  FORCEINLINE ReferencedType *operator->() const { return Reference; }

  FORCEINLINE ReferencedType *GetReference() const { return Reference; }

  FORCEINLINE bool IsValid() const { return Reference != nullptr; }

  uint32 GetRefCount() {
    uint32 Result = 0;
    if (Reference) {
      Result = Reference->GetRefCount();
      check(Result > 0); // you should never have a zero ref count if there is a
                         // live ref counted pointer (*this is live)
    }
    return Result;
  }

private:
  ReferencedType *Reference;
};