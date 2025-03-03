#!/bin/bash

OUTPUT_DIR=gmsl-driver

if [ ! -d $OUTPUT_DIR ];then
    mkdir $OUTPUT_DIR
fi

# Packaging & copy driver modules
rm $OUTPUT_DIR/5.15.136-tegra.tar.gz
tar -czf $OUTPUT_DIR/5.15.136-tegra.tar.gz -C images/6.0/rootfs/lib/modules 5.15.136-tegra
rm $OUTPUT_DIR/updates.tar.gz
tar -czf $OUTPUT_DIR/updates.tar.gz -C images/6.0/rootfs/lib/modules/5.15.136-tegra updates

# Copy videodev.ko
cp images/6.0/rootfs/lib/modules/5.15.136-tegra/kernel/drivers/media/v4l2-core/videodev.ko $OUTPUT_DIR

# Copy dtb
cp images/6.0/rootfs/boot/dtb/tegra234-orin-agx-cti-AGX202-JCB002-G300.dtb $OUTPUT_DIR

# Copy kernel image
cp images/6.0/rootfs/boot/Image $OUTPUT_DIR

# Copy other files
cp extlinux.conf $OUTPUT_DIR
cp copy_to_target.sh $OUTPUT_DIR

# Copy to remote target board by SSH
scp -r $OUTPUT_DIR orbbec@10.8.170.72:~/

