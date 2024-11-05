
# ORBBEC camera driver for GMSL2 interface

This driver is only applicable to Connect Tech's Rogue Carrier board(AGX202) and GMSL deserializer board(JCB002) for NVIDIA® Jetson AGX Orin™ 


##1. Get kernel sources
  
Firstly, you must obtain the kernel source code from Connect Tech Inc.(CTIM), which is based on jetpack6.0 Linux36.3

##2. Download gcc cross compilation tool

```
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/toolchain/aarch64--glibc--stable-2022.08-1.tar.bz2 -O aarch64--glibc--stable-final.tar.bz2
tar xf aarch64--glibc--stable-final.tar.bz2 --strip-components 1
```

##3. The source code structure is organized as follows:

agx-orin-ctim-agx202-jcb002
	├── aarch64--glibc--stable-2022.08-1 (gcc)
	│   ├── aarch64-buildroot-linux-gnu
	│   ├── bin
	│   :
	├── Linux_for_Tegra (kernel src)
	│   └── sources
	│	    ├── cti
	│       ├── hardware
	│       ├── hwpm
	│       ├── kernel
	│       │   ├── kernel-jammy-src
	│       │   └── Makefile
	│       ├── laird
	│       ├── nvdisplay
	│       ├── nvethernetrm
	│       ├── nvgpu
	│       ├── nvidia-oot
	│       ├── Makefile
	│       :
	├── build_all.sh
	├── copy_to_ssh.sh
	├── copy_to_target.sh
	├── cti_orin_agx202_jcb002_jp6.0_v1.0.12-0.2.patch
	├── extlinux.conf
	└── README.md

##4. Apply linux GMSL driver patches

```
git apply cti_agx202_jcb002_g2xx_driver_v1.0.12-0.2.patch
```

##5. Build kernel
```
./build_all.sh
```

##6. Packaging and transferring target files

1.modify "copy_to_ssh.sh" file below IP:
  scp -r $OUTPUT_DIR orbbec@10.8.170.11:~/

2.copy to remote AGX orin target
```
./copy_to_ssh.sh
```

##7. Update files on target AGX Orin board

```
./copy_to_target.sh
```


