# hadoop 安装

## 基本要求

1. 外部依赖

| 需求项 | 版本         | 下载地址 |
|---     |---           |---       |
| Java   | 1.8(8u251)   | [OpenJDK发行版](https://www.oracle.com/java/technologies/javase/javase-jdk8-downloads.html)  |
| hadoop | 3.2.1        | [Acache发行版](https://hadoop.apache.org/releases.html) |
| redis  | 6.0.5        | [Redis下载地址]https://redis.io/download |

需要注意: 目前我们没有做很深入的交叉配套验证,如无必要,建议直接使用对应版本号的第三方软件.

2. 支持的操作系统

目前团队人数和设备都有限,我们主要的验证环境是Linux,但团队其他成员使用自身设备也做了一些不完整验证.
如果您想做完整功能测试,请尽量采用Linux系统进行测试. 如果您已经验证过某些功能可用,那么通过issue请告诉我们:

| 环境            | 编译 | 打包 | 部署 | 本地TPG流程 | 基于Hadoop的TPG流程 |
|---              |---   |---   |---   |---          |---                  |
| Linux+gcc       | OK   |  OK  |  OK  |   OK        |  OK |
| Windows+vs2019  | OK   |  --  |  --  |   OK        |  -- |
| macos+clang     | OK   |  OK  |  OK  |   OK        |  Fail(hadoop不支持mac) |

## 构建

opentpg项目提供了一键编译脚本, 构建方式为

```
sh build.sh [compile] [debug|release]
```
 
如果是构建 debug 版本, 最简单的方式为 `sh build.sh`; 如果是构建 release 版本, 最简单的方式是: `sh build.sh release`;


## 部署

opentpg 支持两种部署方式, 编译环境集成部署,独立环境部署. 相对而言开发环境集成部署比较简单, 比较适合在开发人员自测使用.

### 开发环境集成部署

开发环境集成部署 是指开发环境和测试环境是同一套环境, 这种方式主要是方便开发人员的开发调测,避免在安装部署上浪费太多时间. 如果您计划参与项目开发, 推荐采用这种方式.

开发环境部署方法, 到项目的 deploy 目录下执行下面的命令:

```
sh install.sh <SOFTWARE-DIR> ./
```

其中, `<SOFTWARE-DIR>` 是所有`外部依赖`的软件包的存放的目录.

这样可以直接将整个环境部署到 deploy 目录下, 后续当我们在做测试的时候也不需要重新编译和打包

## 独立环境安装

1. 编译

```
sh build.sh release
```


2. 打包

执行打包前,请确保编译过程成功执行.

```
sh build.sh package <TEAMNAME>
```

* `<TEAMNAME>` 为本团队的名称,命名规则需要满足正则表达式: `[A-Za-z][A-Za-z0-9]*`.

* 生成的软件包的名字为 `hiatpg-<TEAMNAME>-<OS>-<TIMESTAMP>.tar.gz`, 比如: `hiatpg-MyTeam-Linux-200723081617.tar.gz`

3. 部署

依次执行下面的命令安装进入 `hiatpg-<TEAMNAME>-<OS>-<TIMESTAMP>` 目录, 执行下面的命令安装

```
tar xvfz <YOUR-PACKAGE>.tar.gz
cd <YOUR-PACKAGE>
sh install.sh <SOFTWARE-DIR> <INSTALL-DIR>
```

其中, `<YOUR-PACKAGE>` 请替换成您生成的安装包的名字, `<SOFTWARE-DIR>` 是所有`外部依赖`的软件包的存放的目录; `<INSTALL-DIR>` 指具体安装到什么位置.



