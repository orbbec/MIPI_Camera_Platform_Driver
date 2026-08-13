#!/bin/bash

set -e

JETPACK_VERSION=${1:-7.2}
ROOTFS="images/${JETPACK_VERSION}/rootfs"
PACKAGE_DIR="gmsl-driver-jetson"
SOURCE_DTBS="Linux_for_Tegra/source/build/nvidia-public/devicetree/generic-dtbs"

mkdir -p "$PACKAGE_DIR"

copy_required()
{
    local src="$1"

    if [ ! -f "$src" ]; then
        echo "Missing required file: $src"
        exit 1
    fi

    cp -r "$src" "$PACKAGE_DIR"
}

copy_optional()
{
    local src="$1"

    if [ -f "$src" ]; then
        cp -r "$src" "$PACKAGE_DIR"
    else
        echo "Skip missing optional file: $src"
    fi
}

copy_boot_asset()
{
    local name="$1"

    if [ -f "$SOURCE_DTBS/$name" ]; then
        copy_required "$SOURCE_DTBS/$name"
    elif [ -f "$ROOTFS/boot/dtb/$name" ]; then
        copy_required "$ROOTFS/boot/dtb/$name"
    elif [ -f "$ROOTFS/boot/$name" ]; then
        copy_required "$ROOTFS/boot/$name"
    else
        echo "Missing boot asset: $name"
        exit 1
    fi
}

copy_boot_asset_optional()
{
    local name="$1"

    if [ -f "$SOURCE_DTBS/$name" ]; then
        cp -r "$SOURCE_DTBS/$name" "$PACKAGE_DIR"
    elif [ -f "$ROOTFS/boot/dtb/$name" ]; then
        cp -r "$ROOTFS/boot/dtb/$name" "$PACKAGE_DIR"
    elif [ -f "$ROOTFS/boot/$name" ]; then
        cp -r "$ROOTFS/boot/$name" "$PACKAGE_DIR"
    else
        echo "Skip missing optional boot asset: $name"
    fi
}

MODULE_DIR=$(find "$ROOTFS/lib/modules" -mindepth 1 -maxdepth 1 -type d -name '*-tegra' | sort | tail -n 1)
if [ -z "$MODULE_DIR" ]; then
    echo "Missing module directory under $ROOTFS/lib/modules"
    exit 1
fi

copy_boot_asset tegra234-p3737-camera-g300-fg96-overlay.dtbo
copy_boot_asset tegra234-p3737-camera-g300-nomtd-fg96-overlay.dtbo
copy_boot_asset tegra234-p3737-camera-g300-leopard-overlay.dtbo
copy_boot_asset tegra234-p3737-camera-g300-nomtd-leopard-overlay.dtbo
copy_boot_asset tegra234-p3767-camera-p3768-g300-fg96-overlay.dtbo
copy_boot_asset tegra234-p3767-camera-p3768-g300-nomtd-fg96-overlay.dtbo
copy_boot_asset tegra234-p3737-camera-g300-mic-fg-8g-overlay.dtbo
copy_boot_asset tegra234-p3737-camera-g300-fg96-obcam-thin-overlay.dtbo
copy_boot_asset tegra234-p3737-camera-g300-leopard-obcam-thin-overlay.dtbo
copy_boot_asset tegra234-p3737-camera-g300-fg12-overlay.dtbo

copy_boot_asset_optional tegra264-p4071-camera-g300-d317-gm-overlay.dtbo
copy_boot_asset_optional tegra264-p4071-camera-g300-mic-fg-8g-overlay.dtbo

copy_required "$MODULE_DIR/updates/drivers/media/platform/tegra/camera/tegra-camera.ko"
copy_required "$MODULE_DIR/updates/drivers/media/i2c/obc_max9296.ko"
copy_required "$MODULE_DIR/updates/drivers/media/i2c/obc_max96712.ko"
copy_required "$MODULE_DIR/updates/drivers/media/i2c/g300.ko"
copy_required "$MODULE_DIR/updates/drivers/media/i2c/obcam.ko"
copy_required "$MODULE_DIR/updates/drivers/misc/obc_cam_sync.ko"
copy_optional "$MODULE_DIR/updates/drivers/video/tegra/host/nvcsi/nvhost-nvcsi.ko"

# metadata patched kernel modules
copy_required "$MODULE_DIR/kernel/drivers/media/v4l2-core/videodev.ko"

cp copy_to_target_agx_orin_fg96.sh "$PACKAGE_DIR"
cp copy_to_target_agx_orin_fg12.sh "$PACKAGE_DIR"
cp copy_to_target_agx_orin_nomtd_fg96.sh "$PACKAGE_DIR"
cp copy_to_target_agx_orin_leopard.sh "$PACKAGE_DIR"
cp copy_to_target_agx_orin_nomtd_leopard.sh "$PACKAGE_DIR"
cp copy_to_target_orin_nx_fg96.sh "$PACKAGE_DIR"
cp copy_to_target_orin_nx_nomtd_fg96.sh "$PACKAGE_DIR"
cp copy_to_target_agx_orin_mic_fg_8g.sh "$PACKAGE_DIR"
cp copy_to_target_agx_orin_fg96_obcam.sh "$PACKAGE_DIR"
cp copy_to_target_agx_orin_leopard_obcam.sh "$PACKAGE_DIR"
cp copy_to_target_thor_dm317.sh "$PACKAGE_DIR"
cp copy_to_target_thor_mic_fg_8g.sh "$PACKAGE_DIR"

cp remove_g300.sh "$PACKAGE_DIR"
cp reconnect.sh "$PACKAGE_DIR"
cp README.md "$PACKAGE_DIR"

scp -r gmsl-driver-jetson test@xx.x.xx.xx:~/
