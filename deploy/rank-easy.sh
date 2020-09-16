#!/usr/bin/env bash


#SELFDIR=$(dirname $(realpath "$BASH_SOURCE"))
SELFDIR=$(cd $(dirname "$BASH_SOURCE");pwd)
if [[ ${SELFDIR} == "" ]]; then
    echo "Internal error: can not locate the script dir"
    exit 1
fi

##################################################################
function error()
{
    echo    -e  "\033[41;37m$@\033[0m"
}

##################################################################
function notice()
{
    echo    -e  "\033[42;37m$@\033[0m"
}

##################################################################
function warning()
{
    echo    -e  "\033[42;33m$@\033[0m"
}

##################################################################
function    show_help()
{
    echo    "Usage:"
    echo    "   rank-easy.sh <TEAM> <INPUT-NETLIST-FILE> [<OUTPUT-DIR>]"
    echo    ""
    echo    "Options:"
    echo    "   <TEAM>                  The name of your team."
    echo    "   <INPUT-NETLIST-FILE>    The netlist file."
    echo    "   <OUTPUT-DIR>            Where to write the output files, the current working dir is the default."

    return  0
}

##################################################################
function    check_env()
{
    #   检查 java 环境变量
    if [[ "${JAVA_HOME}" == "" ]]; then
        error   "Error: \${JAVA_HOME} is missing"
        return  1
    fi

    #   检查 hadoop 环境变量
    if [[ "${HADOOP_HOME}" == "" ]] || [[ ! -d "${HADOOP_HOME}" ]]; then
        error   "Error: \${HADOOP_HOME} is missing or not accessable"
        return  2
    fi

#    #   检查 redis 环境变量
#    if [[ "${REDIS_HOME}" == "" ]] || [[ ! -d "${REDIS_HOME}" ]]; then
#        error   "Error: \${REDIS_HOME} is missing or not accessable"
#        return  3
#    fi

    #   检查 java 是否可以执行
    java -version
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        error   "Error: Can not run java on this system, maybe the '\${JAVA_HOME}/bin' is not in '\$PATH'"
        return  4
    fi

    #   检查 atalanta 进程是否已经编译出来了
    if [[ ! -f "${SELFDIR}/atalanta" ]]; then
        error   "Error: Can not access '${SELFDIR}/atalanta', maybe you need build the application firstly"
        return  5
    fi

    #   检查 atalanta-hadoop.sh 是否已经存在
    if [[ ! -f "${SELFDIR}/atalanta-hadoop.sh" ]]; then
        error   "Error: Can not access '${SELFDIR}/atalanta-hadoop.sh'"
        return  6
    fi

    return  0
}

##################################################################
#   准备环境
function    prepare_env()
{
    #   修改可执行权限
    chmod +x    "${SELFDIR}/atalanta"
    chmod +x    "${SELFDIR}/atalanta-hadoop.sh"

    return  0
}

##################################################################
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
            error   "Error: Can not access the bench file at: '${benchfile}'"
            return  6
        fi
    fi
    if [[ ! -f "${benchfile}" ]]; then
        error   "Error: Can not access the bench file at: '${benchfile}'"
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
            error   "Error: Can not the exist output directory: '${rankdir}'"
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
        error   "Backup the execute file failed(${RESULT}): '${SELFDIR}/atalanta' -> '${rankdir}'"
        return  5
    fi
    echo    "Backup the execute file success: '${SELFDIR}/atalanta' -> '${rankdir}'"


    #   将bench文件拷贝过来
    echo    "Backup the netlist file..."
    cp  -f  "${benchfile}"  "${rankdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        error   "Backup netlist file failed(${RESULT}): '${benchfile}' -> '${rankdir}'"
        return  5
    fi
    echo    "Backup the netlist success: '${benchfile}' -> '${rankdir}'"


    #   先在本地生成 faultlist
    echo    "Create the fault-list file..."
    local faultfile="${rankdir}/${rankname}.fault"
    "${SELFDIR}/atalanta"   --exec      "create-fault" --netlist   "${rankdir}/${rankname}.bench"  >  "${faultfile}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        error   "Create fault-list for bench failed(${RESULT}): '${rankdir}/${rankname}.bench' -> '${faultfile}'"
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

##################################################################
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
    "${SELFDIR}/atalanta-hadoop.sh" "${team}" "${rankdir}" "${rankname}"
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


##################################################################
#   $1  team
#   $2  rankdir
#   $3  rankname
function rank_local()
{
    local team="$1"
    local rankdir="$2"
    local rankname="$3"

    local   starttime=$(date +'%s')
    "${SELFDIR}/atalanta-local.sh" "${team}" "${rankdir}" "${rankname}"
    RESULT=$?
    local   endtime=$(date +'%s')

    #   生成时间统计信息
    local   execintv=$((endtime - starttime))
    echo    "${execintv}"  > "${rankdir}/${rankname}.cost"

    return  0;
}


##################################################################
function    main()
{
    if [[ $# -lt 1 ]]; then
        error   "Error: Missing parameters, type -h for help"
        return  1
    fi

    if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]; then
        show_help   "$@"
        return  2
    fi

    if [[ $# -lt 2 ]]; then
        error   "Error: Missing parameters, type -h for help"
        return  3
    fi

    #   检查测试环境
    check_env   "$@"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        #   如果环境检查失败，需要终止执行
        return  ${RESULT}
    fi

    #    读取参数
    local team="$1"
    local inputdir="$2"
    local outputdir="$3"
    if [[ "${outputdir}" == "" ]]; then
        outputdir="."
    fi

    if [[ "${RANK_MODE}" == "" ]]; then
        export RANK_MODE="hadoop"
    fi

    if [[ "${RANK_MODE}" != "local" ]] && [[ "${RANK_MODE}" != "hadoop" ]]; then
        error   "Error: Unsupported value of the '\${RANK_MODE}' : '${RANK_MODE}'"
        return  7
    fi

    #   对输入参数进行强校验: team
    if [[ ! "${team}" =~ [A-Za-z][A-Za-z0-9]* ]]; then
        error   "Error: The name of the team is invalid: '${team}' is not match with '[A-Za-z][A-Za-z0-9]*'"
        return  8
    fi

    #   对输入参数进行强校验: inputdir
    if [[ ! -e "${inputdir}" ]]; then
        error   "Error: Can not access the input dir of current scene: '${inputdir}'"
        return  9
    fi

    #   对输入参数进行强校验: outputdir
    if [[ ! -d "${outputdir}" ]]; then
        mkdir -p    "${outputdir}"
    fi

    #   准备测试环境
    echo    "Prepare testing environment ..."
    prepare_env
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        error   "Error: Prepare the testing environment failed(${RESULT})"
        return  10
    fi
    echo    "Prepare testing environment success"

    #   启动测试
    echo    "Testing ..."
    execute_rank    "${team}"   "${inputdir}"   "${outputdir}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        error   "Error: Testing failed(${RESULT})"
        return  11
    fi
    echo    "Testing completed"

    return  0
}


##################################################################
main    "$@"
exit    "$?"

