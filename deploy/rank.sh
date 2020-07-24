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

    nohup "${REDIS_HOME}/bin/redis-server" "${REDIS_HOME}/conf/redis.conf" &
#    nohup "${REDIS_HOME}/bin/redis-server" "${REDIS_HOME}/conf/redis.conf"  2>&1 > "${REDIS_HOME}/log/redis-server.log" < /dev/null &
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



#   $1  team
#   $2  inputdir
#   $3  outputdir
function    execute_rank()
{
    local team="$1"
    local inputdir="$2"
    local outputdir="$3"
    local timestamp=$(date '+%Y%m%d%H%M%S')


    #   找到作为输入的bench文件
    echo    "Locate the netlist file..."
    local benchfile=${inputdir}
    if [[ -d "${inputdir}" ]]; then
        benchfile=$(find "${inputdir}" -name '*.bench' | head -n 1)
        if [[ "${benchfile}" == "" ]]; then
            echo    "Error: Can not access the bench file at: '${benchfile}'"
            return  6
        fi
    fi
    if [[ ! -f "${benchfile}" ]]; then
        echo    "Error: Can not access the bench file at: '${benchfile}'"
        return  6
    fi
    local rankname=$(basename "${benchfile}" | sed 's/.bench//g')
    echo    "Locate the netlist file success: ${benchfile}"


    #   清理下工作目录
    echo    "Creating the output directory..."
    local   rankdir="${outputdir}/${team}@${rankname}@${RANK_MODE}"
    if [[ -d "${rankdir}" ]]; then
        rm -rf "${rankdir}"
        if [[ -d "${rankdir}" ]]; then
            echo    "Error: Can not the exist output directory: '${rankdir}'"
            return  5
        fi
    fi
    mkdir -p    "${rankdir}"
    echo    "Creating the output directory success: '${rankdir}'"


    #   将核心文件拷贝过来
    echo    "Backup the execute file..."
    cp  -f  "${SELFDIR}/atalanta"  "${rankdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Backup the execute file failed(${RESULT}): '${SELFDIR}/atalanta' -> '${rankdir}'"
        return  5
    fi
    echo    "Backup the execute file success: '${SELFDIR}/atalanta' -> '${rankdir}'"


    #   将bench文件拷贝过来
    echo    "Backup the netlist file..."
    cp  -f  "${benchfile}"  "${rankdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Backup netlist file failed(${RESULT}): '${benchfile}' -> '${rankdir}'"
        return  5
    fi
    echo    "Backup the netlist success: '${benchfile}' -> '${rankdir}'"


    #   先再本地生成 faultlist
    echo    "Create the fault-list file..."
    local faultfile="${rankdir}/${rankname}.fault"
    "${SELFDIR}/atalanta"   --exec      "create-fault" --netlist   "${rankdir}/${rankname}.bench"  >  "${faultfile}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Create fault-list for bench failed(${RESULT}): '${rankdir}/${rankname}.bench' -> '${faultfile}'"
        return  5
    fi
    echo    "Create the fault-list file success: '${rankdir}/${rankname}.bench' -> '${faultfile}'"


    #   使用指定的模式来执行 TPG 流程
    rank_${RANK_MODE} "${team}" "${rankdir}" "${rankname}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Execute TPG flow by '${RANK_MODE}'"
    fi


    #   生成统计报告
    local   reportfile="${rankdir}/${rankname}.report"
    local   costtime=$(cat "${rankdir}/${rankname}.cost")
    echo    "timestamp      :   ${timestamp}"           >>  "${reportfile}"
    echo    "team           :   ${team}"                >>  "${reportfile}"
    echo    "mode           :   ${RANK_MODE}"           >>  "${reportfile}"
    echo    "bench-file     :   ${benchfile}"           >>  "${reportfile}"
    echo    "pattern-file   :   ${rankname}.pattern"    >>  "${reportfile}"
    echo    "--"                                        >>  "${reportfile}"
    echo    "pattern-count  :   ???"                    >>  "${reportfile}"
    echo    "coverage       :   ???"                    >>  "${reportfile}"
    echo    "cost-time      :   ${costtime} s"          >>  "${reportfile}"


    return  0
}


#   $1  team
#   $2  rankdir
#   $3  rankname
function rank_hadoop()
{
    local team="$1"
    local rankdir="$2"
    local rankname="$3"


    #   清理旧的输入和输出目录
    local dfsinputdir="/${team}-${rankname}-input"
    local dfsoutputdir="/${team}-${rankname}-output"
    echo    "Clear the input and output directory in DFS..."
    "${HADOOP_HOME}/bin/hdfs" dfs -rm -r -f "${dfsinputdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Clear input directory failed(${RESULT}): '${dfsinputdir}'"
    fi
    "${HADOOP_HOME}/bin/hdfs" dfs -rm -r -f "${dfsoutputdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Clear output directory failed(${RESULT}): '${dfsoutputdir}'"
    fi
    echo    "Clear the input and output directory in DFS complete"


    #   创建输入目录
    echo    "Create input directory for dfs..."
    "${HADOOP_HOME}/bin/hdfs" dfs -mkdir "${dfsinputdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Create input directory of dfs failed(${RESULT}): '${dfsinputdir}"
        return  5
    fi
    echo    "Create input directory of dfs success: '${dfsinputdir}'"


    #   将前面生成的faultlist文件放入输入目录
    echo    "Deploy fault-list file to dfs..."
    "${HADOOP_HOME}/bin/hdfs" dfs -put   "${rankdir}/${rankname}.fault"  "${dfsinputdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Put the fault list file to dfs failed(${RESULT}): '${rankdir}/${rankname}.fault' -> '${dfsinputdir}'"
        return  5
    fi
    echo    "Deploy fault-list file to dfs success: '${dfsinputdir}'"

    #   执行 TPG 流程
    echo    "Executing TPG-FLOW..."
    local   streamfile="$HADOOP_HOME/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar"
    local   starttime=$(date +'%s')
    "$HADOOP_HOME/bin/hadoop" jar "${streamfile}"                                   \
        -input      "/${team}-${rankname}-input"                                    \
        -output     "/${team}-${rankname}-output"                                   \
        -mapper     "atalanta --exec atpg          --netlist  ${rankname}.bench"    \
        -reducer    "atalanta --exec simulate-cube --netlist  ${rankname}.bench"    \
        -file       "${SELFDIR}/atalanta"                                           \
        -file       "${rankdir}/${rankname}.bench"
    RESULT=$?
    local   endtime=$(date +'%s')
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Executing TPG-FLOW failed(${RESULT})"
        return  5
    fi
    echo    "Executing TPG-FLOW success: escape=${execintv}s"


    #   下载输出结果
    echo    "Download the outputs..."
    "${HADOOP_HOME}/bin/hdfs" dfs -get   "${dfsoutputdir}"  "${rankdir}/output"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Download the outputs failed(${RESULT})"
        return  5
    fi
    echo    "Download the outputs success"

    #   合并pattern文件
    local   execintv=$((endtime - starttime))
    cat     "${rankdir}/output/part-00000"      >       "${rankdir}/${rankname}.pattern"
    echo    "${execintv}"                       >       "${rankdir}/${rankname}.cost"

    return  0
}



#   $1  team
#   $2  scenename
#   $3  rankdir
#   $4  rankname
function rank_local()
{
    local team="$1"
    local rankdir="$2"
    local rankname="$3"

    local   benchfile="${rankdir}/${rankname}.bench"
    local   starttime=$(date +'%s')
    cat     "${rankdir}/${rankname}.fault"                                  | \
    "${SELFDIR}/atalanta" --exec atpg           --netlist "${benchfile}"    | \
    "${SELFDIR}/atalanta" --exec simulate-cube  --netlist "${benchfile}"    >   "${rankdir}/${rankname}.pattern"
    local   endtime=$(date +'%s')

    #   生成时间统计信息
    local   execintv=$((endtime - starttime))
    echo    "${execintv}"  > "${rankdir}/${rankname}.cost"

    return  0;
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


    if [[ "${RANK_MODE}" == "" ]]; then
        export RANK_MODE="hadoop"
    fi

    if [[ "${RANK_MODE}" != "local" ]] && [[ "${RANK_MODE}" != "hadoop" ]]; then
        echo    "Error: Unsupported value of the '\${RANK_MODE}' : '${RANK_MODE}'"
        return  10
    fi


    #   对输入参数进行强校验: team
    if [[ ! "${team}" =~ [A-Za-z][A-Za-z0-9]* ]]; then
        echo    "Error: The name of the team is invalid: '${team}' is not match with '[A-Za-z][A-Za-z0-9]*'"
        return  4
    fi


    #   对输入参数进行强校验: inputdir
    if [[ ! -e "${inputdir}" ]]; then
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


#    #   启动redis
#    echo    "Starting redis ..."
#    start_redis
#    RESULT=$?
#    if [[ ${RESULT} -ne 0 ]]; then
#        echo    "Error: Start redis failed(${RESULT})"
#        return  7
#    fi
#    echo    "Start redis success"


    #   启动hadoop
    echo    "Starting hadoop ..."
    start_hadoop
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Start hadoop failed(${RESULT})"
        return  7
    fi
    echo    "Start hadoop success"


    #   启动测试
    echo    "Testing ..."
    execute_rank    "${team}"   "${inputdir}"   "${outputdir}"  "${RANK_MODE}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Testing failed(${RESULT})"
        return  7
    fi
    echo    "Testing completed"


    return  0
}


echo    "----------------"
main    "$@"
exit    "$?"
