# ORBBEC camera driver for GMSL


## 1. Download the JetPack 6.0 source code and toolchain.

```
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/toolchain/aarch64--glibc--stable-2022.08-1.tar.bz2 -O aarch64--glibc--stable-final.tar.bz2
wget https://developer.nvidia.com/downloads/embedded/l4t/r36_release_v3.0/sources/public_sources.tbz2
```
## 2. Tar the source code and toolchain.
```
tar xjf public_sources.tbz2
tar xf aarch64--glibc--stable-final.tar.bz2 --strip-components 1
```

copy kernel_src.tbz2 kernel_oot_modules_src.tbz2 nvidia_kernel_display_driver_source.tbz2 to Linux_for_Tegra/sources
```
cd Linux_for_Tegra/sources
tar xjf kernel_src.tbz2
tar xjf kernel_oot_modules_src.tbz2
tar xjf nvidia_kernel_display_driver_source.tbz2
cd ../..
```

## Folder like this:

├── aarch64--glibc--stable-2022.08-1
├── advan_amr-s100_g300_driver_v1.0.1.patch
├── build_all.sh
├── copy_to_jetson.sh
├── copy_to_target_agx_orin_s100.sh
├── Linux_for_Tegra
│   └── sources
│       ├── generic_rt_build.sh
│       ├── hardware
│       ├── hwpm
│       ├── kernel
│       ├── kernel_src_build_env.sh
│       ├── Makefile
│       ├── nvbuild.sh
│       ├── nvcommon_build.sh
│       ├── nvdisplay
│       ├── nvethernetrm
│       ├── nvgpu
│       └── nvidia-oot
├── README.md
├── reconnect.sh
└── x509.genkey

## 3. Apply G335Lg patches

```
patch -p1 < advan_amr-s100_g300_driver_v1.0.1.patch
```

## 4. Prepare for compiling the kernel
```
sudo apt install build-essential bc flex bison
```

To avoid the following issues:
   SSL error:02001002:system library:fopen:No such file or directory: bss_file.c:175
```
openssl req -new -nodes -utf8 -sha512 -days 36500 -batch -x509 -config x509.genkey -outform DER -out signing_key.x509 -keyout signing_key.pem
mv signing_key.pem signing_key.x509 Linux_for_Tegra/sources/kernel/kernel-jammy-src/certs/
```

## 5. Build kernel Debian packages
```
./build_all.sh
```

## 6. Copy the updated files to the folder(gmsl-driver-jetson)
```
./copy_to_jetson.sh
```

## 7. Install kernel and G335Lg driver to Jetson Orin

Copy them to the target Jetson Orin:

```
# Copy all files to the target board via SSH
scp -r gmsl-driver-jetson ubuntu@192.168.1.88:~/
```

on target Jetson Orin Copy them to the right places:

```
cd gmsl-driver-jetson
./copy_to_target_agx_orin_s100.sh
```
