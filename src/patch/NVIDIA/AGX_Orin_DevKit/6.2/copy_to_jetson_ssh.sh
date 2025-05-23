#!/bin/bash

if [ ! -d "gmsl-driver-jetson" ];then
mkdir gmsl-driver-jetson
fi

cp -r images/6.2/rootfs/boot/tegra234-p3737-camera-g300-fg96-overlay.dtbo gmsl-driver-jetson
cp -r images/6.2/rootfs/boot/tegra234-p3737-camera-g300-cti-overlay.dtbo gmsl-driver-jetson
cp -r images/6.2/rootfs/boot/tegra234-p3737-camera-g300-nomtd-fg96-overlay.dtbo gmsl-driver-jetson
cp -r images/6.2/rootfs/boot/tegra234-p3737-camera-g300-xc-overlay.dtbo gmsl-driver-jetson
cp -r images/6.2/rootfs/boot/tegra234-p3737-camera-g300-nomtd-xc-overlay.dtbo gmsl-driver-jetson
cp -r images/6.2/rootfs/boot/tegra234-p3737-camera-g300-leopard-overlay.dtbo gmsl-driver-jetson
cp -r images/6.2/rootfs/boot/tegra234-p3767-camera-p3768-g300-fg96-overlay.dtbo gmsl-driver-jetson
cp -r images/6.2/rootfs/boot/tegra234-p3737-camera-g300-mic-fg-8g-overlay.dtbo gmsl-driver-jetson


cp images/6.2/rootfs/lib/modules/5.15.148-tegra/updates/drivers/media/platform/tegra/camera/tegra-camera.ko gmsl-driver-jetson
cp images/6.2/rootfs/lib/modules/5.15.148-tegra/updates/drivers/media/i2c/obc_max9296.ko gmsl-driver-jetson
cp images/6.2/rootfs/lib/modules/5.15.148-tegra/updates/drivers/media/i2c/obc_max96712.ko gmsl-driver-jetson
cp images/6.2/rootfs/lib/modules/5.15.148-tegra/updates/drivers/media/i2c/g300.ko gmsl-driver-jetson
cp images/6.2/rootfs/lib/modules/5.15.148-tegra/updates/drivers/misc/obc_cam_sync.ko gmsl-driver-jetson
cp images/6.2/rootfs/lib/modules/5.15.148-tegra/updates/drivers/platform/tegra/rtcpu/capture-ivc.ko gmsl-driver-jetson
cp images/6.2/rootfs/lib/modules/5.15.148-tegra/updates/drivers/video/tegra/host/nvcsi/nvhost-nvcsi-t194.ko gmsl-driver-jetson

# metadata patched kernel modules
cp -r images/6.2/rootfs/lib/modules/5.15.148-tegra/kernel/drivers/media/v4l2-core/videodev.ko gmsl-driver-jetson

cp copy_to_target_agx_orin_fg96.sh gmsl-driver-jetson
cp copy_to_target_agx_orin_cti.sh gmsl-driver-jetson
cp copy_to_target_agx_orin_nomtd_fg96.sh gmsl-driver-jetson
cp copy_to_target_agx_orin_nomtd_xc.sh gmsl-driver-jetson
cp copy_to_target_agx_orin_xc.sh gmsl-driver-jetson
cp copy_to_target_agx_orin_leopard.sh gmsl-driver-jetson
cp copy_to_target_orin_nx_fg96.sh gmsl-driver-jetson
cp copy_to_target_agx_orin_mic_fg_8g.sh gmsl-driver-jetson
cp reconnect.sh gmsl-driver-jetson

scp -r gmsl-driver-jetson mic-732@10.8.170.9:~/

