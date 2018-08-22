#!/bin/bash

wget https://curl.haxx.se/download/curl-7.61.0.tar.gz
wget https://labs.mediatek.com/zh-tw/download/AGSSkG38 -O SDK.tar.bz2
tar -xzf curl-7.61.0.tar.gz
tar -xvjf SDK.tar.bz2

export ROOTDIR="${PWD}"
export STAGING_DIR=${ROOTDIR}/OpenWrt-SDK-ramips-mt7688_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir
export TOOLCHAIN_DIR=$STAGING_DIR/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2

export LDCFLAGS=$TOOLCHAIN_DIR/usr/lib
export LD_LIBRARY_PATH=$TOOLCHAIN_DIR/usr/lib
export PATH=$TOOLCHAIN_DIR/bin:$PATH
export CROSS_COMPILE="mipsel-openwrt-linux"
export AR=${CROSS_COMPILE}-ar
export AS=${CROSS_COMPILE}-as
export LD=${CROSS_COMPILE}-ld
export RANLIB=${CROSS_COMPILE}-ranlib
export CC=${CROSS_COMPILE}-gcc
export NM=${CROSS_COMPILE}-nm

cd curl-7.61.0/

#to prepare your build system for compilation
#./buildconf

./configure --prefix=$TOOLCHAIN_DIR --build=mips-openwrt-linux-gnu --host=mips-openwrt-linux-uclibc --disable-shared --enable-static
make
make install
