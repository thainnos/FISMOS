# How to compile PetaLinux

Using either BSP (Board Support Package) or platform

```shell
#!/bin/bash

# Configure bash
set -e

###
# Set up directories
export PETALINUX_BASE=/opt/Xilinx/Petalinux/
export VIVADO_BASE=/opt/Xilinx/Vivado/2021.2
export HW_DESCRIPTION_BASE=<Path_to_XSA_file>

# Define names
export projectname_petalinux=fismos_v0_petalinux
export hwdescription=fismos_top_wrapper

###
# Source settings
source $PETALINUX_BASE/settings.sh
source $VIVADO_BASE/settings64.sh

###
# Running Petalinux
# Create Project
petalinux-create --type project --template zynq --name BUILD_$projectname_petalinux
cd BUILD_$projectname_petalinux

# Subsystem level configuration
petalinux-config --get-hw-description=$HW_DESCRIPTION_BASE/ --silentconfig
# Kernel
petalinux-config -c kernel --silentconfig
# Configuration of RootFS
petalinux-config -c rootfs --silentconfig

# Get shit done!!
petalinux-build
# ....This may take some time! Grab yourself some coffee! 

# Package and copy images
petalinux-package --boot --fsbl ./images/linux/zynq_fsbl.elf --fpga ./images/linux/$hwdescription.bit --u-boot --force 
```

---
## Run with Qemu

```shell
petalinux-boot --qemu --kernel
```

---
## Configure Linux

### Menuconfig

* `Memory Size`
  * Change to...[TBD]


### RootFS

* `GCC`: 
  * misc -> packagegroup-core-buildessential -> packagegroup-core-buildessential
  * misc -> packagegroup-core-buildessential -> packagegroup-core-buildessential-dev

* Add `IMAGE_INSTALL_append = "kernel-devsrc"` to `project-spec/meta-user/conf/layer.conf`, if Linux headers are needed


### Kernel

* Add `GCC-Scripts` to `kernel/config`
  * `CONFIG_GCC_PLUGINS=y`
  * `CONFIG_GCC_PLUGIN_LATENT_ENTROPY=y`
  * `HAVE_GCC_PLUGINS=y`

---
## General info

`petalinux-package` generates a device tree binary DTB file, a First Stage Bootloader(FSBL), U-Boot, the
Linux kernel FIT image, and compressed root filesystem . Finally, it generates the necessary boot images :
1. BOOT.BIN : A binary file which is responsible for loading the FPGA bitstream, the FSBL and the U-Boot.
2. Image.ub : Contains compressed kernel and device tree

---
## Prepare SD-Card

```shell
sudo fdisk /dev/<your-card>
```

**d -> n -> ENTER -> ENTER -> ENTER -> +300M (-> Y) -> t -> c -> n -> ENTER -> ENTER -> ENTER -> ENTER -> t -> 2 -> 83 -> w**

## Configure partitions

```shell
sudo mkfs.vfat /dev/<your-card>1
sudo mkfs.ext4 /dev/<your-card>2
```

## Copy files to SD-Card

```shell
sudo chmod 777 /dev/<your-card>1
sudo chmod 777 /dev/<your-card>2
cp ./images/linux/BOOT.BIN /dev/<your-card>1
cp ./images/linux/image.ub /dev/<your-card>1
cp ./images/linux/boot.scr /dev/<your-card>1
cp ./images/linux/u-boot.bin/dev/<your-card>1
s
cp ./images/linux/zImage /dev/<your-card>2
cp ./images/linux/devicetree.dtb /dev/<your-card>2
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
```mandorin
.
├── BOOT
│   ├── BOOT.BIN
│   └── image.ub
└── rootfs
    └── zImage
```

## Insert card into ZedBoard and power up

## Connect via UART and check for IP adress

```shell
minicom -D /dev/ttyUSB1 -b 115200
ifconfig
```

## Connect via SSH

```shell
ssh root@<IP-adress>
```

---
# Variant for SD-card preparation

These instructions assume you are using a Linux development machine.

The example below will show the steps for setting up a brand new card.  We will be creating 2 partitions on the card: a FAT partition that will contain the bootloaders and Linux kernel, and an ext3 partition that will contain the root file system.

GETTING STARTED
First insert your card into your development machine’s flash card slot. If using a microSD card you may need to use a microSD to SD card adaptor to fit your slot.

On my Ubuntu machine, the newly inserted card shows up as /dev/sde and that is the device name that will be used through this example. You should substitute the proper device name for your machine. You can use ‘mount’ or ‘df’ to see where the card mounts on your machine.

Let’s unmount the device’s existing file system before we get started with the process:

$ sudo umount /dev/sde1
If you are the type who likes to understand exactly what is going on, you can follow the manual process below.  If you just want to get the job done, you can use a script that automates this process.  See the article entitled  A script for partitioning/formatting a bootable SD/miroSD card, and then return to the INSTALLING THE BOOT FILES section below.

PARTITIONING THE CARD
Now launch fdisk and create an empty partition table. Note that the argument for fdisk is the entire device (/dev/sde) not just a single partition (i.e. /dev/sde1):

# sudo fdisk /dev/sde

Command (m for help): o
Building a new DOS disklabel. Changes will remain in memory only,
until you decide to write them. After that, of course, the previous
content won't be recoverable.

Warning: invalid flag 0x0000 of partition table 4 will be corrected by w(rite)
Let’s first look at the current card information:

Command (m for help): p

Disk /dev/sde: 2032 MB, 2032664576 bytes
64 heads, 63 sectors/track, 984 cylinders
Units = cylinders of 4032 * 512 = 2064384 bytes
Disk identifier: 0x00aa8e5c

   Device Boot      Start         End      Blocks   Id  System
Note the card size in bytes. We will needed it later in the process.

In order to create a bootable SD/microSD compatible with the OMAP3 boot ROM you must set a special geometry using the fdisk “Expert mode”.

To go into “Expert” mode use the “x” command:

Command (m for help): x
Next we will set the geometry to 255 heads, 63 sectors and a calculated value for the number of cylinders required for the particular microSD card.

To calculate the number of cylinders, we take the 2032664576 bytes reported above by fdisk divided by 255 heads, 63 sectors and 512 bytes per sector:

2032664576 / 255 / 63 / 512 = 247.12 which we round down to 247 cylinders.

Expert command (m for help): h
Number of heads (1-256, default 4): 255

Expert command (m for help): s
Number of sectors (1-63, default 62): 63
Warning: setting sector offset for DOS compatiblity

Expert command (m for help): c
Number of cylinders (1-1048576, default 984): 247
Return to fdisk’s main mode and create a new partition 32 MB FAT partition:

Expert command (m for help): r

Command (m for help): n
Command action
   e   extended
   p   primary partition (1-4)
p
Partition number (1-4): 1
First cylinder (1-247, default 1): 1
Last cylinder or +size or +sizeM or +sizeK (1-247, default 15): +32M
Change the partition type to FAT32:

Command (m for help): t
Selected partition 1
Hex code (type L to list codes): c
Changed system type of partition 1 to c (W95 FAT32 (LBA))
And mark it bootable:

Command (m for help): a
Partition number (1-4): 1
Next we create an ext3 partition for the rootfs:

Command (m for help): n
Command action
   e   extended
   p   primary partition (1-4)
p
Partition number (1-4): 2
First cylinder (6-247, default 6): 6
Last cylinder or +size or +sizeM or +sizeK (6-247, default 247): 247
To verify our work, lets print the partition info:

Command (m for help): p

Disk /dev/sde: 2032 MB, 2032664576 bytes
255 heads, 63 sectors/track, 247 cylinders
Units = cylinders of 16065 * 512 = 8225280 bytes
Disk identifier: 0x00aa8e5c

   Device Boot      Start         End      Blocks   Id  System
/dev/sde1   *           1           5       40131    c  W95 FAT32 (LBA)
/dev/sde2               6         247     1943865   83  Linux
Up to this point no changes have been made to the card itself, so our final step is to write the new partition table to the card and then exit:

Command (m for help): w
The partition table has been altered!

Calling ioctl() to re-read partition table.

WARNING: If you have created or modified any DOS 6.x
partitions, please see the fdisk manual page for additional
information.
Syncing disks.
FORMATTING THE NEW PARTITIONS
We format the first partition as a FAT file system (the -n parameter gives it a label of FAT, you can change or omit this if you like):

$  sudo mkfs.vfat -F 32 /dev/sde1 -n FAT
mkfs.vfat 2.11 (12 Mar 2005)
We format the second partition as an ext3 file system:

$ sudo mkfs.ext3 /dev/sde2
mke2fs 1.40.8 (13-Mar-2008)
Filesystem label=
OS type: Linux
Block size=4096 (log=2)
Fragment size=4096 (log=2)
121920 inodes, 485966 blocks
24298 blocks (5.00%) reserved for the super user
First data block=0
Maximum filesystem blocks=499122176
15 block groups
32768 blocks per group, 32768 fragments per group
8128 inodes per group
Superblock backups stored on blocks: 
	32768, 98304, 163840, 229376, 294912

Writing inode tables: done                            
Creating journal (8192 blocks): done
Writing superblocks and filesystem accounting information: ^[done

This filesystem will be automatically checked every 36 mounts or
180 days, whichever comes first.  Use tune2fs -c or -i to override.
INSTALLING THE BOOT FILES
This instructions below will assume that:

your machine is an Overo
you downloaded the Overo images and Gnome rootfs from the Downloads section of this site
Make changes as appropriate if you are using a different machine or have downloaded the files to a different location. You will also need to adjust the revision number in the downloaded filenames.

There are three files required on the first (FAT) partition to boot your OMAP3 processor:

MLO: the boot-loader loader – this small program is loaded into the OMAP3 processor’s static RAM. It does some minimal configuration of system memory and io pins and then loads the second file.
u-boot.bin: the boot loader
uImage: the linux kernel
It is important that these three files have precisely these names.

Mount the FAT partition of your microSD card. This example will assume that you have mounted it at /media/card:

sudo mount /dev/sde1 /media/card
Due to constraints of the mask boot ROM in the OMAP processor, MLO should be written first:

$ sudo cp ~/Desktop/MLO-overo /media/card/MLO
Then copy u-boot and the linux kernel to the card:

$ sudo cp ~/Desktop/u-boot-overo.bin /media/card/u-boot.bin
$ sudo cp ~/Desktop/uImage-overo-2.6.X-rX.bin /media/card/uImage
You can now unmount the FAT partition:

$ sudo umount /dev/sde1
At this point you have a bootable FAT partition.

INSTALLING THE ROOT FILE SYSTEM
The final step is to untar your rootfs onto the ext3 partition that you created above.

Note that this step can be dangerous. You do not want to untar your OMAP3 rootfs onto the root of your development machine file system.  Be careful!

$ sudo mount /dev/sde2 /media/card
Now untar your rootfs:

$ cd /media/card
$ sudo tar xvjf ~/Desktop/gnome-rX.tar.bz2
You can now unmount the ext3 partition:

$ sudo umount /dev/sde2
It is now safe to remove the SD/microSD card from your development machines card slot.

Insert the card in your OMAP3 machine and press the reset button (on Beagle be sure to hold down the User button when pressing and releasing the reset button in order to ensure that the Beagle boots from the SD card)

After a short while you will see a login screen.  Login as user root with password root.  If you desire, you can then use the System/Administration/Users and Groups menu item to create a normal user.

Have fun exploring!

To change your display resolution see the article How to change the display resolution.

But please read the section below first!

A FEW WORDS ABOUT THE U-BOOT ENVIRONMENT PARTITION
It is recommended that you erase the u-boot environment partition every time you install a new version of u-boot.  This will cause u-boot to use its default environment.

The images on this site are always matched sets – the u-boot default enviroment will be appropriate for the kernel and rootfs in the set.  You should only change the default u-boot environment if you know what you are doing.  You shouldn’t normally need to change this default unless you want to use a different display resolution than the default 1024×768.

To erase the u-boot environment partition, press any key during the boot sequence when u-boot prints the following message:

Hit any key to stop autoboot:
Then, at the command prompt for Overo type:

Overo # nand erase 240000 20000

NAND erase: device 0 offset 0x240000, size 0x20000
Erasing at 0x240000 -- 100% complete.
OK
Overo # reset
For Beagle, type:

OMAP3 beagleboard.org # nand erase 260000 20000

NAND erase: device 0 offset 0x260000, size 0x20000
Erasing at 0x260000 -- 100% complete.
OK
OMAP3 beagleboard.org # reset

Your machine will now reboot and you will see confirmation thas you have successfully erased the environment partition whe u-boot prints the following message:

*** Warning - bad CRC or NAND, using default environment
Source : http://www.sakoman.com/OMAP/preparing-a-bootable-sd-card.html
