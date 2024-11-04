# ORBBEC camera driver for GMSL* interface

## Build kernel, dtb and G335Lg driver

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
git apply agx_orin_devkit_g2xx_driver_v1013.patch

# build kernel, dtb and G335Lg driver
# install dependencies
sudo apt install build-essential bc flex bison
# build kernel Debian packages
./build_all.sh 6.0 Linux_for_Tegra/source

```

Debian packages will be generated in `images` folder.

## Install kernel and G335Lg driver to Jetson Orin
<details>
<summary>JP6.0 build results</summary>

- kernel image (not modified): `images/6.0/rootfs/boot/Image`
- dtb : `images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-000x-nv.dtb`
- dtb overlay: `images/6.0/rootfs/boot/tegra234-camera-g2xx-overlay.dtbo`
- oot modules: `images/6.0/rootfs/lib/modules/5.15.136-tegra/update`
- oot modules: `images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/v4l2-core/videodev.ko`
- oot modules: `images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/usb/uvc/uvcvideo.ko`
Copy them to the target Jetson Orin:

```
# run copy_to_ssh.sh Copy them to the target ,Need to be replaced with the user_name and IP you are using
sh copy_to_ssh.sh

```

on target Jetson Orin Copy them to the right places:

```
cd gmsl-driver

# if use XC Dser board
sh copy_to_target.sh

# if use CTI Dser board
sh copy_to_target_cti.sh

# if use PWM01 generate multi-camera synchronization signals.
sh copy_to_target_pwm.sh

# if use G345Lg
sh copy_to_target_nomtd.sh

```




