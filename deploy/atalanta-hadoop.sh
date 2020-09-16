#!/usr/bin/env bash

#   本脚本所在的目录
SELFDIR=$(cd $(dirname "$BASH_SOURCE");pwd)

#   $1  team        团队名称
#   $2  rankdir     测试用例所在的目录（网表）
#   $3  rankname    测试名称
function main()
{
    local   team="$1"
    local   rankdir="$2"
    local   rankname="$2"

    local   streamfile="${HADOOP_HOME}/share/hadoop/tools/lib"/hadoop-streaming-3.3.0.jar
    "${HADOOP_HOME}/bin/hadoop" jar "${streamfile}"                                 \
        -D          "mapreduce.job.maps=5"                                          \
        -files      "${SELFDIR}/atalanta,${rankdir}/${rankname}.bench"              \
        -input      "/${team}-${rankname}-input"                                    \
        -output     "/${team}-${rankname}-output"                                   \
        -mapper     "atalanta --exec atpg          --netlist  ${rankname}.bench"    \
        -reducer    "atalanta --exec simulate-cube --netlist  ${rankname}.bench"    \
    RESULT=$?
    return  ${RESULT}
}

main    "$@"
exit    "$?"
