/****************************************
 * FILE: Engine/Source/HAL/RunnableThread.cpp
 ****************************************/

#include "FRunnableThread.hpp"
#include "Log.hpp"

FRunnableThread::FRunnableThread()
    : Runnable(nullptr), ThreadID(0), bRunning(false) {
}

FRunnableThread::~FRunnableThread() {
    Kill(true);  // 确保线程已经退出
}

// 创建线程并运行
// 等价于 UE 的 CreateInternal() + PostCreate()
bool FRunnableThread::Create(FRunnable* InRunnable, const std::string& InThreadName) {
    if (!InRunnable)
        return false;
    Runnable   = InRunnable;
    ThreadName = InThreadName;

    // 创建线程
    bRunning     = true;
    WorkerThread = std::thread(&FRunnableThread::RunThread, this);

    // 如果需要在此等待 Init() 完成，可加一些事件机制
    return true;
}

// 请求停止线程
bool FRunnableThread::Kill(bool bShouldWait) {
    if (bRunning.exchange(false)) {
        // 通知 Runnable
        if (Runnable)
            Runnable->Stop();
        // 如果要等它退出，就 join
        if (bShouldWait && WorkerThread.joinable())
            WorkerThread.join();
    }
    return true;
}

// 等待线程结束
void FRunnableThread::WaitForCompletion() {
    if (bRunning && WorkerThread.joinable())
        WorkerThread.join();
    bRunning = false;
}

// 真正的线程函数
void FRunnableThread::RunThread() {
    // 记录一下这个线程的 ID
    ThreadID = std::hash<std::thread::id>()(std::this_thread::get_id());

    // LOG("[RunThread] Started: {}, ID: {}", ThreadName, ThreadID);
    if (!Runnable)
        return;
    // 初始化
    if (Runnable->Init()) {
        // LOG("[RunThread] Initialized: {}, ID: {}", ThreadName, ThreadID);
        // 运行
        Runnable->Run();
    }
    Runnable->Exit();
    // LOG("[RunThread] Exited: {}, ID: {}", ThreadName, ThreadID);
    // bRunning不在此处管理
}