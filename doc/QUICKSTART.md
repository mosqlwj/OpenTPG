# 快速入门

## 前提

请参考 [安装说明](doc/INSTALL.md) 准备好安装环境.

## 启动 TPG-Flow


#### rank.sh 脚本的用法

当安装完毕之后, 可以在安装目录下找到 rank.sh 脚本. 这个脚本可以帮助我们一键完成一轮测试.

rank.sh 脚本的命令行选项,可通过 -h 参数查看:

```bash
$ sh rank.sh -h
----------------
Usage:
   rank.sh <TEAM> <INPUT-NETLIST-FILE> [<OUTPUT-DIR>]

Options:
   <TEAM>                  The name of your team.
   <INPUT-NETLIST-FILE>    The netlist file.
   <OUTPUT-DIR>            Where to write the output files.
```

其中:

- `<TEAM>` 团队的名称, 该名称必须匹配正则表达式 `[A-Za-z][A-Za-z]`.
- `<INPUT-NETLIST-FILE>` 需要执行 TPG-Flow 的网表文件名(含路径), 我们统一约定网表文件需要以 .bench 后缀命名.
- `[<OUTPUT-DIR>]` 指名 TPG-Flow 所产生的文件应该输出到哪里. 这是个本地的目录. 如果不指定, 默认为当前工作目录.

如果 rank.sh 执行成功, 脚本会返回 0, 否则返回非 0;

#### rank.sh 脚本的输出信息

rank.sh 脚本会在 \<OUTPUT-DIR\> 目录下以输入参数的信息作为基础生成一个子目录,用于存放所有的输出文件, 比如, 最后生成的 pattern 列表, 统计数据等等.

如下所示:

```bash
sh rank.sh team1 ./samples/c17.bench ./
```

该命令生成的存放输出文件的子目录名是: `team2@c17@hadoop`. 该子目录下的文件如下:

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

其中:

- `atalanta` 是 rank.sh 脚本帮助我们备份的目标程序
- `xxx.bench` 是 rank.sh 脚本帮助我们备份的网表(netlist)文件
- `xxx.cost` 是脚本统计的关键任务的执行耗时
- `xxx.fault` 是脚本根据网表文件自动生成的 faultlist 文件
- `xxx.pattern` 是脚本最终产生的pattern文件
- `xxx.report` 是最终的统计报告
- `output` 这是个目录,是 hadoop 的原始输出文件

