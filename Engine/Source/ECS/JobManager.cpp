/****************************************
 * FILE: Engine/Source/ECS/JobManager.cpp
 ****************************************/

// #include "../../Include/ECS/JobManager.hpp"

// #include <memory>
// #include <typeindex>
// #include <unordered_map>

// #include "../../Include/ECS/ECSCore.hpp"
// #include "../../Include/ECS/JobManagerProxy.hpp"
// #include "../../Include/ECS/JobType.hpp"
// #include "../../Include/ECS/System.hpp"
// #include "../../Include/ECS/SystemManager.hpp"
// #include "../../Include/Utils/DepGraph.hpp"
// #include "../../Include/Utils/Log.hpp"
// #include "../../Include/Utils/ThreadPool.hpp"

// explicit JobManager::JobManager(size_t threadCount)
//     : mPool(threadCount) {}
// /**
//  * @brief 提交一个不带任何依赖的Job
//  */
// void JobManager::submitJob(std::shared_ptr<Job> job) {
//     {
//         std::unique_lock<std::mutex> lock(mMutex);
//         if (job->dependencyCount.load() == 0) {
//             // 立即可执行
//             scheduleJob(job);
//             /**
//              * @bug 不应该立刻执行，不然无法保证依赖关系
//              */
//         } else {
//             // 还要等待依赖完成
//             mPendingJobs.push_back(job);
//         }
//     }
// }

// /**
//  * @brief 对两个Job建立依赖关系： childJob 依赖 parentJob
//  *
//  * @bug 如果parent自己已经被submitJob给提交了，那么这里的依赖关系就无效了
//  *      但是在fetch_add的时候并没有考虑parent是否已经被提交，或者正在做了
//  */
// void JobManager::addDependency(std::shared_ptr<Job> parent, std::shared_ptr<Job> child) {
//     child->dependencyCount.fetch_add(1);
//     // 当parent完成后，要让child计数-1
//     // 这里做一个简单的回调：保留 child 的弱引用
//     auto childWeak        = std::weak_ptr<Job>(child);
//     auto parentWorkOrigin = parent->work;  // 原来的工作
//     // 给 parent->work 包一层包装：执行原本的任务后，再做依赖减1
//     parent->work = [this, parentWorkOrigin, childWeak]() {
//         // 先执行原始工作
//         if (parentWorkOrigin) {
//             parentWorkOrigin();
//         }
//         // 依赖-1
//         if (auto c = childWeak.lock()) {
//             int oldValue = c->dependencyCount.fetch_sub(1) - 1;
//             if (oldValue == 0) {
//                 // 说明child的依赖已经全部完成，现在可以执行child
//                 // 这里再提交job
//                 std::unique_lock<std::mutex> lock(mMutex);
//                 scheduleJob(c);
//             }
//         }
//     };
// }

// /**
//  * @brief 等待所有Job结束（简单实现）
//  */
// void JobManager::waitAll() {
//     // 一个最简方案：我们可以自己跟踪所有提交的 Job 的 future 或 promise
//     // 这里做个简化：sleep直到pendingJobs为空 并没有任务在队列
//     // 生产级别可做更精细的同步

//     bool finished = false;
//     while (!finished) {
//         {
//             std::unique_lock<std::mutex> lock(mMutex);
//             finished = mPendingJobs.empty();
//         }
//         // 也要确保线程池的队列里没任务了
//         // 这里再sleep一点点
//         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//     }
//     // 还要保证正在执行的任务也执行完，这里可以再sleep一下
//     std::this_thread::sleep_for(std::chrono::milliseconds(5));
// }

// void JobManager::scheduleJob(std::shared_ptr<Job> job) {
//     // 提交到线程池
//     mPool.enqueue([job]() {
//         // 执行job->work
//         if (job->work) {
//             job->work();
//         }
//     });
// }