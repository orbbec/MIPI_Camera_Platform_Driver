#!/bin/bash

if [ ! -d "gmsl-driver-jetson" ];then
mkdir gmsl-driver-jetson
fi

## copy dtb file ##
cp images/6.0/rootfs/boot/tegra234-p3737-camera-g300-s100-overlay.dtbo gmsl-driver-jetson

cp images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0000-nv.dtb gmsl-driver-jetson
cp images/6.0/rootfs/boot/dtb/tegra234-p3737-0000+p3701-0005-nv.dtb gmsl-driver-jetson

## cp driver .ko file ##
#cp -r images/6.0/rootfs/lib/modules/5.15.136-tegra/updates gmsl-driver-jetson

cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/platform/tegra/rtcpu/capture-ivc.ko	gmsl-driver-jetson/
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/media/platform/tegra/camera/tegra-camera.ko  gmsl-driver-jetson/

cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/media/i2c/obc_max9296.ko  gmsl-driver-jetson/
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/media/i2c/obc_max96712.ko  gmsl-driver-jetson/
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/media/i2c/g300.ko  gmsl-driver-jetson/

cp images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/v4l2-core/videodev.ko gmsl-driver-jetson

cp images/6.0/rootfs/lib/modules/5.15.136-tegra/updates/drivers/misc/obc_cam_sync.ko gmsl-driver-jetson

## copy image file ##
cp images/6.0/rootfs/boot/Image gmsl-driver-jetson

## copy script file ##
cp copy_to_target_agx_orin_s100.sh gmsl-driver-jetson
cp reconnect.sh gmsl-driver-jetson

## ssh copy all file ##
#scp -r gmsl-driver-jetson ubuntu@10.8.170.44:~/

