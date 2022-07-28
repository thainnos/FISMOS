# Installing Vivado

# Preparation

## Check Shell

> Vivado relies on using `sh` instead of `bash`

```shell
ls -l /bin/sh
ln -sf /bin/bash /bin/sh
```

## Install `libtinfo5`

```shell
sudo apt update && sudo apt upgrade -y
sudo apt install libtinfo5 -y
```

## Install Xilinx Tools

> This section applies to a graphical installation, check further down in this document for headless version

* Get `2020.3` version from Download Section [Link](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2020-3.html)
* Extract download with e.g. `tar -xvf Xilinx_Unified_*.tar.gz`
* Make the installer executable `chmod +x xsetup` && run it!

---

## Install board files for PYNQ-Z2 board

```shell
cp -r ./additional_board_files/ $INSTALLDIR/Vivado/$VERSION/data/boards/board_files/
```

---

## USB-Cable Drivers for e.g. Hardware Manager

* run driver installation `cd /opt/Xilinx/Vivado/2020.2/data/xicom/cable_drivers/lin64/install_script/install_drivers/ && sudo ./install_drivers`
* Restart the editor!

---

## Troubleshooting

### Get patch for multi-monitor usage

> Vivado, respectively `Java` will crash on startup, if more than two monitors are used!

```shell
cd /home/$USER/
curl https://www.xilinx.com/Attachment/AR72614_vivado_2019_2_preliminary_rev2.zip -o AR72614.zip
unzip AR72614.zip

dir="/home/$USER/.Xilinx/patch/"
mkdir $dir

cd AR72614
cp -r . $dir

text="-jvm -patch-module=java.desktop=/home/$USER/.Xilinx/patch/vivado/lib/classes/AR72614/AR72614.jar"\
echo $text >> /home/$USER/Desktop/'Vivado 2019.2.desktop'
```

## Language problem

Unexpected commas in the xdc files may cause crashes. This points to a localization problem. 
To overcome this, change the `$LANG environment` variable to `en_US.UTF-8`. 

If this doesn't help, replace the $LC_NUMERIC variable. I.e. something like this:

```shell
source ~/xilinx/Vivado/2020.2/settings64.sh
unset LC_NUMERIC
export LC_NUMERIC=en_US.UTF-8
vivado &
```

---

## Headless Installation

### Install Tools

> This section applies to a headless installation, check further upwards in this document for graphical version

1. Download Webinstaller from Xilinx [Link](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2020-3.html)
2. If required: Copy to remote and connect via SSH
3. Extract webinstaller with `./Xilinx_Unified_2020.2_1118_1232_Lin64.bin --target ./Xilinx`
4. Create initial config with `xsetup -b ConfigGen` and adjust on personal preferences
5. Run `./xsetup -b AuthTokenGen --location <DESTINATION>` to setup Xilinx account
6. Run setup with `./xsetup -c <CONFIG_LOCATION> xsetup -a XilinxEULA,3rdPartyEULA,WebTalkTerms -b Install --location <DESTINATION>`

### Run script for driver install for HW manager

```shell
cd $INSTALLDIR"Vivado/$VERSION/data/xicom/cable_drivers/lin64/install_script/install_drivers/"
sudo ./install_drivers
```
