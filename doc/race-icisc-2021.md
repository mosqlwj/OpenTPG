# ICISC 竞赛

本项目已经连续 2 届承担参与 ICISC 竞赛出题，后续也将继续在 TPG 领域继续发力。

<a name="race-interface"></a>

## 系统接口定义

<a name="race-system-interface"></a>

### 软件目录结构

| 目录 | 用途 |
|--- |--- |
| .plan | 愿景、计划、想法等 |
| .convention | 公约、文件头 License 模板 |
| doc | 文档材料 |
| opentpg/src | 一个简单的用于生成 TestCube 的代码框架 |
| opentpg/bench | 随工程携带的几个样例网表文件及其配置 |
| opentpg/thirdparty | 所有依赖的第三方库，当前只支持 header-only 的 C++ 库 |

### 核心类及其作用

| 文件名 | 作用 |
|---    |---   |
| ATPGDriver.*      | 定义了一个用于控制所有 cube 生成的流程的接口 |
| CubeGenerator.*   | 定义了一个用于控制单个 cube 生成的流程 |
| CubeHandler.*     | 定义了一个用于 cube 对象处理器 |
| common.*          | 定义了一些基础类型、数据结构及相关函数 |
| errors.*          | 定义了错误处理相关的基础类型及数据结构 |
| Fault.*           | 定义了 fault 对象的内存表示 |
| Faultlist.*       | 存储和管理 fault |
| Gate.*            | 定义了 Gate 对象，包含 Gate 的类型名称等基本信息 |
| main.cpp          | 程序入口 |
| Netlist.*         | 存储和管理网表，支持网表加载、Scan chain 插入等能力 |
| Params.*          | 参数定义和解析 |
| printers.*        | 定义了一些对象的输出方式 |
| ScanChain.*       | 定义了 Scan chain 数据结构 |
| TestCube.*        | 定义了 cube 对象的内存表示，是个典型的数据类 |

<a name="race-system-interface"></a>

### 命令行接口

参见 [README.md](https://gitee.com/openeda/OpenTPG/blob/race-icisc-2021/README.md#system-interface)

<a name="race-code-interface"></a>

### 代码层接口

### Cube 生成器：CubeGenerator

CubeGenerator 定义了为指定的 Fault 对象生成 TestCube 接口。

##### 接口定义

参见文件 `CubeGenerator.h`

```c++
class CubeGenerator {
public:
    virtual ~CubeGenerator() = default;
    virtual FaultStatus Generate(const Fault* fault, TestCube*& cube) = 0;
};
```

`CubeGenerator` 是参赛的队伍必须实现的接口类。该接口只有一个纯虚函数 `Generate`。 该函数输入一个 Fault 对象，需要回 传一个 TestCube 对象。并返回一个 FaultStatus
类型的值，用于表示对该 fault 的识别结果。 TestCube 和 Fault 的定义，可参见对应的头文件。

##### 自定义实现

直接修改 CubeGenerator.cpp 中 `CreateCubeGenerator` 函数的实现，以便启用你自己的 CubeGenerator。

### ATPG 驱动：ATPGDriver

ATPGDriver 定义了为 Faultlist 中缓存的所有 Fault 生成 TestCube 的流程。

##### 接口定义

参见文件 `ATPGDriver.h`

```c++
class ATPGDriver {
public:
    //! 析构函数
    virtual ~ATPGDriver() = default;

    //! 配置网表
    virtual void SetupNetlist(Netlist* n) = 0;

    //! 配置 Fault 表
    virtual void SetupFaultlist(Faultlist* f) = 0;

    //! 指定 Cube 输出目标
    virtual void SetupCubeOutput(CubeHandler* cubeOutput) = 0;

    //! 生成 cube 之前的准备工作
    virtual int Prepare() = 0;

    //! 执行生成 cube 流程
    virtual void Execute() = 0;

    //! 清理内部资源，使得整个驱动回归初始状态
    virtual void Cleanup() = 0;
};
```

`ATPGDriver` 是参赛队伍可选实现的。如果您计划采用自己的定制实现，请实现 ATPGDriver 接口。

##### 自定义实现

直接修改 ATPGDriver.cpp 中 `CreateATPGDriver` 函数的实现，以便启用你自己的 ATPGDriver。

### Context

Context 顾名思义，就是执行 ATPG 计算时的上下文。Context 对象会在 CreateATPGDriver 和 CreateCubeGenerator 函数里面作为输入参数。如下所示：

```c++
extern ATPGDriver* CreateATPGDriver(void* context);

extern CubeGenerator* CreateCubeGenerator(void* context);
```

从上面的函数可以看出这两个函数的 context 参数都是 `void*` 类型。 虽然本项目已经提供了一个 ContextDefault 的 struct，但是实际上， 这并非是强制性约束，各个参赛队伍如果有特殊诉求， 完全可以定制自己的
Context。Context 的具体用法，可以参考 main 函 数（ 定义在 main.cpp）的代码。

#### Cube 对象：TestCube

TestCube 对象承载了 CubeGenerator 计算出来的所有的 PI 类型和 Scan DFF 类型的 Gate 的值。 其内部数据结构由两层嵌套的 std::vector 组成。 外层 vector 为 Cycle
的列表，内层 vector 该 Cycle 的关键 Gate 的值。 Gate 的值**必须**严格按照 GateId 的顺序存储。其内部主要成员如下：

```c++
std::vector<std::vector<LogicVal>> logicValue;
```

其，实际存储结构示意如下：

```text
        +----+----+----+----+----+----+----+----+----+----+
Cycle-0 | PI | PI | PI | PI | ...| PI | DFF| DFF| ...| DFF|
        +----+----+----+----+----+----+----+----+----+----+
Cycle-1 | PI | PI | PI | PI | ...| PI |
        +----+----+----+----+----+----+
Cycle-2 | PI | PI | PI | PI | ...| PI |
        +----+----+----+----+----+----+
```

需要特别注意，只有首个 Cycle 里面里面是需要填写 Scan DFF 的 Gate 的值的。后续的 Cycle 是不可以填写的。

TestCube 的输出一般不需要关心，如果有必要可以调用 CubePrinter 类的 PrintCubes2File 接口来输出。

### 文件接口

#### *.cube 文件

一个 cube 文件由一个或者多个 `TestCube Block`组成。而每个 `TestCube Block` 由 `TestCube Header` 和 `TestCube Cycle` 组成。 下面为一个 cube 文件的样例：

```text
$: 0
01101x110x
1x1x100
$: 1
0x0x10xxx1
001101x
$: 2
0xx010x111
0x01100
$: 3
1x1x000x11
0xxx000
```

上述样例中，定义了 2 个 cube。每个 cube 都是以 `$:` 开头的。`$:` 之后是该 cube 对应 Fault 的 Id。之后的每一行就对应一个 Cycle 的 每个 Gate 的值。

a name="race-rating"></a>

## 评分

* 评分细则，已经公布在 icisc 官网 [https://eda.icisc.cn/download/index?type=2](https://eda.icisc.cn/download/index?type=2)

* 赛题名为：`赛题一：海思-时序逻辑的高性能ATPG技术`

<a name="race-faq"></a>

## FAQ


