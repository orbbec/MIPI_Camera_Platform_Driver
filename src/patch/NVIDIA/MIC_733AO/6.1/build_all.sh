#!/bin/bash
set -e

if [[ "$1" == "-h" ]]; then
    echo "build_all.sh [JetPack_version] [JetPack_Linux_source]"
    echo "build_all.sh -h"
    exit 1
fi

export DEVDIR=$(cd `dirname $0` && pwd)
NPROC=$(nproc)

export JETPACK_VERSION="$1"
export L4T_VERSION="jetson_36.4"
export KERNEL_DIR="kernel/kernel-jammy-src"

SRCS="$DEVDIR/sources_$JETPACK_VERSION"
if [[ -n "$2" ]]; then
    SRCS=$(realpath $2)
fi

if [[ "$JETPACK_VERSION" == "6.1" ]]; then
    export CROSS_COMPILE=$DEVDIR/l4t-gcc/$JETPACK_VERSION/bin/aarch64-buildroot-linux-gnu-
fi
export LOCALVERSION=-tegra
export TEGRA_KERNEL_OUT=$DEVDIR/images/$JETPACK_VERSION
mkdir -p $TEGRA_KERNEL_OUT
export KERNEL_MODULES_OUT=$TEGRA_KERNEL_OUT/modules

# Check if BUILD_NUMBER is set as it will add a postfix to the kernel name "vermagic" (normally it happens on CI who have BUILD_NUMBER defined)
[[ -n "${BUILD_NUMBER}" ]] && echo "Warning! You have BUILD_NUMBER set to ${BUILD_NUMBER}, This will affect your vermagic"

# Build jp6 out-of-tree modules
# following: 
# https://docs.nvidia.com/jetson/archives/r36.4/DeveloperGuide/SD/Kernel/KernelCustomization.html#building-the-jetson-linux-kernel
if [[ "$JETPACK_VERSION" == "6.1" ]]; then
    cd $SRCS
    export KERNEL_HEADERS=$SRCS/kernel/kernel-jammy-src
    ln -sf $TEGRA_KERNEL_OUT $SRCS/out
    #make ARCH=arm64 clean
    #exit 1
    make ARCH=arm64 -C kernel
    # export KERNEL_HEADERS=$SRCS/out
    make ARCH=arm64 modules
    make ARCH=arm64 dtbs
    mkdir -p $TEGRA_KERNEL_OUT/rootfs/boot/dtb
    cp $SRCS/kernel-devicetree/generic-dts/dtbs/tegra234-camera-g300-overlay.dtbo $TEGRA_KERNEL_OUT/rootfs/boot/
    export INSTALL_MOD_PATH=$TEGRA_KERNEL_OUT/rootfs/
    make ARCH=arm64 install -C kernel
    make ARCH=arm64 modules_install

fi



