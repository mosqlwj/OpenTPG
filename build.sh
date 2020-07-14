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


function do_help()
{
    echo  "Usage:"
    echo  "   build.sh  [compile] [build|release]"
    echo  "   build.sh  package   [<NAME>]"
    echo  "   build.sh  format"
    echo  "   build.sh  install"

    return  0
}


# $1  action
# $2+ params
function do_compile()
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


function do_format()
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


#   $1  action
#   $2  team
function do_package
{
    local buildteam="$2"
    if [[ "${buildteam}" == "" ]]; then
        echo    "The 'package' action need a team name for the next parameter"
        return  3
    fi
    if [[ ! "${buildteam}" =~ [A-Za-z][A-Za-z0-9]* ]]; then
        echo    "The name of the team is invalid, expect the name match with '[A-Za-z][A-Za-z0-9]'"
        return  7
    fi

    #   如果竞赛团队指定了打包脚本,那么直接调用该脚本
    if [[ -f "${PROJECT_ROOT}/package.sh" ]]; then
        "${PROJECT_ROOT}/package.sh"
        return  $?
    fi

    #   如果没有指定该脚本,那么采用默认打包行为
    local buildos=$(uname -s)
    if [[ "${buildos}" == "" ]]; then
        echo    "Unknown operation system: '${buildos}'"
        return  4
    fi

    local buildarch=$(arch)
    if [[ "${buildarch}" == "" ]]; then
        echo    "Unknown arch of this system."
        return  5
    fi

    local buildtime=$(date +'%y%m%d%H%M%S')
    local buildcommitid=$(git rev-parse HEAD)
    local buildgccversion=$(gcc --version)
    local installname="hiatpg-${buildteam}"
    local pkgname="${installname}-${buildos}-${buildtime}"
    local pkgdir="${PROJECT_ROOT}/.tmp-package/${installname}"
    rm -rf      "${PROJECT_ROOT}/.tmp-package"
    rm -rf      "${PROJECT_ROOT}"/hiatpg-"${buildteam}"-"${buildos}"-*
    mkdir -p    "${pkgdir}"                                                         &&  \
    echo        "BUILD_TEAM :   '${buildteam}'"         >>  "${pkgdir}/.properties" &&  \
    echo        "BUILD_TIME :   '${buildtime}'"         >>  "${pkgdir}/.properties" &&  \
    echo        "BUILD_ARCH :   '${buildarch}'"         >>  "${pkgdir}/.properties" &&  \
    echo        "BUILD_ID   :   '${buildcommitid}'"     >>  "${pkgdir}/.properties" &&  \
    echo        "BUILD_GCC  :   '${buildgccversion}'"   >>  "${pkgdir}/.properties" &&  \
    cp -rf      "${PROJECT_ROOT}/deploy"/*      "${pkgdir}"     &&  \
    cp -rf      "${PROJECT_ROOT}/bin"           "${pkgdir}"     &&  \
    cd          "${PROJECT_ROOT}/.tmp-package"                  &&  \
    tar cvf     "${pkgname}.tar"        "${installname}"        &&  \
    gzip        "${pkgname}.tar"                                &&  \
    mv          "${pkgname}.tar.gz"     "${PROJECT_ROOT}"
    RESULT=$?
    if [[ ${RESULT} -ne 0 ]]; then
        echo    "Create package failed(${RESULT}): '${PROJECT_ROOT}/.tmp-package'"
        return  6
    fi

    rm -rf "${PROJECT_ROOT}/.tmp-package"

    echo    "Create package success: '${PROJECT_ROOT}/${pkgname}.tar.gz'"
    return  0
}

# $1  action
# $2+ params
function main()
{
    local action="$1"

    if [[ "${action}" == "help" ]] || [[ "${action}" == "--help" ]] || [[ "${action}" == "-h" ]]; then
        do_help     "$@"
        return      "$?"
    fi

    if [[ "${action}" == "debug" ]] || [[ "${action}" == "release" ]] || [[ "${action}" == "" ]]; then
        do_compile  "compile" "$@"
        return      "$?"
    fi

    if [[ "${action}" == "compile" ]]; then
        do_compile  "$@"
        return      "$?"
    fi

    if [[ "${action}" == "format" ]]; then
        do_format   "$@"
        return      "$?"
    fi

    if [[ "${action}" == "package" ]]; then
        do_package  "$@"
        return      "$?"
    fi

    echo  "Unsupported action '${action}', type 'help' for help"
    return  $?
}


main  "$@"
exit  "$?"
