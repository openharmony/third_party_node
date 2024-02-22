#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2020-2030. All rights reserved.
set -e

while getopts "o:i:l:t:h" arg
do
    case "${arg}" in
        "o")
            OUT_DIR=${OPTARG}
            ;;
        "i")
            SOURCE_DIR=${OPTARG}
            ;;
        "l")
            TOOL_DIR=${OPTARG}
            ;;
        "h")
            echo "help"
            ;;
        ?)
            echo "unkonw argument"
            exit 1
            ;;
    esac
done


ndk_dir=$(ls ${SOURCE_DIR}/lib/|more |grep ohos |grep -v mipsel |grep -v riscv |awk '{print $NF}')

for i in $ndk_dir
do
	find ${OUT_DIR}/lib/$i -name 'libc++.a' -type f  -exec bash -c 'echo "INPUT(-lc++_static -lc++abi)" > $1' _ {}  \;
	find ${OUT_DIR}/lib/$i -name 'libc++.so' -type f -exec bash -c 'echo "INPUT(-lc++_shared)" > $1'  _ {}  \;
done

cp -rfp ${SOURCE_DIR}/lib   ${OUT_DIR}
cp -rfp ${SOURCE_DIR}/include  ${OUT_DIR}

function strip_dir() {
    for file in `ls $1`
    do
        if [ -f $1"/"$file ] && [[ $(file -b $1"/"$file) =~ "shared object" ]]
        then
            echo $1"/"$file
            ${TOOL_DIR}/llvm-strip $1"/"$file
        elif [ -d $1"/"$file ]
        then
            strip_dir $1"/"$file
        fi
    done
}
strip_dir ${OUT_DIR}/lib
