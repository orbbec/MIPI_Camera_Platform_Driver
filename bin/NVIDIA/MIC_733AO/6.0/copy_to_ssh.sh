#!/bin/bash

if [ ! -d "gmsl-driver-mic-733ao" ];then
mkdir gmsl-driver-mic-733ao
fi

cp -r images/6.0/rootfs/boot/tegra234-camera-g300-overlay.dtbo gmsl-driver-mic-733ao
cp -r images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0000-nv.dtb gmsl-driver-mic-733ao
cp -r images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0005-nv.dtb gmsl-driver-mic-733ao
cp -r images/6.0/rootfs/lib/modules/5.15.136-tegra/updates gmsl-driver-mic-733ao

# metadata patched kernel modules
cp -r images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/v4l2-core/videodev.ko gmsl-driver-mic-733ao
cp -r images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/usb/uvc/uvcvideo.ko gmsl-driver-mic-733ao
# Kernel Image with SENSOR_HID support for RealSense USB cameras with IMU
cp -r images/6.0/rootfs/boot/Image gmsl-driver-mic-733ao

cp copy_to_target.sh gmsl-driver-mic-733ao
cp reconnect.sh gmsl-driver-mic-733ao

scp -r gmsl-driver-mic-733ao mic-733ao@10.8.170.67:~/

