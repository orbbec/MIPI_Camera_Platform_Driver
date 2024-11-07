#!/bin/bash

sudo cp tegra234-camera-g2xx-cti-overlay.dtbo /boot/tegra234-camera-g2xx-overlay.dtbo

# backup:
back_file=/lib/modules/$(uname -r)/modules_$(uname -r)_updates.tar.bz2
if [ ! -f $back_file ];then
    echo "bakckup updates"
    sudo tar -cjf $back_file /lib/modules/$(uname -r)/updates
fi
sudo cp -r updates /lib/modules/$(uname -r)/

# enable metadata:
sudo cp uvcvideo.ko /lib/modules/$(uname -r)/kernel/drivers/media/usb/uvc/uvcvideo.ko
sudo cp videodev.ko /lib/modules/$(uname -r)/kernel/drivers/media/v4l2-core/videodev.ko
# backup kernel (better to have additional boot entry in extlinux.conf)

if [ ! -f "/boot/Image.orig" ];then
    echo "bakckup Image"
    sudo cp /boot/Image /boot/Image.orig
fi

sudo cp Image /boot/Image

if [ -e /boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb ]; then
    if [ ! -f "/boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb.orig" ];then
        echo "bakckup DTB"
        sudo cp /boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb /boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb.orig
    fi
    sudo cp tegra234-p3737-0000+p3701-0000-nv.dtb /boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb
fi

if [ -e /boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb ]; then
    if [ ! -f "/boot/dtb/kernel_tegra234-p3737-0000+p3701-0000-nv.dtb.orig" ];then
        echo "bakckup DTB"
        sudo cp /boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb /boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb.orig
    fi
   
    sudo cp tegra234-p3737-0000+p3701-0005-nv.dtb /boot/dtb/kernel_tegra234-p3737-0000+p3701-0005-nv.dtb
fi

sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="Jetson Orbbec Camera G335Lg"
sudo depmod

