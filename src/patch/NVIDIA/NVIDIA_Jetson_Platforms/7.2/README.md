# ORBBEC G300 GMSL Camera Driver

This repository builds and packages the Orbbec G300 camera drivers, kernel
modules, and camera DTBO overlays for Jetson Orin and Thor platforms.

## Build

Download the JetPack 7.2 toolchain and source packages:

```bash
mkdir -p l4t-gcc/7.2
cd l4t-gcc/7.2
wget https://developer.nvidia.com/downloads/embedded/L4T/r38_Release_v2.0/release/x-tools.tbz2
tar xf x-tools.tbz2
cd ../..

wget https://developer.nvidia.com/downloads/embedded/L4T/r39_Release_v2.0/sources/public_sources.tbz2
tar xjf public_sources.tbz2
cd Linux_for_Tegra/source
tar xf kernel_src.tbz2
tar xf kernel_oot_modules_src.tbz2
tar xf nvidia_kernel_display_driver_source.tbz2
tar xf nvidia_unified_gpu_display_driver_source.tbz2
cd ../..
```

Build kernel modules and DTBO overlays:

```bash
# Apply G300 patches :
git apply jetson_jp7.2_g300_driver_v1.x.xx.patch

# install dependencies
sudo apt install build-essential bc flex bison libssl-dev zstd

# build kernel, dtb and G335Lg driver
./build_all.sh 7.2 Linux_for_Tegra/source
```

Build outputs are placed under `images/7.2/rootfs` and
`Linux_for_Tegra/source/build/nvidia-public/devicetree/generic-dtbs`.

## Package

Create the target deployment package:

```bash
./copy_to_jetson_ssh.sh 7.2
```

The package directory is `gmsl-driver-jetson`. It contains G300 DTBO overlays,
patched camera modules, G300 I2C drivers, `obc_cam_sync.ko`, deployment scripts,
and `remove_g300.sh`.

## obcam Thin Driver

`Linux_for_Tegra/source/nvidia-oot/drivers/media/i2c/obcam.c` builds
`obcam.ko`. It is a thin V4L2 subdevice and media graph driver used by the
`obcam-thin` overlays. Unlike the full `g300.ko` path, it does not initialize or
program the G300 sensor or SerDes pipeline in kernel space. Its job is to bind
device tree nodes with `compatible = "orbbec,obcam"`, expose stable media
entities to the Tegra VI/NVCSI graph, and advertise the mbus formats,
resolutions, and frame intervals expected by userspace.

The thin path is intended for systems where userspace owns G300 and SerDes
stream configuration. `obcam.c` provides YUYV depth/RGB formats and Y8 IR
formats with the supported G300 resolutions and frame rates, while userspace
configures the real sensor mode and GMSL links before capture.

## Install On Target

Copy `gmsl-driver-jetson` to the target Jetson, then run one script matching the
board and deserializer hardware:

```bash
cd gmsl-driver-jetson
```

AGX Orin:

```bash
# AGX Orin + G300 + FG96-8CH GMSL (EVK): use FG96_8CH_GMSL_V2 deserializer board and PWM01/TSC3 to generate multi-camera synchronization signals.
./copy_to_target_agx_orin_fg96.sh

# AGX Orin + G300 + FG96-8CH GMSL (EVK): use FG96_8CH_GMSL_V2 deserializer board, PWM01/TSC3 synchronization, and no metadata row mode.
./copy_to_target_agx_orin_nomtd_fg96.sh

# AGX Orin + G300 + Leopard LI-JAG-ADP-GMSL2-8CH: use TSC0 to generate multi-camera synchronization signals.
./copy_to_target_agx_orin_leopard.sh

# AGX Orin + G300 + Leopard LI-JAG-ADP-GMSL2-8CH: use TSC0 synchronization and no metadata row mode.
./copy_to_target_agx_orin_nomtd_leopard.sh

# AGX Orin + G300 + Advantech MIC-FG-8G: use TSC0 to generate multi-camera synchronization signals.
./copy_to_target_agx_orin_mic_fg_8g.sh

# AGX Orin + G300 + FG12-16CH multi-deserializer board: use TSC0 to generate multi-camera synchronization signals.
./copy_to_target_agx_orin_fg12.sh

# AGX Orin + G300 obcam-thin + FG96-8CH GMSL (EVK): use FG96_8CH_GMSL_V2 deserializer board and PWM01/TSC3 synchronization; SerDes/G300 stream configuration is handled by userspace.
./copy_to_target_agx_orin_fg96_obcam.sh

# AGX Orin + G300 obcam-thin + Leopard LI-JAG-ADP-GMSL2-8CH: use TSC0 synchronization; SerDes/G300 stream configuration is handled by userspace.
./copy_to_target_agx_orin_leopard_obcam.sh

```

Orin NX:

```bash
# Orin NX + G300 + FG96 2CH: use pwm8 to generate multi-camera synchronization signals.
./copy_to_target_orin_nx_fg96.sh

# Orin NX + G300 + FG96 2CH: use pwm8 synchronization and no metadata row mode.
./copy_to_target_orin_nx_nomtd_fg96.sh
```

Thor:

```bash
# Thor(D317) + G300 + D317 GM deserializer board: use pwm9 to generate multi-camera synchronization signals.
./copy_to_target_thor_dm317.sh

# Thor(MIC-742/MIC-743) + G300 + MIC-FG-8G deserializer: use TSC0 to generate multi-camera synchronization signals.
./copy_to_target_thor_mic_fg_8g.sh
```

Each script copies the selected G300 DTBO to `/boot`, installs the required
modules under `/lib/modules/$(uname -r)`, runs `depmod`, and applies the Jetson-IO
hardware configuration.

Reboot after installation.

## Remove

On the target Jetson:

```bash
cd gmsl-driver-jetson
./remove_g300.sh
```

Then reboot.

## Notes

- G300 camera overlays use `overlay-name = "Jetson Orbbec Camera G300"` except
  obcam-thin variants, which use `overlay-name = "Jetson Orbbec Camera G300 obcam thin"`.
- obcam-thin variants install `obcam.ko` and reserve V4L2/media nodes while
  userspace configures SerDes/G300 streams. See
  `G300_OBCAM_THIN_USERSPACE_CONFIG_CN.md` for the userspace control design.
- Thor G300 overlays use `compatible = "orbbec,obc_cam_sync-tegra264"` for
  `obc_cam_sync`.
- Orin G300 overlays use `compatible = "orbbec,obc_cam_sync-tegra234"` for
  `obc_cam_sync`.
- `nvhost-nvcsi.ko` is the current NVCsi module name used by deployment scripts.
