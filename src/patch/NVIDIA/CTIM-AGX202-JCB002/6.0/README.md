
# ORBBEC camera driver for GMSL2 interface

This driver is only applicable to Connect Tech's Rogue Carrier board(AGX202) and GMSL deserializer board(JCB002) for NVIDIA® Jetson AGX Orin™ 


## 1. Get kernel sources
  
Firstly, you must obtain the kernel source code from Connect Tech Inc.(CTIM), which is based on jetpack6.0 Linux36.3

## 2. Download gcc cross compilation tool

```
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/toolchain/aarch64--glibc--stable-2022.08-1.tar.bz2 -O aarch64--glibc--stable-final.tar.bz2
tar xf aarch64--glibc--stable-final.tar.bz2 --strip-components 1
```

## 3. The source code structure is organized as follows:
```
agx-orin-ctim-agx202-jcb002
	├── aarch64--glibc--stable-2022.08-1	(gcc, download from Nvidia official website)
	│   ├── aarch64-buildroot-linux-gnu
	│   ├── bin
	│   :
	├── Linux_for_Tegra	(Kernel src, from Connect Tech Inc.(CTIM))
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
	│       ├── Makefile	(we provided)
	│       :
	├── build_all.sh  	(we provided)
	├── copy_to_ssh.sh	(we provided)
	├── copy_to_target.sh	(we provided)
	├── cti_orin_agx202_jcb002_jp6.0_v1.0.12-0.2.patch	(we provided)
	├── extlinux.conf	(we provided)
	└── README.md
```

## 4. Apply linux GMSL driver patches

```
git apply cti_agx202_jcb002_g300_driver_v1.2.00_cti_6.0.patch
```

## 5. Prepare for compiling the kernel

To avoid the following issues:
   SSL error:02001002:system library:fopen:No such file or directory: bss_file.c:175
```
openssl req -new -nodes -utf8 -sha512 -days 36500 -batch -x509 -config x509.genkey -outform DER -out signing_key.x509 -keyout signing_key.pem
mv signing_key.pem signing_key.x509 Linux_for_Tegra/sources/kernel/kernel-jammy-src/certs/
```

## 6. Build kernel
```
./build_all.sh
```

## 7. Packaging and transferring target files

1.modify "copy_to_ssh.sh" file below IP:
  scp -r $OUTPUT_DIR orbbec@10.8.170.11:~/

2.copy to remote AGX orin target
```
./copy_to_ssh.sh
```

## 8. Update files on target AGX Orin board

```
./copy_to_target.sh
```


