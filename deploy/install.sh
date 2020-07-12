#!/usr/bin/env bash

#   <INSTALL-DIR>
#       +-- java
#       +-- hadoop
#           +-- bin
#           +-- etc

SELFDIR=$(dirname $(realpath "${BASH_SOURCE[0]}"))

function show_help()
{
    echo    "Usage:"
    echo    "   install.sh <TEAM> <SOFTWARE-DIR> [<INSTALL-DIR>]"
    echo    "   install.sh -h|--help"
    echo    ""
    echo    "Options:"
    echo    "   <TEAM>          The name of your team."
    echo    "   <SOFTWARE-DIR>  The directory where the software placed, such as the hadoop and java."
    echo    "   <INSTALL-DIR>   This project will be installed to '<INSTALL-DIR>/<TEAM>'. The default is current dir."
    echo    "   -h|--help       Show this help"

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

    local package_file="$1"
    if [[ ! -f "${package_file}" ]]; then
        echo    "Can not found or access the package file '${package_file}'"
        return  4
    fi

    local software_dir="$2"
    if [[ ! -d "${software_dir}" ]]; then
        echo    "Can not found or access the software directory '${software_dir}'"
        return  4
    fi

    local install_dir="$3"
    if [[ "${install_dir}" == "" ]]; then
        echo    "Missing parameter of install dir, type -h for help"
        return  5
    fi





    return 0
}

main  "$@"
exit  "$?"

