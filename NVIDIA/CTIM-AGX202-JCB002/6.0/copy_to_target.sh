#!/bin/bash

sudo cp tegra234-orin-agx-cti-AGX202-JCB002-G2XX.dtb /boot/dtb/kernel_tegra234-orin-agx-cti-AGX202-JCB002-G2XX.dtb

kernel_ver=$(uname -r)
# backup kernel driver modules:
back_file=/lib/modules/$kernel_ver-orig.tar.gz
if [ ! -f $back_file ];then
    echo "bakckup updates"
    sudo tar -czf $back_file -C /lib/modules $kernel_ver
    sudo rm -rf /lib/modules/$kernel_ver
    sudo tar -xzf $kernel_ver.tar.gz -C /lib/modules
else
    sudo tar -xzf updates.tar.gz -C /lib/modules/$kernel_ver
fi

# enable metadata:
sudo cp videodev.ko /lib/modules/$kernel_ver/kernel/drivers/media/v4l2-core/videodev.ko

if [ ! -f "/boot/Image.orig" ];then
    echo "bakckup Image"
    sudo cp /boot/Image /boot/Image.orig
    sudo cp /boot/extlinux/extlinux.conf /boot/extlinux/extlinux.conf.bak
fi

sudo cp Image /boot/Image
sudo cp extlinux.conf /boot/extlinux/

#sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="Jetson Orbbec Camera G335Lg"

if [ -e /etc/modules-load.d/g2xx.conf ]; then
    echo "Files g2xx.conf has exists, rm g2xx.conf, create g2xx.conf"
    sudo rm /etc/modules-load.d/g2xx.conf
else
    echo "Files g2xx.conf not exists, create g2xx.conf"    
fi

echo "max9296" |sudo tee /etc/modules-load.d/g2xx.conf
echo "max9295" |sudo tee -a /etc/modules-load.d/g2xx.conf
echo "g2xx" |sudo tee -a /etc/modules-load.d/g2xx.conf
echo "obc_cam_sync" |sudo tee -a /etc/modules-load.d/g2xx.conf

sudo depmod

