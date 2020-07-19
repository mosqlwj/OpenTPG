#!/usr/bin/env bash
#set -x

SELFDIR=$(dirname $(realpath "${BASH_SOURCE[0]}"))


export  HADOOP_HOME="${SELFDIR}/hadoop"
export  REDIS_HOME="${SELFDIR}/redis"
export  PATH="${PATH}:${HADOOP_HOME}/bin:${HADOOP_HOME}/sbin:${SELFDIR}/redis/bin"


function config_hadoop()
{
    local templateFiles=$(cd "${HADOOP_HOME}/etc/hadoop" && ls)
    for f in ${templateFiles} ; do
        if [[ -d "${HADOOP_HOME}/etc/hadoop/${f}" ]]; then
            continue
        fi
        sed "s#\${HADOOP_HOME}#${HADOOP_HOME}#g"    -i  "${HADOOP_HOME}/etc/hadoop/${f}"    &&  \
        sed "s#\${JAVA_HOME}#${JAVA_HOME}#g"        -i  "${HADOOP_HOME}/etc/hadoop/${f}"
    done

    if [[ ! -d "${HADOOP_HOME}/data" ]]; then
        "${HADOOP_HOME}/bin/hdfs"   namenode    -format
    fi

    echo    "Setting up hadoop success"
    return  0
}


function config_redis()
{
    echo    "Setting up regis success"
    return  0
}


config_hadoop && config_redis
