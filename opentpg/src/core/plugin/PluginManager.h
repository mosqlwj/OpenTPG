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
#ifndef OPENTPG_CORE_PLUGIN_PLUGINMANAGER_H
#define OPENTPG_CORE_PLUGIN_PLUGINMANAGER_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace opentpg {
namespace core {
namespace plugin {

/**
 * @brief 插件接口基类
 *
 * 所有 ATPG 算法、仿真器、后端等插件必须继承此类。
 * 定义了插件的基本生命周期管理方法：
 * 1. 初始化: Initialize() - 插件加载时调用
 * 2. 运行: 插件的实际功能由派生类实现
 * 3. 清理: Shutdown() - 插件卸载时调用
 *
 * 设计模式：接口隔离 (Interface Segregation)
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;

    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
};

/**
 * @brief 插件工厂函数类型
 *
 * 使用 std::function 包装工厂函数，支持运行时动态创建插件实例。
 * 工厂函数接收无参数，返回 std::unique_ptr<PluginType>
 *
 * 设计模式：工厂方法 (Factory Method)
 */
template<typename T>
using PluginFactory = std::function<std::unique_ptr<T>()>;

/**
 * @brief 插件注册表
 *
 * 模板化的插件注册中心，使用单例模式管理每种插件类型。
 * 每种插件类型（ATPG 算法、仿真器、后端）都有独立的注册表。
 *
 * 设计模式：
 * - 单例 (Singleton): 每种插件类型只有一个注册表实例
 * - 注册表 (Registry): 维护名称到工厂函数的映射
 *
 * 使用示例：
 * ```cpp
 * // 注册插件
 * PluginRegistry<ATPGAlgorithm>::GetInstance().Register("PODEM",
 *     []() { return std::make_unique<PODEMAlgorithm>(); });
 *
 * // 创建插件
 * auto algorithm = PluginRegistry<ATPGAlgorithm>::GetInstance().Create("PODEM");
 * ```
 */
template<typename PluginType>
class PluginRegistry {
public:
    static PluginRegistry<PluginType>& GetInstance()
    {
        static PluginRegistry<PluginType> instance;
        return instance;
    }

    void Register(const std::string& name, PluginFactory<PluginType> factory)
    {
        factories_[name] = std::move(factory);
    }

    std::unique_ptr<PluginType> Create(const std::string& name) const
    {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second();
        }
        return nullptr;
    }

    std::vector<std::string> ListPlugins() const
    {
        std::vector<std::string> names;
        names.reserve(factories_.size());
        for (const auto& pair : factories_) {
            names.push_back(pair.first);
        }
        return names;
    }

private:
    std::unordered_map<std::string, PluginFactory<PluginType>> factories_;
};

/**
 * @brief 插件管理器
 *
 * 全局插件管理单例，提供统一的插件注册和创建接口。
 * 内部通过 PluginRegistry 委托给具体的插件类型注册表。
 *
 * 设计模式：
 * - 门面 (Facade): 统一管理多种插件类型的注册表
 * - 单例 (Singleton): 全局唯一的插件管理入口
 *
 * 线程安全性：由于每个 PluginRegistry 是单例，需要注意并发注册问题。
 * 当前实现为简化版本，生产环境应考虑使用互斥锁保护注册操作。
 */
class PluginManager {
public:
    static PluginManager& GetInstance();

    template<typename PluginType>
    void RegisterPlugin(const std::string& name, PluginFactory<PluginType> factory)
    {
        PluginRegistry<PluginType>::GetInstance().Register(name, std::move(factory));
    }

    template<typename PluginType>
    std::unique_ptr<PluginType> CreatePlugin(const std::string& name) const
    {
        return PluginRegistry<PluginType>::GetInstance().Create(name);
    }

    template<typename PluginType>
    std::vector<std::string> ListPlugins() const
    {
        return PluginRegistry<PluginType>::GetInstance().ListPlugins();
    }

private:
    PluginManager() = default;
    ~PluginManager() = default;
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
};

} // namespace plugin
} // namespace core
} // namespace opentpg

#endif //OPENTPG_CORE_PLUGIN_PLUGINMANAGER_H
