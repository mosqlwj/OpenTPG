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
#ifndef OPENTPG_CORE_CONTEXT_ATPGCONTEXT_H
#define OPENTPG_CORE_CONTEXT_ATPGCONTEXT_H

#include "core/thread/ThreadPool.h"
#include "core/thread/ParallelEngine.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace opentpg {

namespace model::netlist {
class Netlist;
}

namespace model::fault {
class FaultList;
}

namespace simulation {
class SimulationContext;
}

namespace core {
namespace context {

/**
 * @brief ATPG 执行上下文
 *
 * ATPG（自动测试模式生成）过程的全局上下文管理器。
 * 集中管理网表、故障列表、仿真上下文、配置、线程池等资源。
 * 为 ATPG 算法和仿真器提供统一的接口访问所需资源。
 *
 * 设计模式：
 * - 上下文对象 (Context Object): 集中管理相关资源
 * - 依赖注入 (Dependency Injection): 通过 setter 注入依赖，降低耦合
 * - 资源共享 (Resource Sharing): 多个算法/组件共享同一上下文
 *
 * 管理的资源：
 * 1. 数据模型：
 *    - Netlist: 电路网表（门、连接关系）
 *    - FaultList: 故障列表（待测故障集）
 * 2. 执行环境：
 *    - SimulationContext: 仿真状态管理
 *    - ThreadPool: 并行执行任务的工作线程池
 *    - ParallelEngine: 高级并行算法封装
 * 3. 配置管理：
 *    - 字符串配置: 算法名称、文件路径等
 *    - 整数配置: 最大回溯次数、并行度等
 *
 * 使用示例：
 * ```cpp
 * // 创建上下文
 * ATPGContext context;
 *
 * // 配置数据
 * context.SetNetlist(netlist);
 * context.SetFaultList(faultList);
 *
 * // 设置算法参数
 * context.SetConfig("algorithm", "PODEM");
 * context.SetConfigInt("max_backtracks", 100);
 *
 * // 在算法中使用
 * auto& pool = context.GetThreadPool();
 * pool.Submit([]() {
 *     // ATPG 算法逻辑
 * });
 * ```
 *
 * 生命周期管理：
 * - Reset(): 清理所有资源，支持重复使用上下文
 * - 智能指针：资源使用 shared_ptr，支持多引用
 */
class ATPGContext {
public:
    ATPGContext();
    ~ATPGContext() = default;

    void SetNetlist(std::shared_ptr<model::netlist::Netlist> netlist);
    void SetFaultList(std::shared_ptr<model::fault::FaultList> faultList);
    void SetSimulationContext(std::shared_ptr<simulation::SimulationContext> simContext);
    void SetConfig(const std::string& key, const std::string& value);
    void SetConfigInt(const std::string& key, int value);

    std::shared_ptr<model::netlist::Netlist> GetNetlist() const { return netlist_; }
    std::shared_ptr<model::fault::FaultList> GetFaultList() const { return faultList_; }
    std::shared_ptr<simulation::SimulationContext> GetSimulationContext() const { return simContext_; }

    std::string GetConfig(const std::string& key, const std::string& defaultValue = "") const;
    int GetConfigInt(const std::string& key, int defaultValue = 0) const;

    thread::ThreadPool& GetThreadPool() { return *threadPool_; }
    thread::ParallelEngine& GetParallelEngine() { return *parallelEngine_; }

    void Reset();

private:
    std::shared_ptr<model::netlist::Netlist> netlist_;
    std::shared_ptr<model::fault::FaultList> faultList_;
    std::shared_ptr<simulation::SimulationContext> simContext_;

    std::unique_ptr<thread::ThreadPool> threadPool_;
    std::unique_ptr<thread::ParallelEngine> parallelEngine_;

    std::unordered_map<std::string, std::string> configs_;
    std::unordered_map<std::string, int> configsInt_;
};

} // namespace context
} // namespace core
} // namespace opentpg

#endif //OPENTPG_CORE_CONTEXT_ATPGCONTEXT_H
