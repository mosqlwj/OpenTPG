# 目录

[概述](#summary)

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

OpenTPG 项目的目标是构建开源的 TPG 工具集。

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
| boost   | 1.76            |
| cmdline | 3.2.1           |

* 构建工具链

 - gcc 7, C++11 
 - cmake 3.12 及以上

* 支持的操作系统

  - Linux 4.9 及以上

* 支持硬件环境

  - amd64
  - ARM64

<a name="install-build"></a>
## 构建
----------------------

```bash
mkdir build
cd build
cmake ../
make 
```


<a name="quickstart"></a>
# 快速入门

编译完成之后会生成一个名字为 HiAtpg 的命令行工具。该工具命令行接口如下：

| 参数定义 | 是否必选 | 参数含义 |
|---      |---      |---      |
| `-n <NETLIST-FILE>`   | Yes | 指定网表文件 |
| `-c <CONFIG-FILE>`    | Yes | 执行网表配置文件 |
| `-f <FAULTLIST-FILE>` | No  | 指定输出 FaultList 文件 | 
| `-g <GATE-FILE>`      | No  | 指定输出 Gate 信息文件 | 

## 一键流程

```shell
./HiAtpg -n bench/s17.bench  -c bench/s17.cfg   
```



