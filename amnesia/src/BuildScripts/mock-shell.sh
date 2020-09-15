#!/bin/bash

arch=$1
shift
if [ -z "$arch" ]; then
    echo "$0 ARCH [-- command]"
    echo "Specify if 32 or 64 arch and an optional command to run in the shell"
    exit 1
fi

if [ -r ./mock-config.txt ]; then
    . ./mock-config.txt
else
    echo "Missig mock-config.txt"
    exit 1
fi

if [ $arch = "32" ]; then
ROOT=$ROOT32
else
ROOT=$ROOT64
fi

mock -r $ROOT  --shell $*
