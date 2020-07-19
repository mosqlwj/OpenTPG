#!/usr/bin/env bash

#   <SOFTWARE-DIR>
#
#   <INSTALL-DIR>
#       +-- hadoop
#           +-- bin
#           +-- etc

export SOFTWARE_DIR=""
export INSTALL_DIR=""


SELFDIR=$(dirname $(realpath "${BASH_SOURCE[0]}"))


function show_help()
{
    echo    "Usage:"
    echo    "   install.sh <SOFTWARE-DIR> <INSTALL-DIR>"

    return  0
}


function check_java()
{
    if [[ "${JAVA_HOME}" == "" ]]; then
        echo    "\${JAVA_HOME} should be setup at first"
        return  1
    fi

    if [[ ! -d "${JAVA_HOME}" ]]; then
        echo    "\${JAVA_HOME} is not exist or accessable: '${JAVA_HOME}'"
        return  2
    fi

    local java_version=$(java -version 2>&1 | grep 'version' | awk '{print $3}' | sed 's/"//g')
    if [[ $(echo "${java_version}" | sed 's/_[0-9]*//g') != '1.8.0' ]]; then
        echo    "JDK is installed but version is not matched:"
        echo    "    JAVA_HOME is '${JAVA_HOME}'"
        echo    "    jdk-version is '${java_version}'"
        return  3
    fi

    return  0
}

function clean_hadoop()
{
    #   清除 install 目录下的所有内容
    rm -rf   "${INSTALL_DIR}"/hadoop
    if [[ -d "${INSTALL_DIR}"/hadoop ]]; then
        echo    "Clean installed hadoop failed: '${INSTALL_DIR}'"
        return  6
    fi

    return  0
}

function install_hadoop()
{
    export HADOOP_HOME="${INSTALL_DIR}/hadoop"

    #   找到 hadoop 的安装包,并解压安装
    local hadoop_package=$(cd "${SOFTWARE_DIR}" && ls hadoop-3.2.1.tar* | head -n 1)
    mkdir -p    "${INSTALL_DIR}"                                        &&  \
    cd          "${INSTALL_DIR}"                                        &&  \
    tar xvfz    "${SOFTWARE_DIR}/${hadoop_package}"                     &&  \
    mv          "${INSTALL_DIR}/hadoop-3.2.1"   "${HADOOP_HOME}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Install hadoop failed(${RESULT}): '${SOFTWARE_DIR}/${hadoop_package}' -> '${INSTALL_DIR}'"
        return  1
    fi

    #   安装配置和入口配置脚本
    cp  -rf "${SELFDIR}/tmpl-hadoop"/*    "${HADOOP_HOME}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Setup configurations of hadoop failed(${RESULT}): '${SELFDIR}/tmpl-hadoop' -> '${HADOOP_HOME}'"
        return  1
    fi
    echo    "Setup configurations of hadoop success: '${SELFDIR}/tmpl-hadoop' -> '${HADOOP_HOME}'"


    echo    "Install hadoop success: '${HADOOP_HOME}'"
    return  0
}

function clean_redis()
{
    #   清除 install 目录下的所有内容
    rm -rf   "${INSTALL_DIR}"/redis
    if [[ -d "${INSTALL_DIR}"/redis ]]; then
        echo    "Clean installed redis failed: '${INSTALL_DIR}'"
        return  6
    fi

    return  0
}

function install_redis()
{
    export REDIS_HOME="${INSTALL_DIR}/redis"
    mkdir -p    "${REDIS_HOME}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Error: Create installation direcory for redis failed(${RESULT})"
        return  7
    fi

    #   找到 redis 的安装包,并解压安装
    local redis_package=$(cd "${SOFTWARE_DIR}" && find -name redis-6.0.5.tar* | head -n 1)
    mkdir -p    "${INSTALL_DIR}"                                        &&  \
    cd          "${INSTALL_DIR}"                                        &&  \
    tar xvfz    "${SOFTWARE_DIR}/${redis_package}"                      &&  \
    cd          "${INSTALL_DIR}/redis-6.0.5"                            &&  \
    make                                                                &&  \
    make        "PREFIX=/${REDIS_HOME}"  install
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Install redis failed(${RESULT}): '${SOFTWARE_DIR}/${redis_package}' -> '${INSTALL_DIR}'"
        return  1
    fi

    #   清除构建环境下的所有的东西
    rm -f       "${REDIS_HOME}/redis-6.0.5"

    #   安装配置和入口配置脚本
    cp  -rf "${SELFDIR}/tmpl-redis"/*       "${REDIS_HOME}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Setup configurations of redis failed(${RESULT}): '${SELFDIR}/tmpl-redis' -> '${REDIS_HOME}'"
        return  1
    fi
    echo    "Setup configurations of redis success: '${SELFDIR}/tmpl-redis' -> '${REDIS_HOME}'"


    echo    "Install redis success: '${REDIS_HOME}'"
    return  0
}

function clean_settings()
{
    return  0
}

function install_settings()
{
    if [[ ! -f "${INSTALL_DIR}/settings.sh" ]]; then
        cp  -rf "${SELFDIR}/settings.sh"    "${INSTALL_DIR}"
    fi
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Setup configurations of hadoop failed(${RESULT}): '${SELFDIR}/etc' -> '${INSTALL_DIR}/hadoop/etc'"
        return  1
    fi

    return  0
}

#   $1  software-dir
#   $2  install-dir
function main()
{
    if [[ $# -lt 1 ]]; then
        echo    "Missing paremeters, type -h for help."
        return  1
    fi

    if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]; then
        show_help   "$@"
        return      "$?"
    fi

    if [[ "$#" -lt 2 ]]; then
        echo    "Missing paremeters, type -h for help."
        return  2
    fi

    SOFTWARE_DIR="$1"
    if [[ ! -d "${SOFTWARE_DIR}" ]]; then
        echo    "Can not found or access the software directory '${SOFTWARE_DIR}'"
        return  4
    fi
    SOFTWARE_DIR=$(realpath "${SOFTWARE_DIR}")

    INSTALL_DIR="$2"
    if [[ "${INSTALL_DIR}" == "" ]]; then
        echo    "Missing parameter of install dir, type -h for help"
        return  5
    fi
    INSTALL_DIR=$(realpath "${INSTALL_DIR}")

    echo    "SOFTWARE_DIR   :   '${SOFTWARE_DIR}'"
    echo    "INSTALL_DIR    :   '${INSTALL_DIR}'"

    #   检查 java 运行环境
    check_java
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        return  7
    fi

    #   安装 hadoop
    clean_hadoop && install_hadoop
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "The hadoop installation was failed(${RESULT})"
        return  8
    fi
    echo    "The hadoop installation was success"

    #   安装 redis
    clean_redis && install_redis
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "The redis installation was failed(${RESULT})"
        return  8
    fi
    echo    "The redis installation was success"

    #   安装 settings
    clean_settings && install_settings
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "The settings installation was failed(${RESULT})"
        return  8
    fi
    echo    "The settings installation was success"

    return 0
}

main  "$@"
exit  "$?"

