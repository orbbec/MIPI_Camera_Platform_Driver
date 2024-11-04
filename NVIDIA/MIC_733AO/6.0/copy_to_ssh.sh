#!/bin/bash

if [ ! -d "gmsl-driver" ];then
mkdir gmsl-driver
fi

cp -r images/6.0/rootfs/boot/tegra234-camera-g2xx-overlay.dtbo gmsl-driver
cp -r images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0000-nv.dtb gmsl-driver
cp -r images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0005-nv.dtb gmsl-driver
cp -r images/6.0/rootfs/lib/modules/5.15.136-tegra/updates gmsl-driver

# metadata patched kernel modules
cp -r images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/v4l2-core/videodev.ko gmsl-driver
cp -r images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/usb/uvc/uvcvideo.ko gmsl-driver
# Kernel Image with SENSOR_HID support for RealSense USB cameras with IMU
cp -r images/6.0/rootfs/boot/Image gmsl-driver

cp copy_to_target.sh gmsl-driver

scp -r gmsl-driver mic-733ao@10.8.170.18:~/

