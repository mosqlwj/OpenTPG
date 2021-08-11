
# ICISC 竞赛

本项目已经连续 2 届承担参与 ICISC 竞赛出题，后续也将继续在 TPG 领域继续发力。

往届 ICISC 竞赛参见 [往届 ICISC 竞赛](#race-history)


<a name="race-interface"></a>
## 系统接口定义




<a name="race-interface-install"></a>
### 命令行接口

参见 [README.md](https://gitee.com/openeda/OpenTPG/blob/race-icisc-2021/doc/README.md#system-interface)


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


<a name="race-history"></a>
# 附：往届 ICISC 竞赛

- [2020 届 ICISC 竞赛](https://gitee.com/openeda/OpenTPG/blob/race-icisc-2020/doc/race-icisc-2020.md)



