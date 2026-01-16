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
#include "memory/MemoryPool.h"

#include <cstring>
#include <memory>
#include <stdexcept>

namespace opentpg {
namespace core {
namespace memory {

FixedMemoryPool::FixedMemoryPool(size_t objectSize, size_t initialCapacity)
    : objectSize_(objectSize)
{
    AllocateNewBlock(initialCapacity);
}

FixedMemoryPool::~FixedMemoryPool()
{
    for (auto& block : blocks_) {
        delete[] block.data;
    }
}

void* FixedMemoryPool::Allocate(size_t size)
{
    if (size != objectSize_) {
        throw std::invalid_argument("Size mismatch for fixed memory pool");
    }

    if (!freeList_.empty()) {
        void* ptr = freeList_.back();
        freeList_.pop_back();
        return ptr;
    }

    auto& lastBlock = blocks_.back();
    if (lastBlock.used + objectSize_ > lastBlock.capacity) {
        AllocateNewBlock(lastBlock.capacity * 2);
    }

    auto& block = blocks_.back();
    void* ptr = block.data + block.used;
    block.used += objectSize_;
    return ptr;
}

void FixedMemoryPool::Deallocate(void* ptr)
{
    if (ptr) {
        freeList_.push_back(ptr);
    }
}

void FixedMemoryPool::Reset()
{
    for (auto& block : blocks_) {
        block.used = 0;
    }
    freeList_.clear();
}

size_t FixedMemoryPool::GetTotalSize() const
{
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block.capacity;
    }
    return total;
}

size_t FixedMemoryPool::GetUsedSize() const
{
    size_t used = 0;
    for (const auto& block : blocks_) {
        used += block.used;
    }
    return used;
}

void FixedMemoryPool::AllocateNewBlock(size_t capacity)
{
    Block block;
    block.data = new char[capacity];
    block.capacity = capacity;
    block.used = 0;
    blocks_.push_back(block);
}

VariableMemoryPool::VariableMemoryPool(size_t baseSize)
    : baseSize_(baseSize)
{
}

void* VariableMemoryPool::Allocate(size_t size)
{
    return FindOrCreatePool(size)->Allocate(size);
}

void VariableMemoryPool::Deallocate(void* ptr)
{
    if (!ptr) return;

    for (auto& pool : pools_) {
        if (pool) {
            pool->Deallocate(ptr);
            return;
        }
    }
}

void VariableMemoryPool::Reset()
{
    for (auto& pool : pools_) {
        if (pool) {
            pool->Reset();
        }
    }
}

size_t VariableMemoryPool::GetTotalSize() const
{
    size_t total = 0;
    for (const auto& pool : pools_) {
        if (pool) {
            total += pool->GetTotalSize();
        }
    }
    return total;
}

size_t VariableMemoryPool::GetUsedSize() const
{
    size_t used = 0;
    for (const auto& pool : pools_) {
        if (pool) {
            used += pool->GetUsedSize();
        }
    }
    return used;
}

FixedMemoryPool* VariableMemoryPool::FindOrCreatePool(size_t size)
{
    size_t poolIndex = (size + baseSize_ - 1) / baseSize_ - 1;

    if (poolIndex >= pools_.size()) {
        pools_.resize(poolIndex + 1);
    }

    if (!pools_[poolIndex]) {
        size_t poolSize = (poolIndex + 1) * baseSize_;
        pools_[poolIndex] = std::make_unique<FixedMemoryPool>(poolSize);
    }

    return pools_[poolIndex].get();
}

} // namespace memory
} // namespace core
} // namespace opentpg
