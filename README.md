# 目录

[概述](#summary)

- [竞赛](#summary-race)
- [参与贡献](#summary-join)

[安装说明](#install)

- [基本要求](#install-required)
- [构建](#install-build))

[快速入门](#quickstart)

[系统接口](#system-interface)

- [约定](#convention)
- [命令行接口](#command-line-interface)

<a name="summary"></a>

# 概述

OpenTPG 项目的目标是构建开源的 TPG 工具集。 opentpg 为本项目首个工具，其主要功能就是生成 cube，是完整 ATPG 流程的核心部分之一。

<a name="summary-race"></a>

## 竞赛

关于 2021 年 ICISC 竞赛相关的信息，参见：[doc/race-icisc-2021.md](doc/race-icisc-2021.md)

<a name="summary-join"></a>

## 参与贡献

我们非常欢迎，您通过下面的方式参与贡献：

- 通过 [提交 issue](https://gitee.com/openeda/OpenTPG/issues/new) 给项目提意见或者建议
- 通过 [发起 Pull Request](https://gitee.com/openeda/OpenTPG/pull/new) 直接给项目贡献代码，贡献代码请遵循[CodeStyle](.convention/code-style.md)
- 参与 issue 或者 Pull Request 的评审或者评论

<a name="install"></a>

# 安装说明

<a name="install-required"></a>
## 基本要求
----------------------

* 外部依赖

| 需求项  | 版本             |
|---      |---              |
| boost   | 1.76            |
| cmdline | 3.2.1           |

* 构建工具链

- gcc 7, C++11
- cmake 3.12 及以上

<a name="install-build"></a>
## 构建
----------------------

```shell
mkdir build
cd build
cmake ../
make 
```

<a name="quickstart"></a>

# 快速入门

整体计算流程为：

```text
+---------+    +-----------+    +--------+
| netlist |--->| faultlist |-+->|  cube  |
+---------+    +-----------+ |  +--------+
                             |  +--------+
                             +->| status |
                                +--------+
```

命令行为：

```shell
./opentpg -n bench/s17.bench  -c bench/s17.cfg -f bench/s17.fault -u bench/s17.cube
```

启动之后，进程将自动从 `bench/s17.bench` 和 `bench/s17.cfg` 读取输入，并将生成的 fault 和 cube 写 入文件 `bench/s17.fault` 和 `bench/s17.cube`。

<a name="system-interface"></a>

# 系统接口

<a name="convention"></a>

## 约定

合适的约定将极大简化项目成员之间的沟通。根据不同的用途和场景，这里列出本项目涉及到的主要的约定：

* 文件名后缀约定

| 后缀名     | 用途        |  
|---        |---          | 
| `*.bench` | 网表输入文件 |  
| `*.cfg`   | 网表配置文件 |  
| `*.fault` | 导出的 Fault 列表 |  
| `*.cube`  | 导出的 Cube 列表 | 

* 命令行接口约定

一个命令行的参数，有两种两种组织形式 `<key> <alue>` 和 `<key>`。其中，`<key>` 必须以符号 `-` 或者 `--` 开头。特殊情况下，使用 `--` 对下一个参数进行转义。

* 代码工程约定

`约定1`：我们约定一个类的文件名总是和该文件中的主要类的类名或相同（包括大小写）。
`约定2`：如果一个文件不设计主要类，那么该文件名必须为全小写字母

<a name="command-line-interface"></a>

## 命令行接口

可以通过 `opentpg --help` 来查看所有的命令行定义。

下面将列出主要的命令行接口：

| 参数定义               | 是否必选 | 参数含义 |
|---                    |---      |---      |
| `-n\|--netlist <NETLIST-FILE>`   | Yes     | 指定网表文件 |
| `-c\|--config <CONFIG-FILE>`     | Yes     | 指定网表配置文件 |
| `-f\|--fault <FAULTLIST-FILE>`   | Yes      | 执行导出 Fault 列表功能，<FAULTLIST-FILE> 为指定输出的 FaultList 文件 |
| `-u\|--cube <CUBELIST-FILE>`     | Yes      | 执行导出 Cube 列表功能<CUBELIST-FILE>指定输出 Cube 信息文件 |
