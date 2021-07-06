
# ICISC 竞赛

本项目已经连续 2 届承担参与 ICISC 竞赛出题，后续也将继续在 TPG 领域继续发力。

往届 ICISC 竞赛参见 [往届 ICISC 竞赛](#race-history)


<a name="race-interface"></a>
## 系统接口定义

HiAtpg 主要功能是读入网表及其配置，然后输出 TestCube。


<a name="race-interface-install"></a>
### 关键脚本命令行接口

- `install.sh <SOFTWARE-DIR> <INSTALL-DIR>` 安装脚本命令行接口
- `rank.sh <TEAM> <INPUT-NETLIST-FILE> [<OUTPUT-DIR>]` 测试脚本接口
- `build.sh  [compile] [build|release]` 编译
- `build.sh  package   [<NAME>]` 打包

具体每个参数的含义，可以通过该脚本的 -h 选项查看，比如：
```bash
./install.sh -h
```



<a name="race-case"></a>
## Case

竞赛的 case 最终会放到项目的 [race](race) 目录下。竞赛开始之后，并不会立即提供用于评分的 Case。我们会在正式进入竞赛评分环节之前，会提前 1 到 2 周发布最终的评分 Case。
然而，我们仍然在项目的 [bench](bench) 目录下提供了大量的不同规模和复杂度的 Case。所以，您只需要充分对现有的 Case 进行充分的测试和调优即可，而不必对最终的评分 Case 太过期待或者依赖。

<a name="race-case"></a>
## 自助评估

如何评估自己的优化效果？很简单，直接通过 rank.sh 脚本跑跑用例，在最终的 xxx.report 文件中可以看到最终的统计数据。

<a name="race-rating"></a>
## 评分

评分细则，已经公布在 icisc 官网 [赛题五：基于分布式计算框架的自动测试向量生成算法：http://eda.icisc.cn/download/index?type=2](http://eda.icisc.cn/download/index?type=2)

<a name="race-faq"></a>
## FAQ

#### 执行脚本时报语法错误

问题原因：该常见于 Ubuntu 系统高版本用户，Ubuntu 的高版本，操作系统默认的 sh 并不是 bash，导致使用 sh 启动脚本时，无法识别 bash 语法从而报错。

解决办法：直接改用 bash 启动脚本。比如，`bash ./install.sh`

#### 执行脚本时提示，realpath 命令不存在

问题原因：也是常见于 Ubuntu 系统高版本用户，realpath 本来是安全相关的一个重要的命令，但是 Ubuntu 看起来并没有默认安装这个程序。

解决办法：我们已经更新了脚本，改用了替代命令，您可以直接将竞赛仓库的代码合入进来即可。git 合并操作可自行找 git 相关资料。

#### install.sh 或者 rank.sh 脚本每次执行都要输入密码

问题原因：当前操作系统不支持通过 localhost 连接同机进程时免密登录。

解决办法：手工给 ssh 配置免密登录即可。具体配置方式可参考 Apache Hadoop 官网。下面将相关脚本流程直接拷贝过来了：

```txt
第一步: 首先检查下是否可以通过 ssh 免密登录 localhost:

  $ ssh localhost

第二步: 如果不能够免密登录，那么执行下面的命令，然后重复前一步验证一下:

  $ ssh-keygen -t rsa -P '' -f ~/.ssh/id_rsa
  $ cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
  $ chmod 0600 ~/.ssh/authorized_keys
```

#### 测试脚本执行时提示 JAVA_HOME 不存在

问题原因：原因可能有三个：

1. Java 没安装；
2. 安装后没有配置 JAVA_HOME 环境变量；
3. Java 已经安装了，也配置了 JAVA_HOME 环境变量，但未生效；

解决办法：这个问题需要大家理解 Linux/Unix 系统环境变量如何工作的，属于 Linux/Unix 的一个基础知识点。如果不理解原理建议额外学习下相关的知识避免后面再次被这种问题阻碍大家的学习和工作。这里，具体的解决办法如下：

1. 安装 Java（注意版本要正确），一般 Java 的安装就是一个解压缩的过程；
2. 在 `~/.bashrc` 中配置 JAVA_HOME 环境变量，并将 ${JAVA_HOME}/bin 加入 PATH 环境变量；
3. 退出所有的 shell，然后重新进入 shell 环境；
4. 执行 `java version` 检验 java 是否正确安装；
5. 执行 `echo ${JAVA_HOME}` 验证 JAVA_HOME 是否生效；



<a name="race-history"></a>
# 附：往届 ICISC 竞赛

- [2020 届 ICISC 竞赛](https://gitee.com/openeda/OpenTPG/blob/race-icisc-2020/doc/race-icisc-2020.md)



TestCube.h cube的接口
CubeOutput.h 打印cube

