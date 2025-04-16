#!/bin/bash

if [ -e /boot/tegra234-camera-g300-overlay.dtbo ];then
    sudo rm /boot/tegra234-camera-g300-overlay.dtbo
fi
sudo cp tegra234-p3737-camera-g300-s100-overlay.dtbo /boot/tegra234-p3737-camera-g300-overlay.dtbo

## copy driver modules updates ##
#back_file=/lib/modules/$(uname -r)/modules_$(uname -r)_updates.tar.bz2
#if [ ! -f $back_file ];then
#    echo "bakckup updates"
#    sudo tar -cjf $back_file /lib/modules/$(uname -r)/updates
#fi
#sudo cp -r updates /lib/modules/$(uname -r)/

## copy tegra-camera.ko file ##
tegra_camera_dir=/lib/modules/$(uname -r)/updates/drivers/media/platform/tegra/camera
if [ ! -f $tegra_camera_dir/tegra-camera.ko.orig ];then
    echo "bakckup tegra-camera.ko"
    sudo cp $tegra_camera_dir/tegra-camera.ko $tegra_camera_dir/tegra-camera.ko.orig
fi
sudo cp tegra-camera.ko $tegra_camera_dir/

tegra_rtcpu_dir=/lib/modules/$(uname -r)/updates/drivers/platform/tegra/rtcpu
if [ ! -f $tegra_rtcpu_dir/capture-ivc.ko.orig ];then
    echo "bakckup capture-ivc.ko"
    sudo cp $tegra_rtcpu_dir/capture-ivc.ko $tegra_rtcpu_dir/capture-ivc.ko.orig
fi
sudo cp capture-ivc.ko  $tegra_rtcpu_dir/

sudo cp obc_max9296.ko  /lib/modules/$(uname -r)/updates/drivers/media/i2c/
sudo cp obc_max96712.ko /lib/modules/$(uname -r)/updates/drivers/media/i2c/
sudo cp g300.ko         /lib/modules/$(uname -r)/updates/drivers/media/i2c/

## copy videodev.ko file ##
videodev_dir=/lib/modules/$(uname -r)/kernel/drivers/media/v4l2-core
if [ ! -f $videodev_dir/videodev.ko.orig ];then
    echo "bakckup videodev.ko"
    sudo cp $videodev_dir/videodev.ko $videodev_dir/videodev.ko.orig
fi
sudo cp videodev.ko $videodev_dir/

sudo cp obc_cam_sync.ko /lib/modules/$(uname -r)/updates/drivers/misc/

## copy Image ##
if [ ! -f "/boot/Image.orig" ];then
    echo "bakckup Image"
    sudo cp /boot/Image /boot/Image.orig
fi

sudo cp Image /boot/Image

## copy dtb ##
if [ -e /boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb ]; then
    if [ ! -f "/boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb.orig" ];then
        echo "bakckup 00 DTB"
        sudo cp /boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb /boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb.orig
    fi
    sudo cp tegra234-p3737-0000+p3701-0000-nv.dtb /boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb
fi

if [ -e /boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb ]; then
    if [ ! -f "/boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb.orig" ];then
        echo "bakckup 05 DTB"
        sudo cp /boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb /boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb.orig
    fi
    sudo cp tegra234-p3737-0000+p3701-0005-nv.dtb /boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb
fi

## Modified /boot/extlinux/extlinux.conf to add following DTBO entries ##
sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="Jetson Orbbec Camera G335Lg"

sudo depmod

