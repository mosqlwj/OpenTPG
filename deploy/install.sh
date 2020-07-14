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


function install_hadoop()
{
    #   找到 hadoop 的安装包,并解压安装
    local hadoop_package=$(cd "${SOFTWARE_DIR}" && ls hadoop-3.2.1.tar* | head -n 1)
    mkdir -p    "${INSTALL_DIR}"                                        &&  \
    cd          "${INSTALL_DIR}"                                        &&  \
    tar xvfz    "${SOFTWARE_DIR}/${hadoop_package}"                     &&  \
    mv          "${INSTALL_DIR}/hadoop-3.2.1"   "${INSTALL_DIR}/hadoop"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Install hadoop failed(${RESULT}): '${SOFTWARE_DIR}/${hadoop_package}' -> '${INSTALL_DIR}'"
        return  1
    fi

    #   安装配置和入口配置脚本
    cp  -rf "${SELFDIR}/etc/"/*         "${INSTALL_DIR}/hadoop/etc" &&  \
    cp  -rf "${SELFDIR}/settings.sh"    "${INSTALL_DIR}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Setup configurations of hadoop failed(${RESULT}): '${SELFDIR}/etc' -> '${INSTALL_DIR}/hadoop/etc'"
        return  1
    fi
    echo    "Setup configurations of hadoop success: '${SELFDIR}/etc' -> '${INSTALL_DIR}/hadoop/etc'"


    export HADOOP_HOME="${INSTALL_DIR}/hadoop"

    echo    "Install hadoop success: '${INSTALL_DIR}/java'"
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

    #   清楚 install 目录下的所有内容
    rm -rf   "${INSTALL_DIR}"
    if [[ -d "${INSTALL_DIR}" ]]; then
        echo    "Clean install dir failed: '${INSTALL_DIR}'"
        return  6
    fi

    echo    "SOFTWARE_DIR   :   '${SOFTWARE_DIR}'"
    echo    "INSTALL_DIR    :   '${INSTALL_DIR}'"

    check_java
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        return  7
    fi

    install_hadoop
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Hadoop installation was failed(${RESULT})"
        return  8
    fi
    echo    "Hadoop installation was success"

    return 0
}

main  "$@"
exit  "$?"

