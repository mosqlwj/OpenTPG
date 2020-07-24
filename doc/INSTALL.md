# 安装说明

## 基本要求
----------------------

* 外部依赖

| 需求项 | 版本         | 下载地址 |
|---     |---           |---       |
| Java   | 1.8(8u251)   | [OpenJDK发行版](https://www.oracle.com/java/technologies/javase/javase-jdk8-downloads.html)  |
| hadoop | 3.2.1        | [Acache发行版](https://hadoop.apache.org/releases.html) |
| redis  | 6.0.5        | [Redis下载地址](https://redis.io/download) |

需要注意: 目前我们没有做很深入的交叉配套验证,如无必要,建议直接使用对应版本号的第三方软件.

* 支持的操作系统

目前团队人数和设备都有限,我们主要的验证环境是Linux,但团队其他成员使用自身设备也做了一些不完整验证.
如果您想做完整功能测试,请尽量采用Linux系统进行测试. 如果您已经验证过某些功能可用,那么通过issue请告诉我们:

| 环境                  | 编译 | 打包 | 部署 | 本地TPG流程 | 基于Hadoop的TPG流程 |
|---                    |---   |---   |---   |---          |---                  |
| Linux+gcc             | OK   | OK   | OK   | OK          | OK |
| Windows+VS2019        | OK   | --   | --   | OK          | -- |
| macos+clang           | OK   | OK   | OK   | OK          | Fail(hadoop不支持mac) |

## 构建
----------------------

opentpg项目提供了一键编译脚本, 构建方式为

```
sh build.sh [compile] [debug|release]
```
 
- 如果是构建 debug 版本, 最简单的方式为 `sh build.sh`; 
- 如果是构建 release 版本, 最简单的方式是: `sh build.sh release`;


## 部署
----------------------

opentpg 支持两种部署方式: 编译环境集成部署和独立环境部署. 相对而言, 开发环境集成部署更加简便, 比较适合在开发人员自测时采用.而独立环境部署,增加了额外的步骤和操作,但很适合开发环境和部署环境分离的场景.

#### 部署注备工作

您需要提前准备好所有需要的依赖包, 这些包需要放在同一个目录下, 参见前面的'外部依赖'小节.


#### 部署方式一:开发环境集成部署

开发环境集成部署 是指开发环境和测试环境是同一套环境, 这种方式主要是方便开发人员的开发调测,避免在安装部署上浪费太多时间. 如果您计划参与项目开发, 推荐采用这种方式.

开发环境部署时, 到项目的 deploy 目录下, 执行下面的命令:

```
sh install.sh <SOFTWARE-DIR> .
```

其中:

  - `<SOFTWARE-DIR>` 是所有 `外部依赖` 的软件包的存放的目录.
  - 第二个参数指的是部署到当前目录下,当然这里也就是depoy所在的目录.

开发环境部署完毕后, 后续修改了代码或者脚本, 编译完毕之后, 都是可以直接使用的, 不需要重新执行部署操作.

#### 部署方式二:独立环境部署

独立环境部署时,需要单独生成一个安装包,然后利用该安装包在目标环境上安装.

* 打包

打包操作也可以直接由 build.sh 来做:

```
sh build.sh package <TEAMNAME>
```

其中:

  - `<TEAMNAME>` 为本团队的名称,命名规则需要满足正则表达式: `[A-Za-z][A-Za-z0-9]*`.
  - 生成的软件包的名字为 `hiatpg-<TEAMNAME>-<OS>-<TIMESTAMP>.tar.gz`, 比如: `hiatpg-MyTeam-Linux-200723081617.tar.gz`

* 部署

首先, 需要将前面打包步骤生成的二进制包通过sftp,scp等工具或者其他的任意你喜欢的方式拷贝到部署环境.

然后, 依次执行下面的命令执行安装:

```
tar xvfz <YOUR-PACKAGE>.tar.gz
cd <YOUR-PACKAGE>
sh install.sh <SOFTWARE-DIR> <INSTALL-DIR>
```

其中:
 
  - `<YOUR-PACKAGE>` 请替换成您生成的安装包的名字;
  - `<SOFTWARE-DIR>` 是所有`外部依赖`的软件包的存放的目录; 
  - `<INSTALL-DIR>`  指具体安装到什么位置;

## 小测验
----------------------

进入安装目录,依次执行下面的指令以检验是否整个安装过程已经成功:

```shell script
source setup.bash
sh rank.sh myteam ./samples/c17.bench ./
cat ./myteam@c17@hadoop/c17.pattern
```




