#!/usr/bin/env bash

SELFDIR=$(dirname $(realpath "${BASH_SOURCE[0]}"))

export  HADOOP_HOME="${SELFDIR}/hadoop"
export  PATH="${PATH}:${HADOOP_HOME}/bin:${HADOOP_HOME}/sbin"

templateFiles=$(cd "${HADOOP_HOME}/etc/hadoop" && ls)

for f in ${templateFiles} ; do
    sed "s#\${HADOOP_HOME}#${HADOOP_HOME}#g"    -i  "${HADOOP_HOME}/etc/hadoop/${f}"    &&  \
    sed "s#\${JAVA_HOME}#${JAVA_HOME}#g"        -i  "${HADOOP_HOME}/etc/hadoop/${f}"
done

if [[ ! -d "${HADOOP_HOME}/data" ]]; then
    "${HADOOP_HOME}/bin/hdfs"   namenode    -format
fi

