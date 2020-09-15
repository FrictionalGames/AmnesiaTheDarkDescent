#!/bin/bash

dir=$PWD
arch=$1

if [ -z "$arch" ]; then
    echo "Specify if 32 or 64 arch sync"
    exit 1
fi

shift 1

echo "Syncing code in 2 seconds"
sleep 2
./mock-sync.sh $arch

if [ "$arch" = "32" ]; then
    suffix="x86"
else
    suffix="x86_64"
fi

if [ -r ./mock-config.txt ]; then
    . ./mock-config.txt
else
    echo "Missing mock-config.txt"
    exit 1
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

cat > $root/$dst_root/build.sh <<EOSCRIPT
cd /builddir/$dst_root
rm -f core/sources/BuildID_HPL_2_0_Linux.cpp
mkdir -p build
cd build
cmake $cmake_path -DCMAKE_BUILD_TYPE=Release $cmake_options
make -j4
make -j4 AmnesiaSteam
cd ..
EOSCRIPT



echo "Starting build in 2 seconds"
sleep 2

./mock-shell.sh $arch -- "cd builddir/$dst_root; bash ./build.sh;"

echo "Copying binary"
mkdir -p ./Release/
cp $root/$dst_root/src/build/*.bin.$suffix ./Release/

