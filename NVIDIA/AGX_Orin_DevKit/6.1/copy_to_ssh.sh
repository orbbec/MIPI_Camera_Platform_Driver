#!/bin/bash

if [ ! -d "gmsl-driver-orin" ];then
mkdir gmsl-driver-orin
fi

cp -r images/6.1/rootfs/boot/tegra234-camera-g300-overlay.dtbo gmsl-driver-orin
cp -r images/6.1/rootfs/boot/tegra234-camera-g300-cti-overlay.dtbo gmsl-driver-orin
cp -r images/6.1/rootfs/boot/tegra234-camera-g300-nomtd-overlay.dtbo gmsl-driver-orin
cp -r images/6.1/rootfs/boot/tegra234-camera-g300-pwm-overlay.dtbo gmsl-driver-orin
cp -r images/6.1/rootfs/boot/tegra234-camera-g300-xc-overlay.dtbo gmsl-driver-orin
cp -r images/6.1/rootfs/boot/tegra234-camera-g300-nomtd-xc-overlay.dtbo gmsl-driver-orin

cp -r images/6.1/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0000-nv.dtb gmsl-driver-orin
cp -r images/6.1/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0005-nv.dtb gmsl-driver-orin
cp -r images/6.1/rootfs/lib/modules/5.15.148-tegra/updates gmsl-driver-orin

# metadata patched kernel modules
cp -r images/6.1/rootfs/lib/modules/5.15.148-tegra/kernel/drivers/media/v4l2-core/videodev.ko gmsl-driver-orin
cp -r images/6.1/rootfs/lib/modules/5.15.148-tegra/kernel/drivers/media/usb/uvc/uvcvideo.ko gmsl-driver-orin
# Kernel Image with SENSOR_HID support for RealSense USB cameras with IMU
cp -r images/6.1/rootfs/boot/Image gmsl-driver-orin

cp copy_to_target.sh gmsl-driver-orin
cp copy_to_target_cti.sh gmsl-driver-orin
cp copy_to_target_nomtd.sh gmsl-driver-orin
cp copy_to_target_pwm.sh gmsl-driver-orin
cp copy_to_target_xc.sh gmsl-driver-orin
cp copy_to_target_nomtd_xc.sh gmsl-driver-orin

scp -r gmsl-driver-orin orbbec@10.8.170.33:~/