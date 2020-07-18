#!/usr/bin/env bash


SELFDIR=$(dirname $(realpath "$BASH_SOURCE"))


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
    #   定义几个本函数中用到的局部变量
    local builddir=""           #   构建时的工作目录
    local buildsrc=""           #   待构建模块源代码根目录
    local buildmodule=""        #   待构建模块名
    local buildmode=""          #   构建模式
    local buildopts=""          #   构建选项


    #   确定与操作系统相关的几个参数
    case $(uname -s) in
        Linux*)
            buildopts="-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++"
        ;;
        Darwin*)
            buildopts="-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++"
        ;;
        *)
            echo    "Unknown system: '$(uname -s)'"
            return  1
        ;;
    esac


    #   确定与构建模式相关的参数
    if   [[ "$2" == "debug" ]] || [[ "$2" == "" ]]; then
        buildmode="debug"
        buildopts="${buildopts} -DCMAKE_BUILD_TYPE=Debug"
        cmakegens="CodeBlocks - Unix Makefiles"
    elif [[ "$2" == "release" ]]; then
        buildmode="release"
        buildopts="${buildopts} -DCMAKE_BUILD_TYPE=Release"
        cmakegens="CodeBlocks - Unix Makefiles"
    else
        echo    "Error: Unsupported compile mode '$2'"
        return  2
    fi


    #   执行构建
#    local module_list="atalanta hiatpg"
    local module_list="atalanta"
    for module  in  ${module_list} ; do
        #   构建 module
        buildmodule="${module}"
        buildsrc="${PROJECT_ROOT}"
        builddir="${PROJECT_ROOT}/cmake-build-${module}-${buildmode}"
        mkdir -p    "${builddir}"                                   &&  \
        cd          "${builddir}"                                   &&  \
        cmake       ${buildopts} -G "${cmakegens}" "${buildsrc}"    &&  \
        cd          "${buildsrc}"                                   &&  \
        cmake       --build "${PROJECT_ROOT}" --target ${buildmodule} -- -j 8
        RESULT=$?
        if [[ ${RESULT} -ne 0 ]]; then
            echo    "Error: Build '${buildmodule}' failed(${RESULT})"
            return  3
        fi
        echo    "Build '${buildmodule}' success"
    done


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
        echo    "Warning: Can not given the name of the team, try to read the team name from local file: '${SELFDIR}/TEAM'"
        if [[ -f "${SELFDIR}/TEAM" ]]; then
            buildteam=$(cat "${SELFDIR}/TEAM")
        fi
        if [[ "${buildteam}" == "" ]]; then
            echo    "Error: Can not resolve the team name."
            return  3
        fi
    fi
    if [[ ! "${buildteam}" =~ [A-Za-z][A-Za-z0-9]* ]]; then
        echo    "Error: The name of the team is invalid, expect the name match with '[A-Za-z][A-Za-z0-9]'"
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
        echo    "Error: Unknown operation system: '${buildos}'"
        return  4
    fi

    local buildarch=$(arch)
    if [[ "${buildarch}" == "" ]]; then
        echo    "Error: Unknown arch of this system."
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
        echo    "Error: Create package failed(${RESULT}): '${PROJECT_ROOT}/.tmp-package'"
        return  6
    fi

    rm -rf "${PROJECT_ROOT}/.tmp-package"

    echo    "Error: Create package success: '${PROJECT_ROOT}/${pkgname}.tar.gz'"
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

    echo  "Error: Unsupported action '${action}', type 'help' for help"
    return  $?
}


main  "$@"
exit  "$?"
