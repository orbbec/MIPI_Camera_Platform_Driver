# ORBBEC camera driver for GMSL* interface

## Build kernel, dtb and G335Lg driver

```
# Enter the directory where the kernel source code is located
cd MIC-733AO_64G_Orin_6.1_V1.0.0_Kernel_DeviceTree
mkdir -p l4t-gcc/6.1
cd ./l4t-gcc/6.1
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/toolchain/aarch64--glibc--stable-2022.08-1.tar.bz2 -O aarch64--glibc--stable-final.tar.bz2
tar xf aarch64--glibc--stable-final.tar.bz2 --strip-components 1
cd ../..
```

Apply G335Lg patches and build the kernel image, dtb and G335Lg driver.

```
# Apply G335Lg patches :
git apply mic_733ao_6.1_v1006.patch

# build kernel, dtb and G335Lg driver
# install dependencies
sudo apt install build-essential bc flex bison
# build kernel Debian packages
./build_all.sh 6.1 kernel

```

Debian packages will be generated in `images` folder.

## Install kernel and G335Lg driver to Jetson Orin
<details>
<summary>JP6.1 build results</summary>

- kernel image (not modified): `images/6.1/rootfs/boot/Image`
- dtb overlay: `images/6.1/rootfs/boot/tegra234-camera-g300-overlay.dtbo`
- oot modules: `images/6.1/rootfs/lib/modules/5.15.148-tegra/update`
- oot modules: `images/6.1/rootfs/lib/modules/5.15.148-tegra/kernel/drivers/media/v4l2-core/videodev.ko`
Copy them to the target MIC-733AO:

```
# run copy_to_ssh.sh Copy them to the target ,Need to be replaced with the user_name and IP you are using
sh copy_to_ssh.sh

```

on target MIC-733AO Copy them to the right places:

```
cd gmsl-driver-mic-733ao
sh copy_to_target.sh
```

Unload and reload the G335Lg driver.

```
sh reconnect.sh
```


