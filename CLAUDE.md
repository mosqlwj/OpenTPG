# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 提供在此代码库中工作的指导。

## 项目概述

OpenTPG 是一个用 C++11 编写的开源 ATPG（自动测试模式生成）工具集。主工具 `opentpg` 使用 SPLIT 电路模型（一种改进的组合电路 9 值模型）为数字电路测试生成测试立方。

## 构建命令

使用 `opentpg/` 目录中的构建脚本：

```bash
cd opentpg
./build.sh compile    # 构建项目（创建 build/，使用 cmake + make 编译）
./build.sh clean      # 清理构建产物和生成的文件（*.fault、*.gate、*.cube）
./build.sh format     # 使用 clang-format 格式化代码
./build.sh help       # 显示帮助
```

构建流程：
1. 创建 `opentpg/build/` 目录
2. 运行 cmake 配置
3. 使用 make 编译（4 个并行任务）
4. 将 `opentpg` 可执行文件移动到 `opentpg/` 目录

手动构建（替代方法）：
```bash
cd opentpg
mkdir build && cd build
cmake ../
make
```

### 构建测试

```bash
cd opentpg/build
cmake -DBUILD_TESTS=ON ../  # 确保启用测试
make                          # 构建 opentpg_test 可执行文件
```

测试可执行文件位于 `opentpg/build/opentpg_test`。

### 运行测试

```bash
cd opentpg/build
./opentpg_test                    # 运行所有测试
./opentpg_test --gtest_filter=TestGate.*  # 运行特定测试套件
./opentpg_test --gtest_list_tests        # 列出所有测试
```

或使用 ctest：
```bash
cd opentpg/build
ctest                  # 运行所有测试
ctest -R TestGate       # 运行匹配模式的测试
```

## 运行工具

```bash
./opentpg -n <网表文件> -c <配置文件> -f <故障输出> -u <立方输出>
```

示例：
```bash
./opentpg -n bench/s27.bench -c bench/s27.cfg -f bench/s27.fault -u bench/s27.cube
```

## 数据流

```
网表 (.bench) + 配置 (.cfg) → 故障列表 (.fault) → 测试立方 (.cube) → 状态列表
```

系统读取电路网表和配置，生成故障列表，然后为 ATPG 生成测试立方。

## 架构

### 核心模块 (`opentpg/src/`)

每个目录都是一个具有独立命名空间的模块：

- **`flowtpg/`** - 核心 ATPG 功能
  - `model/` - 数据模型：Netlist（电路表示）、Gate、Fault、ScanChain
  - `control/` - 控制逻辑：ATPGDriver（测试生成接口）、CubeGenerator
  - `context/` - 执行上下文管理
  - `errors/` - 错误处理

- **`shell/`** - 命令行接口和参数处理

- **`logger/`** - 日志系统

- **`utils/`** - 通用工具（字符串处理等）

### 关键接口

- **ATPGDriver** (`flowtpg/control/ATPGDriver.h`) - 测试生成驱动的抽象接口。实现必须：
  - `SetupNetlist()` - 配置网表
  - `SetupFaultlist()` - 配置故障列表
  - `SetupCubeOutput()` - 指定输出处理器
  - `Prepare()` - 生成前准备
  - `Execute()` - 运行立方生成
  - `Cleanup()` - 重置状态

- **CubeHandler** (`flowtpg/context/CubeHandler.h`) - 测试立方输出处理器的抽象接口
  - `Handle(TestCube* cube)` - 处理生成的测试立方（调用者负责传递所有权）

- **Netlist** (`flowtpg/model/Netlist.h`) - 表示数字电路，包含门、扫描链和 PI/PO 映射

## 代码规范

**严格限制仅使用 C++11** - 不允许使用更新的 C++ 标准。

### 命名约定
- **文件名**：与核心类名完全匹配（区分大小写），若无核心类则使用小写
- **类名**：PascalCase（如 `ATPGDriver`、`Netlist`）
- **函数名**：PascalCase（如 `LoadNetlist`、`SetupFaultlist`）
- **变量名**：camelCase（如 `numOfPI`、`name2Gate`）
- **枚举**：类型名 PascalCase，枚举项 UPPER_UNDERSCORE
- **命名空间**：`src/` 中的每个子目录都是一个具有独立命名空间的模块

### 代码风格
- 提交代码前使用 `build.sh format` 通过 clang-format 格式化代码
- 代码格式配置文件位于项目根目录 `.clang-format`
- 避免使用行内注释（`//` 和 `/* */`）
- 逻辑段落之间留空行
- Include 路径：使用相对于 `src/` 的路径（如 `#include "flowtpg/model/Netlist.h"`）
- Include 顺序：项目自研 → 开源库 → 标准库（稳定性从低到高）
- 文件后缀：`.h`（头文件）、`.cpp`（实现）、`.inc`（包含文件）

### 文件扩展名
- `*.bench` - 网表输入文件（ISCAS 格式）
- `*.cfg` - 网表配置文件（扫描链、时钟）
- `*.fault` - 导出的故障列表
- `*.cube` - 导出的测试立方列表

## 依赖项

- **Boost 1.76** - C++ 工具库
- **cmdline 3.2.1** - 命令行解析
- **Google Test 1.11.0** - 单元测试框架（测试时自动通过 FetchContent 下载）
- **CMake 3.12+** - 构建系统
- **gcc 7+** 支持 C++11

## 模块结构

项目使用 `MODULE` 和 `PROJECT` 文件存储模块元数据：
- `opentpg/MODULE` - 定义模块名称和版本
- `opentpg/PROJECT` - 定义项目名称
- 构建脚本通过向上遍历自动定位这些文件

## 测试

### 单元测试
测试目录位于 `opentpg/test/`，使用 Google Test 框架：

- **`test_main.cpp`** - 测试入口
- **`model/`** - Model 模块测试（TestGate、TestFault、TestTestCube）
- **`logger/`** - Logger 模块测试

测试配置通过 CMake 自动发现（`gtest_discover_tests`），无需手动注册。

### 基准电路
位于 `opentpg/bench/`：
- `s27.bench` / `s27.cfg` - 小型测试电路
- `s838.bench` / `s838.cfg` - 大型基准电路
