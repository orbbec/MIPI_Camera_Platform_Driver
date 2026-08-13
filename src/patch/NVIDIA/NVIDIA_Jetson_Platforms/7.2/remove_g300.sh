#!/bin/bash

set -e

KVER=$(uname -r)

restore_file()
{
    local file="$1"
    local backup="${file}.orig"

    if [ -f "$backup" ]; then
        echo "restore $file"
        sudo mv "$backup" "$file"
    else
        echo "skip restore $file: $backup not found"
    fi
}

remove_file()
{
    local file="$1"

    if [ -e "$file" ]; then
        echo "remove $file"
        sudo rm -f "$file"
    else
        echo "skip remove $file: not found"
    fi
}

restore_kernel_dtb()
{
    local name="$1"
    local file="/boot/dtb/kernel_${name}.dtb"

    restore_file "$file"
}

remove_file /boot/tegra234-p3737-camera-g300-overlay.dtbo
remove_file /boot/tegra234-p3767-camera-p3768-g300-overlay.dtbo
remove_file /boot/tegra264-camera-g300-overlay.dtbo

tegra_camera_dir="/lib/modules/${KVER}/updates/drivers/media/platform/tegra/camera"
videodev_dir="/lib/modules/${KVER}/kernel/drivers/media/v4l2-core"
capture_ivc_dir="/lib/modules/${KVER}/updates/drivers/platform/tegra/rtcpu"
nvhost_nvcsi_dir="/lib/modules/${KVER}/updates/drivers/video/tegra/host/nvcsi"

restore_file "$tegra_camera_dir/tegra-camera.ko"
restore_file "$videodev_dir/videodev.ko"
restore_file "$capture_ivc_dir/capture-ivc.ko"
restore_file "$nvhost_nvcsi_dir/nvhost-nvcsi.ko"

restore_file /boot/Image

restore_kernel_dtb tegra234-p3737-0000+p3701-0000-nv
restore_kernel_dtb tegra234-p3737-0000+p3701-0005-nv
restore_kernel_dtb tegra234-p3768-0000+p3767-0000-nv
restore_kernel_dtb tegra234-p3768-0000+p3767-0001-nv
restore_kernel_dtb tegra264-p4071-0000+p3834-0000-nv
restore_kernel_dtb tegra264-p4071-0000+p3834-0008-nv
restore_kernel_dtb tegra264-p4071-0008+p3834-0008-nv

remove_file "/lib/modules/${KVER}/updates/drivers/media/i2c/obc_max9296.ko"
remove_file "/lib/modules/${KVER}/updates/drivers/media/i2c/obc_max96712.ko"
remove_file "/lib/modules/${KVER}/updates/drivers/media/i2c/g300.ko"
remove_file "/lib/modules/${KVER}/updates/drivers/media/i2c/obcam.ko"
remove_file "/lib/modules/${KVER}/updates/drivers/misc/obc_cam_sync.ko"

sudo depmod
sudo sync

echo "G300 driver files removed. Reboot is recommended."
