#!/bin/bash

if [ ! -d "gmsl-driver-mic-733ao" ];then
mkdir gmsl-driver-mic-733ao
fi

cp -r images/6.1/rootfs/boot/tegra234-camera-g300-overlay.dtbo gmsl-driver-mic-733ao
cp -r images/6.1/rootfs/lib/modules/5.15.148-tegra/updates gmsl-driver-mic-733ao

# metadata patched kernel modules
cp -r images/6.1/rootfs/lib/modules/5.15.148-tegra/kernel/drivers/media/v4l2-core/videodev.ko gmsl-driver-mic-733ao

cp copy_to_target.sh gmsl-driver-mic-733ao
cp reconnect.sh gmsl-driver-mic-733ao

scp -r gmsl-driver-mic-733ao mic-733ao@10.8.170.50:~/

