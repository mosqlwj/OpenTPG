#!/usr/bin/env bash

SELFDIR=$(
    cd $(dirname "${BASH_SOURCE[0]}")
    pwd
)

# 定位 MODULE 文件
export MODULE_ROOT="${SELFDIR}"
while [[ 0 -eq 0 ]]; do
    if [[ "${MODULE_ROOT}" == "" || "${MODULE_ROOT}" == "/" ]]; then
        echo "Can not resolve the \${MODULE_ROOT} from ${SELFDIR}"
        exit 1
    fi

    if [[ -f "${MODULE_ROOT}/MODULE" ]]; then
        break
    fi

    MODULE_ROOT=$(dirname "${MODULE_ROOT}")
done

# 定位 PROJECT 文件
export PROJECT_ROOT="${MODULE_ROOT}"
while [[ 0 -eq 0 ]]; do
    if [[ "${PROJECT_ROOT}" == "" || "${PROJECT_ROOT}" == "/" ]]; then
        echo "Can not resolve the \${PROJECT_ROOT} from ${MODULE_ROOT}"
        exit 1
    fi

    if [[ -f "${PROJECT_ROOT}/PROJECT" ]]; then
        break
    fi

    PROJECT_ROOT=$(dirname "${PROJECT_ROOT}")
done

# 加载 MODULE 和 PROJECT 文件[注意加载顺序]
source "${PROJECT_ROOT}/PROJECT"
source "${MODULE_ROOT}/MODULE"

# 帮助
function build_help() {
    echo "Usage:"
    echo "   build.sh [compile]"
    echo "   build.sh clean"
    echo "   build.sh format"
    return 0
}

# 编译
function build_compile() {
    rm -rf "${MODULE_ROOT}/build"
    mkdir -p "${MODULE_ROOT}/build"

    cd "${MODULE_ROOT}/build" &&
        cmake ../ &&
        make -j 4 &&
        mv -f "${MODULE_ROOT}/build/${MODULE_NAME}" "${MODULE_ROOT}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo "Build ${MODULE_NAME}-${MODULE_VERSION} failed"
        return 1
    fi

    echo "Build ${MODULE_NAME}-${MODULE_VERSION} success"

    return 0
}

# 清理
function build_clean() {
    echo "Clean : ${MODULE_ROOT}/build"
    rm -rf "${MODULE_ROOT}/build"

    echo "Clean : ${MODULE_ROOT}/bench/*.fault"
    rm -rf "${MODULE_ROOT}/bench"/*.fault

    echo "Clean : ${MODULE_ROOT}/bench/*.gate"
    rm -rf "${MODULE_ROOT}/bench"/*.gate

    echo "Clean : ${MODULE_ROOT}/bench/*.cube"
    rm -rf "${MODULE_ROOT}/bench"/*.cube

    return 0
}

# 代码格式化
function build_format() {
    local cpplist=$(find "${MODULE_ROOT}/src" -name '*.cpp')
    local hlist=$(find "${MODULE_ROOT}/src" -name '*.h')
    local alllist="${cpplist} ${hlist}"
    for f in ${alllist}; do
        clang-format -style=file -i "${f}"
        RESULT=$?
        if [[ ${RESULT} -eq 0 ]]; then
            echo    "[ OK ] ${f}"
        else
            echo    "[FAIL] ${f}"
        fi
    done

    return 0
}

# 入口函数
function main() {
    local action="$1"
    if [[ $# -lt 1 ]]; then
        action="compile"
    fi

    case "${action}" in
    help | -h | --help)
        build_help "$@"
        return $?
        ;;
    compile)
        build_compile "$@"
        return $?
        ;;
    clean)
        build_clean "$@"
        return $?
        ;;
    format)
        build_format "$@"
        return $?
        ;;
    esac

    echo "Unsupported command '${action}'"
    return 1
}

main "$@"
exit "$?"
