#!/bin/bash

dir=$PWD
arch=$1

if [ -z "$arch" ]; then
    echo "Specify if 32 or 64 arch sync"
    exit 1
fi

shift 1

if [ "$1" = "-n" ]; then
    rsync_args=-vvn
    shift 1
fi

if [ -r ./mock-config.txt ]; then
    . ./mock-config.txt
else
    echo "Missing mock-config.txt"
    exit 1
fi

if [ -z "$dst_root" ]; then
    echo "Specify the dst_root (relative destination root) in the mock-config.txt"
    exit 1
fi

if [ ! -z "$parent" ]; then
    src_root=$dir/$parent
else
    src_root=$dir
fi

if [ $arch -eq 32 ]; then
    ROOT=$ROOT32
elif [ $arch -eq 64 ]; then
    ROOT=$ROOT64
else
    echo "Pick a correct sync arch (32 or 64)"
    exit 1
fi

root=`mock -r $ROOT --print-root-path`/builddir

if [ ! -e $dir/mock-sync.txt -a ! -e $dir/mock-rsync.txt ]; then
    echo "App has no mock-sync.txt or mock-rsync.txt"
    continue
fi

if [ -e $dir/mock-sync.txt ]; then
    pushd $src_root
    srcpaths=`cat $dir/mock-sync.txt`
    for srcpath in $srcpaths; do
        if [ -d $srcpath ]; then
            mkdir -p $root/$dst_root/$srcpath/ 
            rsync -a  --delete-after $srcpath/ $root/$dst_root/$srcpath/ $rsync_args
        else
            mkdir -p `dirname $root/$dst_root/$srcpath `
            rsync -a  --delete-after $srcpath $root/$dst_root/$srcpath $rsync_args
        fi
    done
    popd
fi

if [ -e $dir/mock-rsync.txt ]; then
    rsync -a -f ". $dir/mock-rsync.txt" $src_root/ $root/$dst_root/ $rsync_args
fi

