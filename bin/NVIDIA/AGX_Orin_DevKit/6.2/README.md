# ORBBEC camera driver for GMSL

## Build kernel, dtb and G335Lg driver

Download the source code and toolchain.

```
# JetPack 6.2
mkdir -p l4t-gcc/6.2
cd ./l4t-gcc/6.2
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/toolchain/aarch64--glibc--stable-2022.08-1.tar.bz2 -O aarch64--glibc--stable-final.tar.bz2
tar xf aarch64--glibc--stable-final.tar.bz2 --strip-components 1
cd ../..
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v4.3/sources/public_sources.tbz2
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
git apply agx_orin_devkit_6.2_v1.0.20.patch

# build kernel, dtb and G335Lg driver
# install dependencies
sudo apt install build-essential bc flex bison
# build kernel Debian packages
./build_all.sh 6.2 Linux_for_Tegra/source

```

Debian packages will be generated in `images` folder.

## Install kernel and G335Lg driver to Jetson Orin
<details>
<summary>JP6.2 build results</summary>

- dtb overlay: `images/6.2/rootfs/boot/tegra234-camera-g300-overlay.dtbo`
- oot modules: `images/6.2/rootfs/lib/modules/5.15.148-tegra/update`
- oot modules: `images/6.2/rootfs/lib/modules/5.15.148-tegra/kernel/drivers/media/v4l2-core/videodev.ko`
Copy them to the target Jetson Orin:

```
# run copy_to_ssh.sh Copy them to the target ,Need to be replaced with the user_name and IP you are using
sh copy_to_ssh.sh

```

on target Jetson Orin Copy them to the right places:

```
cd gmsl-driver
# G335Lg: if use FG96_8CH_GMSL_V2 Dser board and use hrtimer generate multi-camera synchronization signals.
sh copy_to_target.sh

# G335Lg: if use FG96_8CH_GMSL_V2 Dser board and use PWM01 generate multi-camera synchronization signals.
sh copy_to_target_pwm.sh

# G335Lg: if use Leopard LI-JAG-ADP-GMSL2-8CH Dser board and use PWM01 generate multi-camera synchronization signals.
sh copy_to_target_leopard.sh

# G335Lg: if use CTI Dser board and use PWM01 generate multi-camera synchronization signals.
sh copy_to_target_cti.sh 

# G335Lg: if use XC Dser board and use hrtimer generate multi-camera synchronization signals.
sh copy_to_target_xc.sh 

# G345Lg: if use FG96_8CH_GMSL_V2 Dser board and use hrtimer generate multi-camera synchronization signals.
sh copy_to_target_nomtd.sh

# G345Lg: if use XC Dser board and use hrtimer generate multi-camera synchronization signals.
sh copy_to_target_nomtd_xc.sh
```
