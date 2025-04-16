# ORBBEC camera driver for GMSL

## Build kernel, dtb and G335Lg driver

Download the source code and toolchain.

```
# JetPack 6.0
mkdir -p l4t-gcc/6.0
cd ./l4t-gcc/6.0
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/toolchain/aarch64--glibc--stable-2022.08-1.tar.bz2 -O aarch64--glibc--stable-final.tar.bz2
tar xf aarch64--glibc--stable-final.tar.bz2 --strip-components 1
cd ../..
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/sources/public_sources.tbz2
tar xjf public_sources.tbz2
cd Linux_for_Tegra/source
tar xjf kernel_src.tbz2
tar xjf kernel_oot_modules_src.tbz2
tar xjf nvidia_kernel_display_driver_source.tbz2
cd ../..
```

Apply G335Lg patches and build the kernel image, dtb and G335Lg driver.

```
# Apply G335Lg patches :
git apply advan_amr-s100_g300_driver_v1.0.1.patch

# build kernel, dtb and G335Lg driver

# install dependencies
sudo apt install build-essential bc flex bison

# build kernel Debian packages
./build_all.sh

# Copy the updated files to the folder(gmsl-driver-jetson)
./copy_to_jetson.sh
```

## Install kernel and G335Lg driver to Jetson Orin

Copy them to the target Jetson Orin:

```
# Copy all files to the target board via SSH
scp -r gmsl-driver-jetson ubuntu@192.168.1.88:~/
```

on target Jetson Orin Copy them to the right places:

```
cd gmsl-driver-jetson
./copy_to_target_agx_orin_s100.sh
```
