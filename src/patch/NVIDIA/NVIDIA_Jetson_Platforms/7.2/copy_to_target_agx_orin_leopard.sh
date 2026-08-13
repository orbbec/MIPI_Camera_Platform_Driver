#!/bin/bash

EXPECTED_KERNEL_PATTERN="6.8.12-1021-tegra"
EXPECTED_JETPACK="JetPack 7.2"
KVER=$(uname -r)

check_kernel_version()
{
    case "$KVER" in
        6.8.12-1021-tegra)
            ;;
        *)
            echo "ERROR: unsupported Linux kernel version: $KVER" >&2
            echo "This driver package is built for ${EXPECTED_JETPACK} / Linux ${EXPECTED_KERNEL_PATTERN}." >&2
            echo "Refusing to install to avoid mixing kernel modules across JetPack versions." >&2
            exit 1
            ;;
    esac
}

check_kernel_version

sudo cp tegra234-p3737-camera-g300-leopard-overlay.dtbo /boot/tegra234-p3737-camera-g300-overlay.dtbo

## copy tegra-camera.ko file ##
tegra_camera_dir=/lib/modules/${KVER}/updates/drivers/media/platform/tegra/camera
if [ ! -f $tegra_camera_dir/tegra-camera.ko.orig ];then
    echo "bakckup tegra-camera.ko"
    sudo cp $tegra_camera_dir/tegra-camera.ko $tegra_camera_dir/tegra-camera.ko.orig
fi
sudo cp tegra-camera.ko $tegra_camera_dir

# enable metadata:
## copy videodev.ko file ##
videodev_dir=/lib/modules/${KVER}/kernel/drivers/media/v4l2-core
if [ ! -f $videodev_dir/videodev.ko.orig ];then
    echo "bakckup videodev.ko"
    sudo cp $videodev_dir/videodev.ko $videodev_dir/videodev.ko.orig
fi
sudo cp videodev.ko $videodev_dir

## copy nvhost-nvcsi.ko file ##
nvhost_nvcsi_t194_dir=/lib/modules/${KVER}/updates/drivers/video/tegra/host/nvcsi/
if [ ! -f $nvhost_nvcsi_t194_dir/nvhost-nvcsi.ko.orig ];then
    echo "bakckup nvhost-nvcsi.ko"
    sudo cp $nvhost_nvcsi_t194_dir/nvhost-nvcsi.ko $nvhost_nvcsi_t194_dir/nvhost-nvcsi.ko.orig
fi
sudo cp nvhost-nvcsi.ko $nvhost_nvcsi_t194_dir

sudo cp obc_max9296.ko /lib/modules/${KVER}/updates/drivers/media/i2c/
sudo cp obc_max96712.ko /lib/modules/${KVER}/updates/drivers/media/i2c/
sudo cp g300.ko /lib/modules/${KVER}/updates/drivers/media/i2c/
sudo cp obc_cam_sync.ko /lib/modules/${KVER}/updates/drivers/misc/

# Modified /boot/extlinux/extlinux.conf to add following DTBO entries
sudo /opt/nvidia/jetson-io/config-by-hardware.py -n 2="Jetson Orbbec Camera G300"
sudo depmod
