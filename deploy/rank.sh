#!/usr/bin/env bash


SELFDIR=$(dirname $(realpath "$BASH_SOURCE"))


#   准备环境
function    prepare_env()
{
    #   先检查下关键的环境配置文件是否存在
    if [[ ! -f "${SELFDIR}/setup.bash" ]]; then
        echo    "Error: Can not access the setup.bash at '${SELFDIR}'"
        return  1
    fi

    #   加载环境配置信息
    source  "${SELFDIR}/setup.bash"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Load the settings failed(${RESULT}): '${SELFDIR}/settings.h'"
        return  2
    fi

    echo    "JAVA_HOME      :   ${JAVA_HOME}"
    echo    "HADOOP_HOME    :   ${HADOOP_HOME}"
    echo    "REDIS_HOME     :   ${REDIS_HOME}"

    return  0
}

#
function    start_hadoop()
{
    "${HADOOP_HOME}/sbin/start-dfs.sh"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Start up hadoop-dfs failed(${RESULT})"
        return  3
    fi

    echo    "Start up hadoop-dfs success"
    return  0;
}

function    stop_hadoop()
{
    "${HADOOP_HOME}/sbin/start-all.sh"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Stop hadoop-dfs failed(${RESULT})"
        return  4
    fi

    echo    "Stop hadoop-dfs success"
    return  0
}

function    start_redis()
{
    mkdir -p "${REDIS_HOME}/log"
    nohup "${REDIS_HOME}/bin/redis-server" "${REDIS_HOME}/conf/redis.conf"  2>&1 > "${REDIS_HOME}/log/redis-server.log" < /dev/null &
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Start redis-server failed(${RESULT})"
        return  5
    fi

    echo    "Start redis-server success"
    return  0
}

function    stop_redis()
{
    local pids=$(ps -ef | grep "${REDIS_HOME}/redis-server" | awk 'print $2')
    if [[ $(echo ${pids} | wc -w) -ne 0 ]]; then
        for pid in ${pids} ; do
            kill -9 ${pid}
        done
    fi

    echo    "Stop redis-server completed"
    return  0
}



#   $1 team
#   $2 inputdir
#   $3 outputdir
function    execute_rank()
{
    local team="$1"
    local inputdir="$2"
    local outputdir="$3"


    local timestamp=$(date '+%Y%m%d%H%M%S')
    local scenename=$(basename "${inputdir}")
    local outputname="${team}-${scenename}"


    #   清理下工作目录
    local   rankdir="${outputdir}/${outputname}"
    if [[ -d "${rankdir}" ]]; then
#        rm -rf "${rankdir}"
        if [[ -d "${rankdir}" ]]; then
            echo    "Error: Can not found clean the output directory: '${rankdir}'"
            return  5
        fi
    fi
    mkdir -p    "${rankdir}"


    #   找到作为输入的bench文件
    local benchfile=$(find "${inputdir}" -name *.bench | head -n 1)
    if [[ "${benchfile}" == "" ]]; then
        echo    "Error: Can not access the bench file at: '${inputdir}'"
        return  6
    fi
    local rankname=$(basename "${benchfile}" | sed 's/.bench//g')


    #   将bench文件拷贝过来
    cp  -f  "${benchfile}"  "${rankdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Backup bench file failed(${RESULT}): '${benchfile}' -> '${rankdir}'"
        return  5
    fi


    #   先再本地生成 faultlist
    local faultfile="${rankdir}/${rankname}.fault"
    "${SELFDIR}/atlanta"    --exec      "create-fault" \
                            --netlist   "file:${benchfile}"  >  "${faultfile}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Create fault-list for bench failed(${RESULT}): '${benchfile}' -> ${rankdir}/c17.fault"
        return  5
    fi


    #   清理旧的输入和输出目录
    "${HADOOP}/hdfs" dfs -rm -r -f "/${team}-${scenename}-input"
    "${HADOOP}/hdfs" dfs -rm -r -f "/${team}-${scenename}-output"


    #   创建输入目录
    "${HADOOP}/hdfs" dfs -mkdir "/${team}-${scenename}-input"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Create input directory of dfs failed(${RESULT}): '/${team}-${scenename}-input'"
        return  5
    fi
    echo    "Create input directory of dfs success: '/${team}-${scenename}-input'"


    #   将前面生成的faultlist文件放入输入目录
    "${HADOOP}/hdfs" dfs -put   "${rankdir}/${rankname}.fault"  "/${team}-${scenename}-input"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Put the fault list file to dfs failed(${RESULT}): '${rankdir}/${rankname}.fault' -> '/${team}-${scenename}-input'"
        return  5
    fi
    echo    "Create input directory of dfs success: '/${team}-${scenename}-input'"


    #   启动hadoop
    local   streamfile="$HADOOP_HOME/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar"
    $HADOOP_HOME/bin/hadoop jar "${streamfile}"                                     \
        -input      "/${team}-${scenename}-input"                                   \
        -output     "/${team}-${scenename}-output"                                  \
        -mapper     "atalanta --exec atpg     --netlist  file:${rankname}.bench}"   \
        -reducer    "atalanta --exec simulate --netlist  file:${rankname}.bench}"   \
        -file       "${SELFDIR}/atlanta"                                            \
        -file       "${rankdir}/${rankname}.bench"                                  \
        -jobconf    mapreduce.job.maps=5
    RESULT=$?

    "${HADOOP}/hdfs" dfs -get   "/${team}-${scenename}-output"  "${rankdir}/output"


}



function    show_help()
{
    echo    "Usage:"
    echo    "   rank.sh <TEAM> <INOUT-SCENE-DIR> [<OUTPUT-DIR>]"
}


function main()
{
    if [[ $# -lt 1 ]]; then
        echo    "Error: Missing parameters, type -h for help"
        return  1
    fi

    if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]; then
        show_help   "$@"
        return  2
    fi

    if [[ $# -lt 2 ]]; then
        echo    "Error: Missing parameters, type -h for help"
        return  3
    fi


    #    读取参数
    local team="$1"
    local inputdir=$(realpath "$2")
    local outputdir=$(realpath "$3")


    #   对输入参数进行强校验: team
    if [[ ! "${team}" =~ [A-Za-z][A-Za-z0-9]* ]]; then
        echo    "Error: The name of the team is invalid: '${team}' is not match with '[A-Za-z][A-Za-z0-9]*'"
        return  4
    fi


    #   对输入参数进行强校验: inputdir
    if [[ ! -d "${inputdir}" ]]; then
        echo    "Error: Can not access the input dir of current scene: '${inputdir}'"
        return  5
    fi


    #   对输入参数进行强校验: outputdir
    if [[ ! -d "${outputdir}" ]]; then
        mkdir -p    "${outputdir}"
    fi


    #   准备环境
    echo    "Starting testing environment ..."
    prepare_env
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Prepare the testing environment failed(${RESULT})"
        return  7
    fi
    echo    "Start testing environment success"

    #   启动hadoop
    echo    "Starting hadoop ..."
    start_hadoop
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Start hadoop failed(${RESULT})"
        return  7
    fi
    echo    "Start hadoop success"


    #   启动redis
    echo    "Starting redis ..."
    start_redis
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Start redis failed(${RESULT})"
        return  7
    fi
    echo    "Start redis success"

    return  111


    #   启动测试
    echo    "Testing ..."
    execute_rank    "${team}"   "${inputdir}"   "${outputdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Testing failed(${RESULT})"
        return  7
    fi
    echo    "Error: Testing completed"


    return  0
}


main    "$@"
exit    "$?"
