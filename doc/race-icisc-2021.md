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
| Simulator.h       | 定义了仿真器的接口 |
| Sim*              | 仿真器的简易实现，这部分代码并不是必须的，这里提供的仿真器主要目的是便于各位学生验证 cube 正确性。目前这个仿真器还只是一个原型，我们尽力做到做到，但由于投入有限还无法保证没有 bug 存在。各参赛队伍也无需对 simulator 相关代码进行优化。 |

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

上述样例中，定义了 4 个 cube。每个 cube 都是以 `$:` 开头的。`$:` 之后是该 cube 对应 Fault 的 Id。之后的每一行就对应一个 Cycle 的 每个 Gate 的值。

<a name="race-rating"></a>

## 如何自验证

本项目自带了一个简易的 simulator，可以通过 `-s` 来告诉程序在将 cube 写入文件之后顺便做一下仿真。启用自动仿真时，终端上输出信息可参考如下：

```txt
CUBE DETECT:0 YES
CUBE DETECT:1 YES
CUBE DETECT:2 YES
CUBE DETECT:3 YES
CUBE DETECT:4 YES
CUBE DETECT:5 NO
CUBE DETECT:6 YES
CUBE DETECT:7 NO
CUBE DETECT:8 NO
CUBE DETECT:9 YES
CUBE DETECT:10 YES
CUBE DETECT:11 YES
CUBE DETECT:12 YES
...
```

其中，`CUBE DETECT:xxx` 中 `xxx` 位置的数字代表 fault 的编号，而 `YES` 和 `NO` 表示这个 fault 是否被您生成的 cube 识别到。

主要注意，目前实现的简易 simulator 实现相当简单，性能自然不高，对于大网表，仿真的时间会变得很长，
并不适合一直开着 -s 来跑。由于 simulator 不是性能评分的一部分，因此也不需要大家对 simulator 进行优化。

## 评分

* 评分细则，已经公布在 icisc 官网 [https://eda.icisc.cn/download/index?type=2](https://eda.icisc.cn/download/index?type=2)

* 赛题名为：`赛题一：海思-时序逻辑的高性能ATPG技术`

<a name="race-pack"></a>

## 作品打包要求

各参赛队伍的代码需要将在提交作品前先生成一个发布包。关于软件的包的目录结构，请务必遵循如下要求：

* 软件包的命名方式是：`<TEAM-TAG>.tar.gz`，其中，`<TEAM-TAG>` 为参赛队伍的名称唯一性识别标签，该标签必须符合正则表达式：`[A-Za-z0-0]+`
* 软件包的目录结构为：
  
```
  <TEAM-TAG>
      +-- .TEAM
      +-- <Your-other-files>
      +-- ...
```
其中，
  * `<TEAM-TAG>` 是软件包的最顶层目录，和软件包中的部分是相同的。
  * `.TEAM` 是软件包的启动脚本（必须为 bash 脚本），会被自动验收工具自动加载。 

`.TEAM` 这个脚本也是可选的，只有当您遇到下面几种情况中的一种或者多种时，才必须提供 `.TEAM` 文件：
  1. 您修改了编译的目标的名称，名字已经不叫 `opentpg`；
  2. 您的目标文件不放在 `<TEAM-TAG>` 目录下；
  3. 您的目标文件依赖了第三方库，需要在启动时指定依赖的第三方库的路径；

`.TEAM` 文件中可以定义下面几个变量：
  * `TEAM_APP` 用于指定您的 app 的位置；
  * `TEAM_NAME` 用于指定您的参赛队伍的真实名称；
  * `TEAM_VERSION` 用于追踪您提交的参赛的版本号；

## 作品提交

由于举办方目前没有提供统一的作品提交平台。我们采用直接在 ARM 开发环境提交的方式。

具体方法为：

* 第一步，在您的开发服务器 `${HOME}` 目录下新建一个名字为 `product-release` 的目录；

参考命令：
```shell
mkdir -p ${HOME}/product-release
```

* 第二步，按照 [作品打包要求](#race-pack) 的要求对编译好的二进制文件以及依赖库（如果有）打包，然后将包拷贝到 `${HOME}/product-release`；

* 第三步，将源代码包整体打包放到 `${HOME}/product-release` 目录下，包名固定为 `source.tar.gz`；

赛题验收方会在作品提交时间截止后，从 `${HOME}/product-release` 目录下拷贝作品做作品验收。上面这些动作请务必在 **作品提交截止时间** 前完成，
不要在截止时间之后继续拷贝，一方面可能会晚了，不会重新拷贝；另一方面，如果刚好碰到收包脚本正在拉去你的代码，会导致拉取的文件半新半旧的问题。

下面答复几个可能的担忧：


## FAQ

#### 别人看到我代码咋办？

服务器上所有用户的 ${HOME} 目录都是隔离的，别人看不了你的代码。只有出题方在收集各个队伍的作品包和代码包时，会通过特殊权限拷贝作品。

附带提一句：建议大家也跟赛事组织者提提意见搭建一套作品提交的平台，以方便后续赛事的作品提交环节。

#### 提交作品时为什么代码也要提交？

一方面，万一在验收过程中遇到重大问题，还有可能通过重新编译来抢救一下。
另一方面，验收方需要对代码做一下 Review 排除抄袭，作弊等嫌疑。

#### 作品提交完毕之后，我能删掉 ${HOME}/product-release 下的东西吗？

最好不要删除。虽然，我们已经反复演练了验收过程，但是验收过程仍然还是比较复杂，涉及较多人工操作过程。
验收的持续时间也比较长。我们必须考虑万一验收过程中丢搞丢了作品包，还有机会补救。


