#!/bin/bash

arch=$1
shift
if [ -z "$arch" ]; then
	echo "Specify if 32 or 64 arch sync"
	exit 1
fi

PREFIX=`dirname "$0"`

if [ -r ./mock-config.txt ]; then
    echo "Reading ./mock-config.txt"
    . ./mock-config.txt
else
    echo "Missig mock-config.txt"
    exit 1
fi

if [ $arch = "32" ]; then
ROOT=$ROOT32
EXTRA=$EXTRA32
elif [ $arch = "64" ]; then
ROOT=$ROOT64
EXTRA=$EXTRA64
else
    echo "Pick a correct sync arch (32 or 64)"
    exit
fi


if [ "$1" == "init" ]; then
    mock -r $ROOT --init
    shift
fi

mock -r $ROOT --install ccache dos2unix \
    pkgconfig mesa-libGLU-devel \
    libX11-devel libXrandr-devel libXmu-devel \
    libXi-devel libXext-devel libXft-devel \
    alsa-lib-devel freeglut-devel \
    libtiff-devel curl-devel \
    pulseaudio-libs-devel freetype-devel cmake $EXTRA \
    libpng-devel libjpeg-devel zlib-devel fltk \
    vim-enhanced libXinerama-devel libXpm-devel \
    $*
