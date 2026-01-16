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
#include "thread/ThreadPool.h"

namespace opentpg {
namespace core {
namespace thread {

ThreadPool::ThreadPool(size_t numThreads)
    : stop_(false)
{
    for (size_t i = 0; i < numThreads; ++i) {
        threads_.emplace_back(&ThreadPool::WorkerThread, this);
    }
}

ThreadPool::~ThreadPool()
{
    Shutdown();
}

void ThreadPool::WorkerThread()
{
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

            if (stop_ && tasks_.empty()) {
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        task();
    }
}

void ThreadPool::Shutdown()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_ = true;
    }

    condition_.notify_all();

    for (std::thread& worker : threads_) {
        worker.join();
    }

    threads_.clear();
}

size_t ThreadPool::GetQueueSize() const
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    return tasks_.size();
}

} // namespace thread
} // namespace core
} // namespace opentpg
