#!/bin/bash

if [ ! -d "gmsl-driver-jetson" ];then
mkdir gmsl-driver-jetson
fi

cp -r images/6.0/rootfs/boot/tegra234-p3737-camera-g300-mic-fg-8g-overlay.dtbo gmsl-driver-jetson

cp -r images/6.0/rootfs/boot/dtb/tegra234-p3768-0000+p3767-0000-nv.dtb gmsl-driver-jetson
cp -r images/6.0/rootfs/boot/dtb/tegra234-p3768-0000+p3767-0001-nv.dtb gmsl-driver-jetson

cp -r images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0000-nv.dtb gmsl-driver-jetson
cp -r images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0005-nv.dtb gmsl-driver-jetson

cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/media/platform/tegra/camera/tegra-camera.ko gmsl-driver-jetson
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/media/i2c/obc_max9296.ko gmsl-driver-jetson
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/media/i2c/obc_max96712.ko gmsl-driver-jetson
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/media/i2c/g300.ko gmsl-driver-jetson
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/misc/obc_cam_sync.ko gmsl-driver-jetson
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/platform/tegra/rtcpu/capture-ivc.ko gmsl-driver-jetson
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/video/tegra/host/nvcsi/nvhost-nvcsi-t194.ko gmsl-driver-jetson

# metadata patched kernel modules
cp -r images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/v4l2-core/videodev.ko gmsl-driver-jetson
# Kernel Image with SENSOR_HID support for RealSense USB cameras with IMU
cp -r images/6.0/rootfs/boot/Image gmsl-driver-jetson

cp copy_to_target_agx_orin_mic_fg_8g.sh gmsl-driver-jetson
cp reconnect.sh gmsl-driver-jetson
cp README.md gmsl-driver-jetson

scp -r gmsl-driver-jetson mic-732@10.8.170.20:~/