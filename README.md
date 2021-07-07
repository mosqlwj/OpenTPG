# 目录

[概述](#summary)
<!--
- [软件介绍](#summary-introduction)
- [软件架构](#summary-architecture)
-->
- [竞赛](#summary-race)
- [参与贡献](#summary-join)

[安装说明](#install)
- [基本要求](#install-required)
- [构建](#install-build)
- [部署](#install-deploy)
- [小测验](#install-test)

[快速入门](#quickstart)

<a name="summary"></a>
# 概述

OpenTPG 项目的目标是构建开源的 TPG 工具集。主要工具为：

* HiAtpg：XXXX

<!--
<a name="summary-introduction"></a>
## 软件介绍

<a name="summary-architecture"></a>
## 软件架构
-->

<a name="summary-race"></a>
## 竞赛

关于 2021 年 ICISC 竞赛相关的信息，参见：[doc/race-icisc-2021.md](doc/race-icisc-2021.md)


<a name="summary-join"></a>
## 参与贡献

我们非常欢迎，您通过下面的方式参与贡献：

- 通过 [提交 issue](https://gitee.com/openeda/opentpg/issues/new) 给项目提意见或者建议
- 通过 [发起 Pull Request](https://gitee.com/openeda/opentpg/pull/new) 直接给项目贡献代码
- 参与 issue 或者 Pull Request 的评审或者评论

<a name="install"></a>
# 安装说明

<a name="install-required"></a>
## 基本要求
----------------------

* 外部依赖

| 需求项  | 版本             |
|---      |---              |
| boost   | TODO            |
| cmdline | 3.2.1           |
| gcc     | 10+ 以上, C++17  |
| cmake   | 3.15 以上       |


* 支持的操作系统

  - Linux 4.9 + amd64
  - Linux 4.9 + ARM64

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

```bash
mkdir build
cd build
cmake ../
make 
make install
```


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

