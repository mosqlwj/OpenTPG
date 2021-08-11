
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

TestCube 对象对象承载了 CubeGenerator 计算出来的所有的 PI 类型和 xxx 类型的 Gate 的值。其数据是连续存放的。PI 在前，xxx 在后。

TestCube 的输出一般不需要关心，如果有必要可以调用 CubeOutput 类的 PrintCubes2File 接口来输出。

```c++
std::vector<std::vector<GateValue>> logicValue;
```

<a name="race-rating"></a>

## 评分

* 评分细则，已经公布在 icisc 官网 [https://eda.icisc.cn/download/index?type=2](https://eda.icisc.cn/download/index?type=2)

* 赛题名为：`赛题一：海思-时序逻辑的高性能ATPG技术`

<a name="race-faq"></a>
## FAQ


