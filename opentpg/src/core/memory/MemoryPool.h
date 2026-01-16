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
#ifndef OPENTPG_CORE_MEMORY_MEMORYPOOL_H
#define OPENTPG_CORE_MEMORY_MEMORYPOOL_H

#include <cstddef>
#include <memory>
#include <vector>

namespace opentpg {
namespace core {
namespace memory {

/**
 * @brief 内存池基类
 *
 * 定义内存池的通用接口，提供内存分配、释放、重置等功能。
 * 使用内存池可以减少频繁的内存分配/释放开销，提高性能。
 *
 * 设计模式：
 * - 策略模式 (Strategy): 不同的池实现使用不同的分配策略
 *
 * 性能优势：
 * 1. 减少内存碎片
 * 2. 降低 new/delete 调用次数
 * 3. 提高缓存局部性
 */
class MemoryPool {
public:
    virtual ~MemoryPool() = default;

    virtual void* Allocate(size_t size) = 0;
    virtual void Deallocate(void* ptr) = 0;
    virtual void Reset() = 0;
    virtual size_t GetTotalSize() const = 0;
    virtual size_t GetUsedSize() const = 0;
};

/**
 * @brief 固定大小内存池
 *
 * 为相同大小的对象提供高效的内存分配机制。
 * 适用于需要频繁创建/销毁大量相同大小对象的场景（如 Gate、Fault 等）。
 *
 * 设计模式：
 * - 对象池 (Object Pool Pattern): 预分配固定大小的内存块
 *
 * 实现机制：
 * 1. 分配策略：
 *    - 首先从 freeList_ 复用已释放的内存
 *    - 如果没有可复用的，从当前 block 分配新内存
 *    - 如果当前 block 已满，分配新的 block（容量翻倍）
 * 2. 释放策略：
 *    - 将释放的内存块添加到 freeList_，供后续复用
 *    - 不立即释放回系统，减少系统调用
 *
 * 性能考虑：
 * - 初始容量: 1024，可根据实际对象数量调整
 * - Block 扩容: 每次容量翻倍，避免频繁重新分配
 * - 内存对齐: char* 数组确保对象在连续内存中
 *
 * 使用示例：
 * ```cpp
 * FixedMemoryPool gatePool(sizeof(Gate), 4096);
 * Gate* gate1 = static_cast<Gate*>(gatePool.Allocate(sizeof(Gate)));
 * new (gate1) Gate(...);  // 就地构造
 * gatePool.Deallocate(gate1);  // 析构但保留内存
 * ```
 */
class FixedMemoryPool : public MemoryPool {
public:
    explicit FixedMemoryPool(size_t objectSize, size_t initialCapacity = 1024);
    ~FixedMemoryPool() override;

    void* Allocate(size_t size) override;
    void Deallocate(void* ptr) override;
    void Reset() override;
    size_t GetTotalSize() const override;
    size_t GetUsedSize() const override;

private:
    struct Block {
        char* data;
        size_t capacity;
        size_t used;
    };

    std::vector<Block> blocks_;
    std::vector<void*> freeList_;
    size_t objectSize_;

    void AllocateNewBlock(size_t capacity);
};

/**
 * @brief 可变大小内存池
 *
 * 支持不同大小对象的内存分配，内部使用多个固定大小内存池。
 * 根据请求的内存大小，自动选择或创建合适的固定池。
 *
 * 设计模式：
 * - 分层池 (Hierarchical Pools): 按大小分层的多个固定池
 * - 延迟初始化 (Lazy Initialization): 按需创建固定池
 *
 * 分配策略：
 * 1. 根据请求大小计算池索引：poolIndex = (size + baseSize - 1) / baseSize - 1
 * 2. 每个池负责 baseSize 的倍数大小（如 baseSize=64，则处理 64, 128, 192...）
 * 3. 延迟创建：只有当请求特定大小时才创建对应池
 *
 * 使用场景：
 * - 不同类型对象的大小差异较大（如 Gate vs Fault vs TestCube）
 * - 无法预知所有对象大小的场景
 * - GPU 友好：固定大小的连续内存块更易传输到 GPU
 *
 * 参数选择：
 * - baseSize: 建议设为最常见对象的大小，减少池的数量
 * - 过小：增加池数量，内存开销大
 * - 过大：单个池浪费内存较多
 */
class VariableMemoryPool : public MemoryPool {
public:
    explicit VariableMemoryPool(size_t baseSize = 64);
    ~VariableMemoryPool() override = default;

    void* Allocate(size_t size) override;
    void Deallocate(void* ptr) override;
    void Reset() override;
    size_t GetTotalSize() const override;
    size_t GetUsedSize() const override;

private:
    std::vector<std::unique_ptr<FixedMemoryPool>> pools_;
    size_t baseSize_;

    FixedMemoryPool* FindOrCreatePool(size_t size);
};

} // namespace memory
} // namespace core
} // namespace opentpg

#endif //OPENTPG_CORE_MEMORY_MEMORYPOOL_H
