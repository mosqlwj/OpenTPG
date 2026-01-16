# 代码注释说明总结

## 概述
为核心基础设施代码添加了详细的中文注释，包括设计模式、使用场景、性能考虑等内容。

## 已添加注释的文件

### 1. PluginManager.h/cpp
**位置**: `opentpg/src/core/plugin/PluginManager.h`

**注释内容**：
- `IPlugin` 接口基类：接口隔离模式，定义插件生命周期
- `PluginFactory`: 工厂方法模式，包装插件创建函数
- `PluginRegistry`: 单例注册表，管理插件注册
- `PluginManager`: 门面模式，统一管理多种插件类型

**关键设计模式**：
- 接口隔离 (Interface Segregation)
- 工厂方法 (Factory Method)
- 单例 (Singleton)
- 门面 (Facade)

---

### 2. MemoryPool.h/cpp
**位置**: `opentpg/src/core/memory/MemoryPool.h`

**注释内容**：
- `MemoryPool`: 策略模式基类，定义内存池接口
- `FixedMemoryPool`: 对象池模式，固定大小内存分配
- `VariableMemoryPool`: 分层池模式，按大小管理的多个固定池

**关键设计模式**：
- 策略 (Strategy Pattern)
- 对象池 (Object Pool Pattern)
- 分层池 (Hierarchical Pools)

**性能优化说明**：
- Block 扩容策略：每次容量翻倍
- 内存复用：freeList_ 缓存已释放的内存块
- 分层池：VariableMemoryPool 按大小分配到合适的固定池

---

### 3. ObjectPool.h
**位置**: `opentpg/src/core/memory/ObjectPool.h`

**注释内容**：
- `ObjectPool<T>`: 类型安全的独占对象池，返回 unique_ptr
- `SharedObjectPool<T>`: 共享所有权对象池，返回 shared_ptr
- `Deleter`: 自定义删除器，智能指针与对象池桥接

**关键设计模式**：
- RAII (资源获取即初始化)
- 自定义删除器 (Custom Deleter)
- 引用计数 (Reference Counting) - 仅 SharedObjectPool

**使用示例**：
```cpp
// 独占对象池
ObjectPool<Gate> gatePool(4096);
auto gate = gatePool.Create(gateId, gateType);  // 自动管理生命周期

// 共享对象池
SharedObjectPool<Netlist> netlistPool(512);
auto netlist = netlistPool.Create(netlistFile);
auto netlist2 = netlist;  // 共享同一对象
```

---

### 4. ThreadPool.h/cpp
**位置**: `opentpg/src/core/thread/ThreadPool.h`

**注释内容**：
- `ThreadPool`: 生产者-消费者模式的线程池
- 任务提交：Submit() 方法，支持任意可调用对象和参数
- 任务队列：线程安全的 std::queue<std::function<void()>>
- 同步机制：mutex + condition_variable

**关键设计模式**：
- 生产者-消费者 (Producer-Consumer)
- 线程池 (Thread Pool)

**线程安全性说明**：
- 任务队列：使用 std::mutex 保护
- 停止标志：使用 std::atomic<bool>
- 条件变量：用于线程间通信

**使用示例**：
```cpp
ThreadPool pool(4);  // 4 个工作线程
auto future = pool.Submit([](int x, int y) {
    return ComputeHeavyTask(x, y);
}, 10, 20);
int result = future.get();  // 等待结果
```

---

### 5. ParallelEngine.h
**位置**: `opentpg/src/core/thread/ParallelEngine.h`

**注释内容**：
- `ParallelEngine`: 策略模式的并行算法封装
- `ForEach`: 并行遍历，每个元素独立处理
- `ForEachParallel`: 分块并行处理，减少任务调度开销
- `Reduce`: 并行归约（Map-Reduce 模式）

**关键设计模式**：
- 策略 (Strategy Pattern)
- 模板方法 (Template Method)

**性能考虑**：
- 任务粒度 vs 调度开销的权衡
- 分块大小：ForEachParallel 支持自定义 chunkSize
- 两阶段归约：Reduce 先并行计算，再顺序合并

**使用示例**：
```cpp
ThreadPool pool(4);
ParallelEngine engine(pool);

// 1. 并行遍历
engine.ForEach(gates.begin(), gates.end(), [](Gate* gate) {
    gate->Evaluate();
});

// 2. 分块并行处理（每 100 个元素一块）
engine.ForEachParallel(items.begin(), items.end(), [](Item& item) {
    ProcessItem(item);
}, 100);

// 3. 并行归约
int sum = engine.Reduce(numbers.begin(), numbers.end(),
    [](int x) { return x; },
    [](int a, int b) { return a + b; });
```

---

### 6. ATPGContext.h/cpp
**位置**: `opentpg/src/core/context/ATPGContext.h`

**注释内容**：
- `ATPGContext`: ATPG 过程的全局上下文管理器
- 资源管理：网表、故障列表、仿真上下文
- 配置管理：字符串配置（算法名称等）和整数配置（最大回溯次数等）
- 执行环境：ThreadPool 和 ParallelEngine

**关键设计模式**：
- 上下文对象 (Context Object)
- 依赖注入 (Dependency Injection)

**生命周期管理**：
- Reset(): 清理所有资源，支持重复使用
- 智能指针：资源使用 shared_ptr，支持多引用

**使用示例**：
```cpp
// 创建上下文
ATPGContext context;

// 配置数据
context.SetNetlist(netlist);
context.SetFaultList(faultList);

// 设置算法参数
context.SetConfig("algorithm", "PODEM");
context.SetConfigInt("max_backtracks", 100);

// 在算法中使用
auto& pool = context.GetThreadPool();
pool.Submit([]() {
    // ATPG 算法逻辑
});
```

---

## 代码风格

所有注释遵循以下规范：
1. 使用中文描述，便于国内开发者理解
2. 包含设计模式名称（括号内英文，便于查阅）
3. 说明使用场景和性能考虑
4. 提供代码示例
5. 说明线程安全性和生命周期管理

## 验证

编译验证：
```bash
Build opentpg-0.0.1 success
```

测试验证：
```bash
[==========] 19 tests from 4 test suites ran. (0 ms total)
[ PASSED  ] 19 tests.
```

## 下一步

阶段 1 已完成（基础设施重构）。
根据重构规划，下一步应该是：
- 阶段 2：数据模型重构
  - 使用 shared_ptr 重构 Gate/Netlist
  - 使用智能指针重构 Fault/FaultList
  - 重构 TestCube/TestPattern
  - 实现值系统（LogicValue, SPLITValue）
  - 添加 GPU 友好数据布局（GPULayout）
