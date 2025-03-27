/******************************************************
 * @file Tasks/Tasks.hpp
 * @brief
 *****************************************************/

#pragma once

#include "Memory/RefCounting.hpp"
#include "TypeUtils/CoreType.hpp"
#include "TypeUtils/Invoke.hpp"
#include <atomic>

namespace TE::Tasks {
namespace Private {

// Engine/Source/Runtime/Core/Public/Tasks/TaskPrivate.h:120
class FTaskBase {

public:
  void AddRef() {}
  void Release() {}
  uint32 GetRefCount() { return 0; }
};

// Engine/Source/Runtime/Core/Public/Tasks/Task.h:33
class FTaskHandle {
public:
  bool IsValid() const { return false; }

  // 检查任务是否已经完成
  bool IsCompleted() const { return false; }

protected:
  TRefCountPtr<FTaskBase> Pimpl;
};
} // namespace Private

// Engine/Source/Runtime/Core/Public/Tasks/Task.h:220
template <typename ResultType> class TTask {
public:
  ResultType Result;
  TTask(ResultType Result) : Result(Result) {}
};

// Engine/Source/Runtime/Core/Public/Tasks/Task.h:299
template <typename TaskBodyType>
TTask<TInvokeResult_T<TaskBodyType>> Launch(const TCHAR *DebugName,
                                            TaskBodyType &&TaskBody) {
  TInvokeResult_T<TaskBodyType> Result;
  Result = Invoke(TaskBody);
  return TTask<TInvokeResult_T<TaskBodyType>>(Result);
}
// Launch(const TCHAR *DebugName, TaskBodyType &&TaskBody,
//        ETaskPriority         Priority         = ETaskPriority::Normal,
//        EExtendedTaskPriority ExtendedPriority = EExtendedTaskPriority::None,
//        ETaskFlags            Flags            = ETaskFlags::None) {
//     using FResult = TInvokeResult_T<TaskBodyType>;
//     TTask<FResult> Task;
//     Task.Launch(DebugName, Forward<TaskBodyType>(TaskBody), Priority,
//     ExtendedPriority, Flags); return Task;
// }
} // namespace TE::Tasks