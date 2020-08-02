# 目录

[概述](#summary)
- [软件介绍](#summary-introduction)
- [软件架构](#summary-architecture)

[安装说明](#install)
- [基本要求](#install-required)
- [构建](#install-build)
- [部署](#install-deploy)
  - [部署注备工作](#install-deploy-ready)
  - [部署方式一：开发环境集成部署](#install-deploy-way01)
  - [部署方式二：独立环境部署](#install-deploy-way02)
- [小测验](#install-test)

[快速入门](#quickstart)

<a name="summary"></a>
# 概述

OpenTPG 项目的目标是构建开源的 TPG 工具集。当前还处于项目的非常早期阶段，当前，短期目标为满足 ICISC 竞赛出题诉求。欢迎有志于 TPG 领域的同学参与本项目。
通过参加本项目你将学会下面几项技能：

1. 您将掌握芯片领域非常关键的 TPG 流程的基本原理和 ATPG 相关关键算法的基本实现思路
2. Hadoop 是大数据技术的基础性工具，掌握 Hadoop 同时也将让您大数据技术纳入自己的基本技能集
3. 基础的 Linux 开发技能，C++开发能力，现代化的工程管理方式等等

关于 ICISC 竞赛相关的信息，参见：

[doc/race-icisc-2020.md](doc/race-icisc-2020.md)

<a name="summary-introduction"></a>
## 软件介绍


<a name="summary-architecture"></a>
## 软件架构



<a name="install"></a>
# 安装说明

<a name="install-required"></a>
## 基本要求
----------------------

* 外部依赖

| 需求项 | 版本         | 下载地址 |
|---     |---           |---       |
| Java   | 1.8(8u251)   | [OpenJDK发行版：https://www.oracle.com/java/technologies/javase/javase-jdk8-downloads.html](https://www.oracle.com/java/technologies/javase/javase-jdk8-downloads.html)  |
| hadoop | 3.2.1        | [Acache发行版：https://hadoop.apache.org/releases.html](https://hadoop.apache.org/releases.html) |
| redis  | 6.0.5        | [Redis下载地址：https://redis.io/download](https://redis.io/download) |

需要注意：目前我们没有做很深入的交叉配套验证，如无必要，建议直接使用对应版本号的第三方软件.

* 支持的操作系统

目前团队人数和设备都有限，我们主要的验证环境是 Linux，但团队其他成员使用自身设备也做了一些不完整验证.
如果您想做完整功能测试，请尽量采用 Linux 系统进行测试. 如果您已经验证过某些功能可用，那么通过 issue 请告诉我们：

| 环境                  | 编译 | 打包 | 部署 | 本地TPG流程 | 基于Hadoop的TPG流程 |
|---                    |---   |---   |---   |---          |---                  |
| Linux+gcc             | OK   | OK   | OK   | OK          | OK |

<!--
{* | Windows+VS2019        | OK   | --   | --   | OK          | -- | *}
{* | macos+clang           | OK   | OK   | OK   | OK          | Fail(hadoop不支持mac) | *}
-->

* shell 环境

我们所有的脚本都需要在 bash 环境下执行. 您可以通过下面的命令检查当前的shell环境：

```bash
echo $SHELL
```

如果您当前的 shell 并不是 bash，那么直接输入下面的指令即可切换到 bash：

```bash
bash
```

<a name="install-build"></a>
## 构建
----------------------

OpenTPG 项目提供了一键编译脚本，构建方式为

```bash
sh build.sh [compile] [debug|release]
```
 
- 如果是构建 debug 版本，最简单的方式为：`sh build.sh`
- 如果是构建 release 版本，最简单的方式是：`sh build.sh release`


<a name="install-deploy"></a>
## 部署
----------------------

OpenTPG 支持两种部署方式："编译环境集成部署"和"独立环境部署"。相对而言，开发环境集成部署更加简便，比较适合在开发人员自测时采用。而独立环境部署，增加了额外的步骤和操作，但很适合开发环境和部署环境分离的场景。

<a name="install-deploy-ready"></a>
#### 部署注备工作

您需要提前准备好所有需要的依赖包，这些包需要放在同一个目录下，参见前面的'外部依赖'小节。


<a name="install-deploy-way01"></a>
#### 部署方式一：开发环境集成部署

"开发环境集成部署"是指开发环境和测试环境是同一套环境，这种方式主要是方便开发人员的开发调测，避免在安装部署上浪费太多时间. 如果您计划参与项目开发，推荐采用这种方式。

开发环境部署时，到项目的 deploy 目录下，执行下面的命令：

```bash
sh install.sh <SOFTWARE-DIR> ./
```

其中：

  - `<SOFTWARE-DIR>` 是所有 `外部依赖` 的软件包的存放的目录
  - 第二个参数指的是部署到当前目录下，当然这里也就是depoy所在的目录


执行完毕之后，还需要在安装目录下通过下面的指令加载配置：

```bash
source setup.bash
```

开发环境部署完毕后，后续修改了代码或者脚本，编译完毕之后，都是可以直接使用的，不需要重新执行部署操作。

<a name="install-deploy-way02"></a>
#### 部署方式二：独立环境部署

独立环境部署时，需要单独生成一个安装包，然后利用该安装包在目标环境上安装。

* 打包

打包操作也可以直接由 build.sh 来做：

```bash
sh build.sh package <TEAMNAME>
```

其中：

  - `<TEAMNAME>` 为本团队的名称，命名规则需要满足正则表达式：`[A-Za-z][A-Za-z0-9]*`.
  - 生成的软件包的名字为 `hiatpg-<TEAMNAME>-<OS>-<TIMESTAMP>.tar.gz`，比如：`hiatpg-MyTeam-Linux-200723081617.tar.gz`

* 部署

首先，需要将前面打包步骤生成的二进制包通过 sftp、scp 等工具或者其他的任意你喜欢的方式拷贝到部署环境.

然后，依次执行下面的命令执行安装：

```bash
tar xvfz <YOUR-PACKAGE>.tar.gz
cd <YOUR-PACKAGE>
sh install.sh <SOFTWARE-DIR> <INSTALL-DIR>
```

其中：
 
  - `<YOUR-PACKAGE>` 请替换成您生成的安装包的名字
  - `<SOFTWARE-DIR>` 是所有`外部依赖`的软件包的存放的目录
  - `<INSTALL-DIR>`  指具体安装到什么位置


<a name="install-test"></a>
## 小测验
----------------------

进入安装目录，依次执行下面的指令以检验是否整个安装过程已经成功：

```bash
source setup.bash
sh rank.sh myteam ./samples/c17.bench ./
cat ./myteam@c17@hadoop/c17.pattern
```

如果执行成功，可以看到输出的pattern列表：

```text
pattern:0	11011
pattern:1	10110
pattern:2	00110
pattern:3	11011
pattern:4	00000
pattern:5	10010
pattern:6	01100
pattern:7	00011
```

这里 rank.sh 是我们的测试驱动脚本，用于自动帮助我们完成一轮测试全流程。



<a name="quickstart"></a>
# 快速入门

当安装完毕之后，可以在安装目录下找到 rank.sh 脚本。 这个脚本可以帮助我们一键完成一轮测试。

rank.sh 脚本的命令行选项选项如下：

```bash
rank.sh <TEAM> <INPUT-NETLIST-FILE> [<OUTPUT-DIR>]
```

其中：

- `<TEAM>` 团队的名称，该名称必须匹配正则表达式 `[A-Za-z][A-Za-z]`
- `<INPUT-NETLIST-FILE>` 需要执行 TPG-Flow 的网表文件名(含路径)，我们统一约定网表文件需要以 .bench 后缀命名
- `[<OUTPUT-DIR>]` 指名 TPG-Flow 所产生的文件应该输出到哪里. 这是个本地的目录. 如果不指定，默认为当前工作目录

如果 rank.sh 执行成功，脚本会返回 0，否则返回非 0。下面是一个简单的示例：

```bash
sh rank.sh team1 ./samples/c17.bench
```

执行完毕之后，可以在当前工作目录下看到 `team1@c17@hadoop` 的目录。 下面为该目录下的信息：

```text
$ ls -l
total 512
-rwxrwxr-x. 1 liugang liugang 498616 Jul 24 23:29 atalanta
-rw-rw-r--. 1 liugang liugang    236 Jul 24 23:29 c17.bench
-rw-rw-r--. 1 liugang liugang      2 Jul 24 23:29 c17.cost
-rw-rw-r--. 1 liugang liugang    148 Jul 24 23:29 c17.fault
-rw-rw-r--. 1 liugang liugang    128 Jul 24 23:29 c17.pattern
-rw-rw-r--. 1 liugang liugang    272 Jul 24 23:29 c17.report
drwxr-xr-x. 2 liugang liugang   4096 Jul 24 23:29 output
```

其中：

- `atalanta` 是 rank.sh 脚本帮助我们备份的目标程序
- `xxx.bench` 是 rank.sh 脚本帮助我们备份的网表（netlist）文件
- `xxx.cost` 是脚本统计的关键任务的执行耗时
- `xxx.fault` 是脚本根据网表文件自动生成的 faultlist 文件
- `xxx.pattern` 是脚本最终产生的 pattern 文件
- `xxx.report` 是最终的统计报告
- `output` 这是个目录，是 hadoop 的原始输出文件

