#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
PROJECTS="$(find $basepath/../project/mt*/config*/*/HIFI4_A/  -name 'ProjectConfig.mk')"

show_usage() {
    echo "Usage: $0 <project|list|all> [clean]"
    echo ""
    echo "Example:"
    echo "  $0 mt8168-default-project"
    echo "      -> build mt8168-default-project project"
    echo "  $0 clean"
    echo "      -> clean out folder"
    echo "  $0 mt8168-default-project clean"
    echo "      -> only clean out/mt8168-default-project folder"
    echo "  $0 list"
    echo "      -> list all supported projects"
    echo "  $0 all"
    echo "      -> build all supported projects"
    echo ""
}

declare -a project_list
show_available_project() {
    echo ""
    echo "Available Build Projects:"
    echo ""
    for i in ${!project_list[@]}
    do
        echo "  ${i}. ${project_list[i]}"
    done
    echo ""
}

select_project() {
    while read -p "Please select project index to build: " select_index
    do
        if [[ "${select_index}" =~ ^[0-9]+$ ]]; then
            if [ "${select_index}" -ge "0" -a "${select_index}" -lt "${#project_list[@]}" ]; then
                MY_PROJECT=${project_list[select_index]}
                break
            else
                echo "Wrong project index!"
            fi
        elif [[ "${select_index}" = "q" ]]; then
            exit 0
        else
            echo "Project index is not integer!"
        fi
    done
}

#
# Begin here
#
BUILD_ALL_PROJECTS=0

idx=0
for i in `echo ${PROJECTS} | tr " " "\n" | sort`
do
    tmp=${i%/*}
    tmp=${tmp%/*}
    p=${tmp##*/}
    project_list[idx]=${p}
    let idx=idx+1
done

if [ "$#" -eq "0" ]; then
    show_available_project
    select_project
elif [ "$#" -eq "1" ]; then
    if [ "$1" = "list" ]; then
        show_available_project
        exit 0
    elif [ "$1" = "clean" ]; then
        rm -rf $basepath/../out
        exit 0
    elif [ "$1" = "-h" -o "$1" = "help" ]; then
        show_usage
        exit
    elif [ "$1" = "all" ]; then
        show_available_project
        BUILD_ALL_PROJECTS=1
    else
        MY_PROJECT=$1
        if echo ${project_list[@]} | grep -w "${MY_PROJECT}" &> /dev/null; then
            :
        else
            echo "project ${MY_PROJECT} not found"
            exit 1
        fi
    fi
elif [ "$#" -eq 2 ]; then
    if [ "$2" = "clean" ]; then
        MY_PROJECT=$1
        if echo ${project_list[@]} | grep -w "${MY_PROJECT}" &> /dev/null; then
            rm -rf $basepath/../out/${MY_PROJECT}
            exit 0
        else
            echo "project ${MY_PROJECT} not found"
            exit 1
        fi
    else
        echo "Unsupport arguments"
        show_usage
        exit 1
    fi
else
    echo "Unsupport arguments"
    show_usage
    exit 1
fi

if [ "${BUILD_ALL_PROJECTS}" -eq "0" ]; then
    echo ""
    echo "Start to build project: ${MY_PROJECT}"
    echo ""
    PROJECT="${MY_PROJECT}" make -j24 -C $basepath || exit 1
elif [ "${BUILD_ALL_PROJECTS}" -eq "1" ]; then
    for p in ${project_list[@]}
    do
        echo ""
        echo "Start to build project: ${p}"
        echo ""
        PROJECT="${p}" make -j24 -C $basepath || exit 1
    done
fi
unset project_list

exit 0
