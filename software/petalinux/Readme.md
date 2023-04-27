# How to compile PetaLinux for FISMOS

Using either BSP (Board Support Package) or platform

```shell
#!/bin/bash

# Configure bash
set -e

###
# Set up directories
export PETALINUX_BASE=/opt/petalinux
export VIVADO_BASE=/opt/xilinx/Vivado/2022.1
export HW_DESCRIPTION_BASE=<Path_to_XSA_file>

# Define names
export projectname_petalinux=fismos_v0_petalinux
export hwdescription=fismos_top_wrapper
export bitstream=system

###
# Source settings
source $VIVADO_BASE/settings64.sh
source $PETALINUX_BASE/settings.sh

###
# Running Petalinux
# Create Project
petalinux-create --type project --template zynq --name BUILD_$projectname_petalinux
cd BUILD_$projectname_petalinux

# Subsystem level configuration
petalinux-config --get-hw-description=$HW_DESCRIPTION_BASE --silentconfig
# Kernel
petalinux-config -c kernel  --silentconfig
# Configuration of RootFS
petalinux-config -c rootfs  --silentconfig

# Get shit done!!
petalinux-build
# ....This may take some time! Grab yourself some coffee! 

# Package and copy images
petalinux-package --boot --fsbl ./images/linux/zynq_fsbl.elf --fpga ./images/linux/$bitstream.bit --u-boot --force 
```

---

## Run with Qemu

```shell
petalinux-boot --qemu --kernel
petalinux-boot --qemu --u-boot
```

---

## Configure Linux for building LKMs

### RootFS

`petalinux-config -c rootfs`  

* `GCC`:
  * Filesystem Packages -> misc -> packagegroup-core-buildessential -> [*] packagegroup-core-buildessential
  * Filesystem Packages -> misc -> packagegroup-core-buildessential -> [*] packagegroup-core-buildessential-dev
  * Yocto Settings ---> [*] Enable Debug Tweaks

* Add `IMAGE_INSTALL:append = "kernel-devsrc"` to `project-spec/meta-user/conf/layer.conf`, if Linux headers are needed
* **Attention:** check [migration guide](https://docs.yoctoproject.org/next/migration-guides/migration-3.4.html) for updated PetaLinux
* In this release, the : character replaces the use of _ to refer to an override, most commonly when making a conditional assignment of a variable.

### Kernel

* Add `GCC-Scripts` to `kernel/config`
  * `CONFIG_GCC_PLUGINS=y`
  * `CONFIG_GCC_PLUGIN_LATENT_ENTROPY=y`
  * `HAVE_GCC_PLUGINS=y`

---

## General info

`petalinux-package` generates a device tree binary DTB file, a First Stage Bootloader(FSBL), U-Boot, the Linux kernel FIT image, and compressed root filesystem.  
Finally, it generates the necessary boot images :

1. BOOT.BIN : A binary file which is responsible for loading the FPGA bitstream, the FSBL and the U-Boot.
2. Image.ub : Contains compressed kernel and device tree

---

## Prepare SD-Card

```shell
export sdcard=sdb
export bootcard=sdb1
export rootfscard=sdb2

sudo fdisk /dev/$sdcard
```

Workflow: **d -> n -> ENTER -> ENTER -> ENTER -> +300M (-> Y) -> t -> c -> n -> ENTER -> ENTER -> ENTER -> ENTER -> t -> 2 -> 83 -> w**

## Configure partitions

```shell
sudo mkfs.vfat /dev/$bootcard
sudo mkfs.ext4 /dev/$rootfscard
```

## Copy files to SD-Card

```shell
sudo chmod 777 -R /dev/$bootcard
sudo chmod 777 -R /dev/$rootfscard
cp ./images/linux/BOOT.BIN /dev/$bootcard
cp ./images/linux/image.ub /dev/$bootcard
cp ./images/linux/boot.scr /dev/$bootcard
cp ./images/linux/u-boot.bin /dev/$bootcard
sync
cp ./images/linux/zImage /dev/$rootfscard
cp ./images/linux/system.dtb /dev/$rootfscard
sync
```

## Final SD-card setup

### WORKING

```shell
.
├── BOOT.BIN
├── boot.scr
├── image.ub
└── u-boot.bin
```

### Not Working

```shell
.
├── BOOT
│   ├── BOOT.BIN
│   └── image.ub
└── rootfs
    └── zImage
```

## Insert card into ZedBoard and power up

## Connect via UART and check for IP address

```shell
minicom -D /dev/ttyUSB1 -b 115200
ifconfig
```

## `IfConfig`

```shell
ifconfig eth0
sudo ifconfig eth0 192.168.0.199 up
sudo ifconfig eth0 netmask 255.255.255.0
sudo ifconfig eth0 up
```

---

## Connect via SSH

```shell
ssh root@<IP-adress>
```
