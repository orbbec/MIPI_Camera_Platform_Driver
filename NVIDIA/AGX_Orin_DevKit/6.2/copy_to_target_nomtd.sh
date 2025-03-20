#!/bin/bash

sudo cp tegra234-camera-g300-nomtd-overlay.dtbo /boot/tegra234-camera-g300-overlay.dtbo

# backup:
back_file=/lib/modules/$(uname -r)/modules_$(uname -r)_updates.tar.bz2
if [ ! -f $back_file ];then
    echo "bakckup updates"
    sudo tar -cjf $back_file /lib/modules/$(uname -r)/updates
fi
sudo cp -r updates /lib/modules/$(uname -r)/

# enable metadata:
sudo cp videodev.ko /lib/modules/$(uname -r)/kernel/drivers/media/v4l2-core/videodev.ko

sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="Jetson Orbbec Camera G335Lg"
sudo depmod

