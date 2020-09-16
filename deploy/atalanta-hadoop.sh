#!/usr/bin/env bash

###################################################################################################
#   脚本接口定义：
#   1. 脚本文件名必须为 atalanta-hadoop.sh
#   2. 为避免环境参数无法传递给本脚本，这里要求本脚本必须采用 bash 编写
#   3. 脚本支持下面三个参数，而且这三个参数与 main 函数的输入参数完全相同，如下：
#       $1  team        团队名称
#       $2  rankdir     测试用例所在的目录（网表）
#       $3  rankname    测试名称(不含bench后缀)
#   4. 本脚本支持几个环境变量：
#       ${JAVA_HOME}    JDK 安装目录
#       ${HADOOP_HOME}  hadoop 的安装目录
#       ${REDIS_HOME}   redis 的安装目录
#   5. 下面几个参数必须原样配置
#       -input      "/${team}-${rankname}-input"
#       -output     "/${team}-${rankname}-output"
#   6. 下面这些参数可以自行追加，但现有内容不可删除或者修改（重要）：
#       -files      "${SELFDIR}/atalanta,${rankdir}/${rankname}.bench"
#   7. 参赛队伍可以根据需要添加其他的参数
###################################################################################################


###################################################################################################
#   获得本脚本所在的目录
SELFDIR=$(cd $(dirname "$BASH_SOURCE");pwd)


###################################################################################################
#   支持调试
if [[ "${RANK_DEBUG}" == "true" ]]; then
    set -x
fi

###################################################################################################
#   $1  team        团队名称
#   $2  rankdir     测试用例所在的目录（网表）
#   $3  rankname    测试名称(不含bench后缀)
function main()
{
    local   team="$1"
    local   rankdir="$2"
    local   rankname="$3"

    local   streamfile="${HADOOP_HOME}/share/hadoop/tools/lib"/hadoop-streaming-3.3.0.jar
    "${HADOOP_HOME}/bin/hadoop" jar "${streamfile}"                                 \
        -D          "mapreduce.job.maps=5"                                          \
        -files      "${SELFDIR}/atalanta,${rankdir}/${rankname}.bench"              \
        -input      "/${team}-${rankname}-input"                                    \
        -output     "/${team}-${rankname}-output"                                   \
        -mapper     "atalanta --exec atpg          --netlist  ${rankname}.bench"    \
        -reducer    "atalanta --exec simulate-cube --netlist  ${rankname}.bench"
    RESULT=$?
    return  ${RESULT}
}


###################################################################################################
main    "$@"
exit    "$?"
