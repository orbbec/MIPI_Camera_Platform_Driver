#!/bin/bash

sudo cp tegra234-camera-g300-leopard-overlay.dtbo /boot/tegra234-camera-g300-overlay.dtbo

# copy driver modules updates
back_file=/lib/modules/$(uname -r)/modules_$(uname -r)_updates.tar.bz2
if [ ! -f $back_file ];then
    echo "bakckup updates"
    sudo tar -cjf $back_file /lib/modules/$(uname -r)/updates
fi
sudo cp -r updates /lib/modules/$(uname -r)/

# copy driver ko file
sudo cp videodev.ko /lib/modules/$(uname -r)/kernel/drivers/media/v4l2-core/videodev.ko

# Modified /boot/extlinux/extlinux.conf to add following DTBO entries
sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="Jetson Orbbec Camera G335Lg"

sudo depmod

