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
#ifndef OPENTPG_CORE_MEMORY_OBJECTPOOL_H
#define OPENTPG_CORE_MEMORY_OBJECTPOOL_H

#include "memory/MemoryPool.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace opentpg {
namespace core {
namespace memory {

/**
 * @brief 对象池（独占所有权）
 *
 * 类型安全的对象池，返回 std::unique_ptr，自动管理对象生命周期。
 * 对象析构时自动将内存返回池中，无需手动释放。
 *
 * 设计模式：
 * - RAII (资源获取即初始化): 智能指针确保资源释放
 * - 自定义删除器 (Custom Deleter): 使用 Deleter 将释放逻辑委托给对象池
 *
 * 使用示例：
 * ```cpp
 * ObjectPool<Gate> gatePool(4096);
 *
 * // 创建对象（unique_ptr 自动管理生命周期）
 * auto gate = gatePool.Create(gateId, gateType);
 *
 * // 使用对象
 * gate->Compute();
 *
 * // 离开作用域时自动释放回池
 * ```
 *
 * 性能优势：
 * - 类型安全：编译期类型检查
 * - 自动清理：无需手动调用 Return()
 * - 异常安全：即使异常发生也能正确释放
 */
template<typename T>
class ObjectPool {
public:
    explicit ObjectPool(size_t initialCapacity = 1024)
        : pool_(sizeof(T), initialCapacity)
    {
    }

    ~ObjectPool() = default;

    template<typename... Args>
    std::unique_ptr<T> Create(Args&&... args)
    {
        void* ptr = pool_.Allocate(sizeof(T));
        T* object = new (ptr) T(std::forward<Args>(args)...);
        return std::unique_ptr<T>(object, Deleter(this));
    }

    void Return(T* object)
    {
        if (object) {
            object->~T();
            pool_.Deallocate(object);
        }
    }

    void Reset()
    {
        pool_.Reset();
    }

    size_t GetTotalSize() const
    {
        return pool_.GetTotalSize() / sizeof(T);
    }

    size_t GetUsedSize() const
    {
        return pool_.GetUsedSize() / sizeof(T);
    }

private:
    /**
     * @brief 自定义删除器
     *
     * 将 unique_ptr 的删除逻辑委托给对象池的 Return() 方法。
     * 当 unique_ptr 被销毁时，自动调用此删除器。
     */
    struct Deleter {
        ObjectPool* pool;
        explicit Deleter(ObjectPool* p) : pool(p) {}

        void operator()(T* object) const
        {
            pool->Return(object);
        }
    };

    FixedMemoryPool pool_;
};

/**
 * @brief 共享对象池（共享所有权）
 *
 * 类型安全的对象池，返回 std::shared_ptr，支持多个所有者共享同一对象。
 * 对象在最后一个 shared_ptr 被销毁时自动将内存返回池中。
 *
 * 设计模式：
 * - 引用计数 (Reference Counting): 使用 shared_ptr 自动跟踪所有者数量
 * - 自定义删除器 (Custom Deleter): 将释放逻辑委托给对象池
 *
 * 使用示例：
 * ```cpp
 * SharedObjectPool<Netlist> netlistPool(512);
 *
 * // 创建共享对象
 * auto netlist1 = netlistPool.Create(netlistFile);
 * auto netlist2 = netlist1;  // 共享同一对象
 *
 * // 使用对象（netlist1 和 netlist2 指向同一 Netlist）
 * netlist1->LoadGates();
 * netlist2->GetGateCount();
 *
 * // 所有引用都释放时自动返回池
 * ```
 *
 * 使用场景：
 * - 共享只读数据（如 Netlist、Config）
 * - 缓存计算结果（如 SCOAP 度量）
 * - 多线程共享（避免重复计算）
 */
template<typename T>
class SharedObjectPool {
public:
    explicit SharedObjectPool(size_t initialCapacity = 1024)
        : pool_(initialCapacity)
    {
    }

    template<typename... Args>
    std::shared_ptr<T> Create(Args&&... args)
    {
        void* ptr = pool_.Allocate(sizeof(T));
        T* object = new (ptr) T(std::forward<Args>(args)...);
        return std::shared_ptr<T>(object, Deleter(&pool_));
    }

    void Reset()
    {
        pool_.Reset();
    }

private:
    /**
     * @brief 共享对象的删除器
     *
     * 注意：这里使用指针而非引用，因为 shared_ptr 的删除器是按值传递。
     * Deleter 是函数对象（可调用），所以可以存储池的指针。
     */
    struct Deleter {
        FixedMemoryPool* pool;
        explicit Deleter(FixedMemoryPool* p) : pool(p) {}

        void operator()(T* object) const
        {
            if (object) {
                object->~T();
                pool->Deallocate(object);
            }
        }
    };

    FixedMemoryPool pool_;
};

} // namespace memory
} // namespace core
} // namespace opentpg

#endif //OPENTPG_CORE_MEMORY_OBJECTPOOL_H
