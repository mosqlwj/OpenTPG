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

## 安装步骤

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

部署前,请准备好下面依赖的软件包.