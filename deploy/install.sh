#!/usr/bin/env bash

#   <INSTALL-DIR>
#       +-- java
#       +-- hadoop
#           +-- bin
#           +-- etc
export PACKAGE_FILE=""
export SOFTWARE_DIR=""
export INSTALL_DIR=""


SELFDIR=$(dirname $(realpath "${BASH_SOURCE[0]}"))


function show_help()
{
    echo    ""
}


function install_java()
{
    if [[ -d "${JAVA_HOME}" ]]; then
        echo    "Java is installed: '${JAVA_HOME}'"
        local java_version=$(java -version 2>&1 | grep 'java version' | awk '{print $3}' | sed 's/"//g')
        if [[ "${java_version}" == '1.8.0_251' ]]; then
            echo    "Java is installed and version is matched: \$JAVA_HOME=${JAVA_HOME}"
            return  0
        fi
    fi

    local java_package=$(cd "${SOFTWARE_DIR}" | ls jdk-8u251-* | head -n 1)
    mkdir -p    "${INSTALL_DIR}"                                        &&  \
    cd          "${INSTALL_DIR}"                                        &&  \
    tar xvfz    "${SOFTWARE_DIR}/${java_package}"                       &&  \
    mv          "${INSTALL_DIR}/jdk1.8.0_251"   "${INSTALL_DIR}/java"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Install java failed(${RESULT}): '${SOFTWARE_DIR}/${java_package}' -> '${INSTALL_DIR}'"
        return  1
    fi

    echo    "Java install here: '${INSTALL_DIR}/java'"
    return  0
}


function install_hadoop()
{
    local hadoop_package=$(cd ${SOFTWARE_DIR} | ls hadoop-3.2.1* | head -n 1)
    mkdir -p    "${INSTALL_DIR}"                                        &&  \
    cd          "${INSTALL_DIR}"                                        &&  \
    tar xvfz    "${SOFTWARE_DIR}/${hadoop_package}"                     &&  \
    mv          "${INSTALL_DIR}/hadoop-3.2.1"   "${INSTALL_DIR}/hadoop"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Install hadoop failed(${RESULT}): '${SOFTWARE_DIR}/${hadoop_package}' -> '${INSTALL_DIR}'"
        return  1
    fi

    echo    "Install hadoop success: '${INSTALL_DIR}/java'"
    return  0
}


#   $1  package-file
#   $2  software-dir
#   $3  install-dir
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

    if [[ "$#" -lt 3 ]]; then
        echo    "Missing paremeters, type -h for help."
        return  2
    fi

    PACKAGE_FILE="$1"
    if [[ ! -f "${PACKAGE_FILE}" ]]; then
        echo    "Can not found or access the package file '${PACKAGE_FILE}'"
        return  4
    fi

    SOFTWARE_DIR="$2"
    if [[ ! -d "${SOFTWARE_DIR}" ]]; then
        echo    "Can not found or access the software directory '${SOFTWARE_DIR}'"
        return  4
    fi

    INSTALL_DIR="$3"
    if [[ "${INSTALL_DIR}" == "" ]]; then
        echo    "Missing parameter of install dir, type -h for help"
        return  5
    fi

    #   清楚 install 目录下的所有内容
    rm -f   "${INSTALL_DIR}"
    if [[ -d "${INSTALL_DIR}" ]]; then
        echo    "Clean install dir failed: '${INSTALL_DIR}'"
        return  6
    fi

    echo    "PACKAGE_FILE   :   '${PACKAGE_FILE}'"
    echo    "SOFTWARE_DIR   :   '${SOFTWARE_DIR}'"
    echo    "INSTALL_DIR    :   '${INSTALL_DIR}'"

    install_java
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Java installation was failed(${RESULT})"
        return  7
    fi
    echo    "Java installation was success"

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

