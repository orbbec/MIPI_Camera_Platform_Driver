# ORBBEC camera driver for GMSL* interface

## Build kernel, dtb and G335Lg driver

```
# Enter the directory where the kernel source code is located
cd MIC-732_64G_Orin_6.0_V1.0.14_Kernel_DeviceTree
mkdir -p l4t-gcc/6.0
cd ./l4t-gcc/6.0
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/toolchain/aarch64--glibc--stable-2022.08-1.tar.bz2 -O aarch64--glibc--stable-final.tar.bz2
tar xf aarch64--glibc--stable-final.tar.bz2 --strip-components 1
cd ../..
```

Apply G335Lg patches and build the kernel image, dtb and G335Lg driver.

```
# Apply G335Lg patches :
git apply mic_732ao_6.0_v1207.patch

# build kernel, dtb and G335Lg driver
# install dependencies
sudo apt install build-essential bc flex bison
# build kernel Debian packages
./build_all.sh 6.0 kernel

```

Debian packages will be generated in `images` folder.

## Install kernel and G335Lg driver to Jetson Orin
<details>
<summary>JP6.0 build results</summary>

- kernel image (not modified): `images/6.0/rootfs/boot/Image`
- dtb : `images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-000x-nv.dtb`
- dtb overlay: `images/6.0/rootfs/boot/tegra234-p3737-camera-g300-mic-fg-8g-overlay.dtbo`
- oot modules: `images/6.0/rootfs/lib/modules/5.15.136-tegra/update`
- oot modules: `images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/v4l2-core/videodev.ko`
Copy them to the target MIC-732AO:

```
# run copy_to_ssh.sh Copy them to the target ,Need to be replaced with the user_name and IP you are using
sh copy_to_jetson_ssh.sh

```

on target MIC-732AO Copy them to the right places:

```
cd gmsl-driver-jetson

# MIC-732 + G335Lg + ADVANTECH MIC-FG-8G Dser board 
sh copy_to_target_agx_orin_mic_fg_8g.sh

```




