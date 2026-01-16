/**
 * Copyright (c) 2021 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#ifndef OPENTPG_CORE_THREAD_THREADPOOL_H
#define OPENTPG_CORE_THREAD_THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace opentpg {
namespace core {
namespace thread {

/**
 * @brief 线程池
 *
 * 生产者-消费者模式的工作线程池，支持异步任务提交和并行执行。
 * 默认使用硬件并发度创建工作线程，可根据需求调整。
 *
 * 设计模式：
 * - 生产者-消费者 (Producer-Consumer): 主线程提交任务，工作线程执行任务
 * - 线程池 (Thread Pool): 复用线程，避免频繁创建/销毁开销
 *
 * 实现机制：
 * 1. 任务提交：
 *    - Submit() 方法接受任意可调用对象和参数
 *    - 使用 packaged_task 包装任务，支持 future 获取结果
 *    - 线程安全：使用 mutex 保护任务队列
 *    - 通知机制：condition_variable 唤醒工作线程
 * 2. 任务执行：
 *    - 工作线程从队列中取出任务执行
 *    - 支持停止信号：优雅关闭时退出
 * 3. 优雅关闭：
 *    - Stop() 设置停止标志
 *    - 等待所有工作线程完成
 *
 * 使用示例：
 * ```cpp
 * ThreadPool pool(4);  // 4 个工作线程
 *
 * // 提交任务
 * auto future1 = pool.Submit([]() {
 *     return ComputeHeavyTask();
 * });
 *
 * auto future2 = pool.Submit([](int x, int y) {
 *     return ComputeWithArgs(x, y);
 * }, 10, 20);
 *
 * // 等待结果
 * int result1 = future1.get();
 * int result2 = future2.get();
 * ```
 *
 * 线程安全性：
 * - 任务队列：使用 mutex 保护
 * - 停止标志：使用 atomic
 * - 条件变量：用于线程同步
 */
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<typename F, typename... Args>
    auto Submit(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using ReturnType = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (stop_) {
                throw std::runtime_error("Cannot submit task to stopped ThreadPool");
            }
            tasks_.emplace([task]() { (*task)(); });
        }

        condition_.notify_one();
        return result;
    }

    void Shutdown();
    size_t GetQueueSize() const;
    size_t GetThreadCount() const { return threads_.size(); }

private:
    void WorkerThread();

    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
};

} // namespace thread
} // namespace core
} // namespace opentpg

#endif //OPENTPG_CORE_THREAD_THREADPOOL_H
