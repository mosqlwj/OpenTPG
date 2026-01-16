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
#ifndef OPENTPG_CORE_THREAD_PARALLELENGINE_H
#define OPENTPG_CORE_THREAD_PARALLELENGINE_H

#include "thread/ThreadPool.h"

#include <algorithm>
#include <future>
#include <iterator>
#include <vector>

namespace opentpg {
namespace core {
namespace thread {

/**
 * @brief 并行执行引擎
 *
 * 基于 ThreadPool 提供高级并行算法模板，简化并行编程的复杂性。
 * 支持常见的并行模式：遍历、分块处理、归约等。
 *
 * 设计模式：
 * - 策略 (Strategy Pattern): 封装不同的并行算法
 * - 模板方法 (Template Method): 类型安全的并行操作
 *
 * 提供的算法：
 * 1. ForEach: 并行遍历，每个元素独立处理
 * 2. ForEachParallel: 分块并行处理，减少任务调度开销
 * 3. Reduce: 并行归约（如求和、最大值等）
 *
 * 使用示例：
 * ```cpp
 * ThreadPool pool(4);
 * ParallelEngine engine(pool);
 *
 * // 1. 并行遍历
 * engine.ForEach(gates.begin(), gates.end(), [](Gate* gate) {
 *     gate->Evaluate();
 * });
 *
 * // 2. 分块并行处理（每 100 个元素一块）
 * engine.ForEachParallel(items.begin(), items.end(), [](Item& item) {
 *     ProcessItem(item);
 * }, 100);
 *
 * // 3. 并行归约
 * int sum = engine.Reduce(numbers.begin(), numbers.end(),
 *     [](int x) { return x; },
 *     [](int a, int b) { return a + b; });
 * ```
 *
 * 性能考虑：
 * - 任务粒度：细粒度任务提高并行度，但增加调度开销
 * - 负载均衡：ThreadPool 自动分配任务到空闲线程
 * - 结果合并：Reduce 方法两阶段处理（并行计算 + 顺序合并）
 */
class ParallelEngine {
public:
    explicit ParallelEngine(ThreadPool& pool)
        : pool_(pool)
    {
    }

    ~ParallelEngine() = default;

    template<typename Iterator, typename Function>
    void ForEach(Iterator begin, Iterator end, Function&& func)
    {
        using ValueType = typename std::iterator_traits<Iterator>::value_type;
        std::vector<std::future<void>> futures;

        for (Iterator it = begin; it != end; ++it) {
            futures.push_back(pool_.Submit([func, value = *it]() mutable {
                func(value);
            }));
        }

        for (auto& future : futures) {
            future.wait();
        }
    }

    template<typename Iterator, typename Function>
    void ForEachParallel(Iterator begin, Iterator end, Function&& func, size_t chunkSize = 1)
    {
        std::vector<std::future<void>> futures;
        size_t total = std::distance(begin, end);

        for (size_t i = 0; i < total; i += chunkSize) {
            Iterator chunkBegin = begin;
            std::advance(chunkBegin, i);
            Iterator chunkEnd = chunkBegin;
            std::advance(chunkEnd, std::min(chunkSize, total - i));

            futures.push_back(pool_.Submit([func, chunkBegin, chunkEnd]() mutable {
                for (Iterator it = chunkBegin; it != chunkEnd; ++it) {
                    func(*it);
                }
            }));
        }

        for (auto& future : futures) {
            future.wait();
        }
    }

    template<typename Iterator, typename Function, typename ReduceFunc>
    auto Reduce(Iterator begin, Iterator end, Function&& func, ReduceFunc&& reduce)
        -> decltype(reduce(std::declval<typename std::iterator_traits<Iterator>::value_type>(),
                    std::declval<typename std::iterator_traits<Iterator>::value_type>()))
    {
        using ValueType = typename std::iterator_traits<Iterator>::value_type;
        using FuncResultType = decltype(func(std::declval<ValueType>()));

        std::vector<std::future<FuncResultType>> futures;

        for (Iterator it = begin; it != end; ++it) {
            futures.push_back(pool_.Submit([func, value = *it]() {
                return func(value);
            }));
        }

        std::vector<FuncResultType> results;
        for (auto& future : futures) {
            results.push_back(future.get());
        }

        FuncResultType accumulator = results[0];
        for (size_t i = 1; i < results.size(); ++i) {
            accumulator = reduce(accumulator, results[i]);
        }

        return accumulator;
    }

    ThreadPool& GetPool() { return pool_; }
    const ThreadPool& GetPool() const { return pool_; }

private:
    ThreadPool& pool_;
};

} // namespace thread
} // namespace core
} // namespace opentpg

#endif //OPENTPG_CORE_THREAD_PARALLELENGINE_H
