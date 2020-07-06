#!/usr/bin/env bash


SELFDIR=$(dirname $(realpath "$0"))


PROJECT_ROOT="${SELFDIR}"
while [[ ! -f "${PROJECT_ROOT}/PROJECT" ]]; do
    if [[ "" == "${PROJECT_ROOT}" ]]; then
        exit  1
    fi

    if [[ "/" == "${PROJECT_ROOT}" ]]; then
        exit  2
    fi

    PROJECT_ROOT=$(dirname "${PROJECT_ROOT}")
done


function help()
{
    echo  "Usage:"
    echo  "   build.sh  [compile] [build|release]"
    echo  "   build.sh  format"

    return  0
}


# $1  action
# $2+ params
function compile()
{
    local cmakeopts=""
    local filter=$(uname -s)
    case "${filter}" in
        Linux*)
            cmakeopts="-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"
        ;;
        Darwin*)
            cmakeopts="-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++"
        ;;
        *)
            echo    "Unknown system: '${filter}'"
            return  1
        ;;
    esac


    local buildmode=""
    if   [[ "$2" == "debug" ]] || [[ "$2" == "" ]]; then
        buildmode="debug"
        cmakeopts="${cmakeopts} -DCMAKE_BUILD_TYPE=Debug"
        cmakegens="CodeBlocks - Unix Makefiles"
    elif [[ "$2" == "release" ]]; then
        buildmode="release"
        cmakeopts="${cmakeopts} -DCMAKE_BUILD_TYPE=Release"
        cmakegens="CodeBlocks - Unix Makefiles"
    else
        echo    "Unsupported compile mode '$2'"
        return  2
    fi


    local builddir="${PROJECT_ROOT}/cmake-build/${buildmode}"
    mkdir -p "${builddir}"
    cd    "${builddir}" && cmake ${cmakeopts} -G "${cmakegens}" "${PROJECT_ROOT}/src/hiatpg"  &&  \
    make clean  &&  \
    make
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Build project failed(${RESULT})"
        return  3
    fi
    echo    "Build project success"


    return  0
}


function format()
{
    local cppfiles=$(cd "${PROJECT_ROOT}" && find "${PROJECT_ROOT}/src" -name '*.cpp' | grep -v -E '.*-build.*' | grep -v -E 'CMakeFiles')
    local hppfiles=$(cd "${PROJECT_ROOT}" && find "${PROJECT_ROOT}/src" -name '*.h'   | grep -v -E '.*-build.*' | grep -v -E 'CMakeFiles')
    local fmtfiles="${cppfiles} ${hppfiles}"
    for f in ${fmtfiles}; do
        cd "${PROJECT_ROOT}" && clang-format --style=file -i "${f}"
        RESULT=$?
        if [[ ${RESULT} -ne 0 ]]; then
            echo  "Format Fail: ${f}"
        else
            echo  "Format OK  : ${f}"
        fi
    done

    return  0
}


# $1  action
# $2+ params
function main()
{
    local action="$1"

    if [[ "${action}" == "help" ]] || [[ "${action}" == "--help" ]] || [[ "${action}" == "-h" ]]; then
        help    "$@"
        return  "$?"
    fi

    if [[ "${action}" == "debug" ]] || [[ "${action}" == "release" ]] || [[ "${action}" == "" ]]; then
        compile "compile" "$@"
        return  $?
    fi

    if [[ "${action}" == "compile" ]]; then
        compile "$@"
        return  "$?"
    fi

    if [[ "${action}" == "format" ]]; then
        format  "$@"
        return  "$?"
    fi

    echo  "Unsupported action '${action}', type 'help' for help"
    return  $?
}


main  "$@"
exit  "$?"
