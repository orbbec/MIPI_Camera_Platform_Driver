if [ -e /sys/module/g300 ]; then
	echo "Files g300 has loaded,unload g300"
	sudo rmmod g300
	sudo rmmod max9296
	sudo rmmod max9295
	echo "reconnect g300"
	sudo insmod /lib/modules/5.15.148-tegra/updates/drivers/media/i2c/max9296.ko
	sudo insmod /lib/modules/5.15.148-tegra/updates/drivers/media/i2c/max9295.ko
	sudo insmod /lib/modules/5.15.148-tegra/updates/drivers/media/i2c/g300.ko
else
	echo "connect g300"
	sudo insmod /lib/modules/5.15.148-tegra/updates/drivers/media/i2c/max9296.ko
	sudo insmod /lib/modules/5.15.148-tegra/updates/drivers/media/i2c/max9295.ko
	sudo insmod /lib/modules/5.15.148-tegra/updates/drivers/media/i2c/g300.ko
fi