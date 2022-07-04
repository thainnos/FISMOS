# Installing Petalinux

# Preparation

## Installint additional packages

> Note: This script is based on prior installing Vivado toolchain, see specific [Readme_FPGA.md](./Readme_FPGA.md)

```shell
sudo apt install netstat-* xterm gcc-multilib ncurses-* zlib1g:i386 net-tools -y
```

> Note: `ncurses` may convert to `libncurses-dev` (Page 10: [https://www.xilinx.com/support/documentation/sw_manuals/petalinux2014_2/ug976-petalinux-installation.pdf])

## Get installer


* Check Homepage of Xilinx for Downloads here: [Link](https://www.xilinx.com/downloadNav/embedded-design-tools/2021-1.html)


* Make executable with `sudo chmod +x ./petalinux-v*`
* Run `./petalinux-v****.*-final-installer.run -d /opt/Xilinx/Petalinux`
* **Alternative**: Move installer to `/opt/Xilinx/Petalinux/` and run `./petalinux-v2020.2-final-installer.run` 
  
* Confirm License Agreement with `q -> y -> Enter`
* **INSTALL!**
* You may ignore TFTP Error Message (Page 14: [https://www.xilinx.com/support/documentation/sw_manuals/xilinx2020_1/ug1144-petalinux-tools-reference-guide.pdf])

## Running

* Source settings with `source /opt/Xilinx/Petalinux/settings.sh`
* Make use of e.g. `petalinux-create --help`