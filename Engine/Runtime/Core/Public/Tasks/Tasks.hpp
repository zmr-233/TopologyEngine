/******************************************************
 * @file Tasks/Tasks.hpp
 * @brief
 *****************************************************/

#pragma once

#include "TypeUtils/CoreType.hpp"

namespace TE::Tasks {
namespace Private {
class FTaskHandle {
  public:
    bool IsValid() const { return false; }

    // 检查任务是否已经完成
    bool IsCompleted() const { return false; }
};
} // namespace Private
template <typename ResultType> class TTask {
  public:
    ResultType Result;
    TTask(ResultType Result) : Result(Result) {}
};

template <typename TaskBodyType>
TTask<TInvokeResult_T<TaskBodyType>> Launch(String DebugName, TaskBodyType &&TaskBody) {
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