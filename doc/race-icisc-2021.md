
# ICISC 竞赛

本项目已经连续 2 届承担参与 ICISC 竞赛出题，后续也将继续在 TPG 领域继续发力。

<a name="race-interface"></a>

## 系统接口定义

<a name="race-system-interface"></a>

### 软件目录结构

| 目录 | 用途 |
|--- |--- |
| doc | 文档材料 |
| HiAtpg/src | 一个简单的用于生成 TestCube 的代码框架 |
| HiAtpg/bench | 随工程携带的几个样例网表文件及其配置 |
| HiAtpg/thirdparty | 所有依赖的第三方库，当前只支持 header-only 的 C++ 库 |

<a name="race-system-interface"></a>

### 命令行接口

参见 [README.md](https://gitee.com/openeda/OpenTPG/blob/race-icisc-2021/README.md#system-interface)

<a name="race-code-interface"></a>

### 代码接口

### Cube 生成器：CubeGenerator

##### 1. 实现接口定义

参见文件 `CubeGenerator.h`

```c++
struct CubeGenerator
{
virtual ~CubeGenerator(){};
virtual TestCube* Generate(const Fault* fault) = 0;
};
```

`CubeGenerator` 是参赛的队伍必须实现的接口类。该接口只有一个纯虚函数 Generate 必须实现。 该函数输入一个 Fault 对象，并返回一个 TestCube 对象。 TestCube 和 Fault
的定义，可参见对应的头文件。

#### 2. 注册自己实现的 CubeGenerator

直接修改 CubeGenerator.cpp 中 `CreateCubeGenerator` 函数的实现，以便启用你自己的 CubeGenerator。

#### Cube 对象：TestCube

TestCube 对象对象承载了 CubeGenerator 计算出来的所有的 PI 类型和 DFF 类型的 Gate 的值。其数据是连续存放的。PI 在前，DFF 在后。 其内部数据结构定义如下，两层嵌套的 std::vector，外层
vector 为 Frame 的列表，内层 vector 该 Cycle 的关键 Gate 的值。

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

需要特别注意，只有首个 Cycle 里面里面是需要填写 DFF 的 Gate 的值的。后续的 Cycle 是不可以填写的。

TestCube 的输出一般不需要关心，如果有必要可以调用 CubeOutput 类的 PrintCubes2File 接口来输出。


### 文件接口

#### cube 文件

一个 cube 文件由一个或者多个 `TestCube Block`组成。而每个 `TestCube Block` 由 `TestCube Header` 和 `TestCube Cycle` 组成。
下面为一个 cube 文件的样例：

```text
$: 0
111110001111
01001000
01001000
$: 1
001110001010
11000001
11011001
$: 2
110111000110
00110101
00111x10
```

上述样例中，定义了 3 个 cube。每个 cube 都是以 `$:` 开头的。同时，`$:` 开头的行就是 `TestCube Header`；
两个`TestCube Header` 之间的部分就是 `TestCube Cycle`。


<a name="race-rating"></a>

## 评分

* 评分细则，已经公布在 icisc 官网 [https://eda.icisc.cn/download/index?type=2](https://eda.icisc.cn/download/index?type=2)

* 赛题名为：`赛题一：海思-时序逻辑的高性能ATPG技术`

<a name="race-faq"></a>
## FAQ


