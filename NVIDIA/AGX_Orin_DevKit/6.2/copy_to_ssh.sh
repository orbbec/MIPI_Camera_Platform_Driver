#!/bin/bash

if [ ! -d "gmsl-driver-orin" ];then
mkdir gmsl-driver-orin
fi

cp -r images/6.2/rootfs/boot/tegra234-camera-g300-overlay.dtbo gmsl-driver-orin
cp -r images/6.2/rootfs/boot/tegra234-camera-g300-cti-overlay.dtbo gmsl-driver-orin
cp -r images/6.2/rootfs/boot/tegra234-camera-g300-nomtd-overlay.dtbo gmsl-driver-orin
cp -r images/6.2/rootfs/boot/tegra234-camera-g300-pwm-overlay.dtbo gmsl-driver-orin
cp -r images/6.2/rootfs/boot/tegra234-camera-g300-xc-overlay.dtbo gmsl-driver-orin
cp -r images/6.2/rootfs/boot/tegra234-camera-g300-nomtd-xc-overlay.dtbo gmsl-driver-orin
cp -r images/6.2/rootfs/boot/tegra234-camera-g300-leopard-overlay.dtbo gmsl-driver-orin

cp -r images/6.2/rootfs/lib/modules/5.15.148-tegra/updates gmsl-driver-orin

# metadata patched kernel modules
cp -r images/6.2/rootfs/lib/modules/5.15.148-tegra/kernel/drivers/media/v4l2-core/videodev.ko gmsl-driver-orin
# Kernel Image with SENSOR_HID support for RealSense USB cameras with IMU
cp -r images/6.2/rootfs/boot/Image gmsl-driver-orin

cp copy_to_target.sh gmsl-driver-orin
cp copy_to_target_cti.sh gmsl-driver-orin
cp copy_to_target_nomtd.sh gmsl-driver-orin
cp copy_to_target_nomtd_xc.sh gmsl-driver-orin
cp copy_to_target_pwm.sh gmsl-driver-orin
cp copy_to_target_xc.sh gmsl-driver-orin
cp copy_to_target_leopard.sh gmsl-driver-orin
cp reconnect.sh gmsl-driver-orin

scp -r gmsl-driver-orin orbbec@10.8.170.19:~/

