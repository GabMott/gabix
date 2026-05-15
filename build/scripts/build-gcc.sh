#!/bin/bash
# build-gcc.sh — Build and install x86_64-elf GCC cross-compiler
# Two-stage build: all-gcc first, then all-target-libgcc (bootstrap).
# Usage: bash build-gcc.sh <version>

GCC_VERSION=$1

export PREFIX="/usr/local"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"

cd $PREFIX/src
mkdir build-gcc
cd build-gcc

../gcc-${GCC_VERSION}/configure \
    --target=$TARGET \
    --prefix="$PREFIX" \
    --disable-nls \
    --enable-languages=c,c++ \
    --without-headers

make -j $(nproc) all-gcc
make -j $(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc

cd $PREFIX/src
rm -rf build-gcc.sh build-gcc gcc-${GCC_VERSION}
