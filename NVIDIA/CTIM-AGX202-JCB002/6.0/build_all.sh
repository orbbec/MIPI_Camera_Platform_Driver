#!/bin/bash

set -e

export DEVDIR=$(cd `dirname $0` && pwd)
NPROC=$(nproc)

export JETPACK_VERSION="6.0"
L4T_VERSION="jetson_36.3"
KERNEL_DIR="kernel/kernel-jammy-src"

SRCS=$DEVDIR/Linux_for_Tegra/sources

export KERNEL_DEF_CONFIG=cti_tegra_defconfig
#export KERNEL_DEF_CONFIG=defconfig
export CROSS_COMPILE=$DEVDIR/aarch64--glibc--stable-2022.08-1/bin/aarch64-buildroot-linux-gnu-
export LOCALVERSION=-tegra
export TEGRA_KERNEL_OUT=$DEVDIR/images/$JETPACK_VERSION
mkdir -p $TEGRA_KERNEL_OUT
export KERNEL_MODULES_OUT=$TEGRA_KERNEL_OUT/modules

# Check if BUILD_NUMBER is set as it will add a postfix to the kernel name "vermagic" (normally it happens on CI who have BUILD_NUMBER defined)
[[ -n "${BUILD_NUMBER}" ]] && echo "Warning! You have BUILD_NUMBER set to ${BUILD_NUMBER}, This will affect your vermagic"

# Build jp6 out-of-tree modules
# following: 
# https://docs.nvidia.com/jetson/archives/r36.2/DeveloperGuide/SD/Kernel/KernelCustomization.html#building-the-jetson-linux-kernel

cd $SRCS
export KERNEL_OUTPUT=$TEGRA_KERNEL_OUT/kernel_out
export KERNEL_HEADERS=$SRCS/kernel/kernel-jammy-src
#make ARCH=arm64 clean -C kernel
#exit 1
make ARCH=arm64 -C kernel
make ARCH=arm64 modules
make ARCH=arm64 dtbs
mkdir -p $TEGRA_KERNEL_OUT/rootfs/boot/dtb
cp $SRCS/nvidia-oot/device-tree/platform/generic-dts/dtbs/tegra234-orin-agx-cti-AGX202-JCB002-G300.dtb $TEGRA_KERNEL_OUT/rootfs/boot/dtb/
export INSTALL_MOD_PATH=$TEGRA_KERNEL_OUT/rootfs/
make ARCH=arm64 install -C kernel
make ARCH=arm64 modules_install


