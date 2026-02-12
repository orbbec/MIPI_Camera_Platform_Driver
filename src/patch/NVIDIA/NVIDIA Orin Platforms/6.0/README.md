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
git apply agx_orin_devkit_6.0_v1.2.05.patch

# build kernel, dtb and G335Lg driver
# install dependencies
sudo apt install build-essential bc flex bison
# build kernel Debian packages
./build_all.sh 6.0 Linux_for_Tegra/source

```

Debian packages will be generated in `images` folder.

## Install kernel and G335Lg driver to Jetson 
<details>
<summary>JP6.0 build results</summary>

- kernel image (not modified): `images/6.0/rootfs/boot/Image`
- dtb : `images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-000x-nv.dtb`
- dtb overlay: `images/6.0/rootfs/boot/tegra234-camera-g300-overlay.dtbo`
- oot modules: `images/6.0/rootfs/lib/modules/5.15.136-tegra/update`
- oot modules: `images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/v4l2-core/videodev.ko`
Copy them to the target Jetson:

```
# run copy_to_jetson_ssh.sh Copy them to the target Jetson,Need to be replaced with the user_name and IP you are using
sh copy_to_jetson_ssh.sh

```

on target Jetson Devkit Copy them to the right places:

```
cd gmsl-driver-jetson

# AGX_Orin + G335Lg + FG96_8CH_GMSL (EVK): if use FG96_8CH_GMSL_V2 Dser board and use PWM01/TSC3 generate multi-camera synchronization signals.
sh copy_to_target_agx_orin_fg96.sh

# AGX_Orin + G335Lg + LI-JAG-ADP-GMSL2-8CH: if use Leopard LI-JAG-ADP-GMSL2-8CH Dser board and use TSC1 generate multi-camera synchronization signals.
sh copy_to_target_agx_orin_leopard.sh

# AGX_Orin + G335Lg/G345Lg + LI-JAG-ADP-GMSL2-8CH: If use LI-JAG-ADP-GMSL2-8CH Dser board, use TSC1 generate multi-camera synchronization signals and place the Metadata data in the first row of the image.
sh copy_to_target_agx_orin_nomtd_leopard.sh

# AGX_Orin + G335Lg + MIC-FG-8G: if use ADVANTECH MIC-FG-8G Dser board and use TSC1 generate multi-camera synchronization signals.
sh copy_to_target_agx_orin_mic_fg_8g.sh

# AGX_Orin + G335Lg/G345Lg + FG96_8CH_GMSL (EVK): If use FG96_8CH_GMSL_V2 Dser board, use TSC1 generate multi-camera synchronization signals and place the Metadata data in the first row of the image.
sh copy_to_target_agx_orin_nomtd_fg96.sh

# Orin_NX +  G335Lg + FG96_2CH: if use FG96_2CH_V2 Dser board and use pwm8 generate multi-camera synchronization signals.
sh copy_to_target_orin_nx_fg96.sh

```
