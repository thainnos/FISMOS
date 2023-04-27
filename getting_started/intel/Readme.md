# Installing Quartus

> This write up is based on [http://www.armadeus.org/wiki/index.php?title=Quartus_installation_on_Linux]

## Quartus

* ownload Quartus Prime Lite from [https://www.intel.de/content/www/de/de/software/programmable/quartus-prime/overview.html]
* Unpack and install it to `/opt/intelFPGA_lite/`!

### `libpng12.so.0` is missing

```shell
sudo add-apt-repository ppa:linuxuprising/libpng12
sudo apt update
sudo apt install libpng12-0
```

### Add Quartus to bash

`sudo nano ~/.bashrc`

```shell
# Quartus 18.1 in /opt/intelFPGA_lite
export QSYS_ROOTDIR="/opt/intelFPGA_lite/18.1/quartus/sopc_builder/bin"

export ALTERAPATH="/opt/intelFPGA_lite/18.1/"
export ALTERAOCLSDKROOT="${ALTERAPATH}/hld"
export QUARTUS_ROOTDIR=${ALTERAPATH}/quartus
export QUARTUS_ROOTDIR_OVERRIDE="$QUARTUS_ROOTDIR"
export PATH=$PATH:${ALTERAPATH}/quartus/bin
export PATH=$PATH:${ALTERAPATH}/nios2eds/bin
export PATH=$PATH:${QSYS_ROOTDIR}

# Set variables for Quartus 20.1
export QUARTUS_ROOTDIR=/opt/intelFPGA_lite/20.1/quartus
export QUARTUS=/opt/intelFPGA_lite/20.1/quartus
export QUARTUS_64BIT=1

export NIOS2EDS=/opt/intelFPGA_lite/20.1/nios2eds
export SOPC_KIT_NIOS2=/opt/intelFPGA_lite/20.1/nios2eds
export NIOS2LINUX=/opt/intelFPGA_lite/20.1/nios2eds

export PATH=$PATH:/opt/intelFPGA_lite/20.1/quartus/bin/
export PATH=$PATH:/opt/intelFPGA_lite/20.1/modelsim_ase/bin/

# Modelsim in /opt/intelFPGA_lite
export MODELSIMPATH=${ALTERAPATH}/modelsim_ase
export PATH=$PATH:${MODELSIMPATH}/bin
```

```shell
source ~/.bashrc
```

### Cable Drivers for USB Blaster I & II

Add a new file `/etc/udev/rules.d/92-usbblaster.rules` file.  
Note that the 92 is important for e.g. Debian Wheezy as there is a default rule for "usbfs-like" devices `/lib/udev/rules.d/91-permissions.rules` specifying a MODE of 664.  
Because udev assigns priority to rules by the sort order of the file names theyappear in the USB Blaster rules have to be placed in an appropriate file.

````shell
# Intel FPGA Download Cable (USB-Blaster)
SUBSYSTEM=="usb", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6001", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6002", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6003", MODE="0666"

# Intel FPGA Download Cable II (USB-Blaster 2)
SUBSYSTEM=="usb", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6010", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6810", MODE="0666"
````

---

## Modelsim

* Open Quartus and go to Tools>Options>EDA Tool Options, and make sure the path for ModelSim-Altera is set to `/<YOUR_DIRECTORY>/modelsim_ase/bin`
* Go to Assignments>Settings>EDA Tool Settingsand make sure the Simulation Tool Name is set to ModelSim-Altera

## In case of error: `could not find ./../linux_rh60/vsim`

* Modify `/opt/intelFPGA_lite/18.1/modelsim_ase/bin/vsim`:
  * `vco="linux_rh60"` to `vco="linux"`
  * `mode=${MTI_VCO_MODE:-" "}` to `mode=${MTI_VCO_MODE:-"32"}`

### Install dependencies

```shell
sudo dpkg --add-architecture i386
sudo apt-get update

sudo apt-get install build-essential

sudo apt-get install gcc-multilib g++-multilib \
lib32z1 lib32stdc++6 lib32gcc1 \
expat:i386 fontconfig:i386 libfreetype6:i386 libexpat1:i386 libc6:i386 libgtk-3-0:i386 \
libcanberra0:i386 libpng12-0:i386 libice6:i386 libsm6:i386 libncurses5:i386 zlib1g:i386 \
libx11-6:i386 libxau6:i386 libxdmcp6:i386 libxext6:i386 libxft2:i386 libxrender1:i386 \
libxt6:i386 libxtst6:i386 gcc-multilib g++-multilib lib32z1 lib32stdc++6 lib32gcc1 \
expat:i386 fontconfig:i386 libfreetype6:i386 libexpat1:i386 libc6:i386 libgtk-3-0:i386 \
libcanberra0:i386 libice6:i386 libsm6:i386 libncurses5:i386 zlib1g:i386 libx11-6:i386 \
libxau6:i386 libxdmcp6:i386 libxext6:i386 libxft2:i386 libxrender1:i386 libxt6:i386 \
libxtst6:i386 libxtst6:i386
```

> Note: This also should include `Freetype`. If it doesn't do so, check next heading.

#### If error: `** Fatal: Read failure in vlm process (0,0)`

* Get current version from e.g. [GNU.org]
* Link: <https://download-mirror.savannah.gnu.org/releases/freetype/>

```shell
tar xvf freetype-2.4.12
cd  freetype-2.4.12
./configure --build=i686-pc-linux-gnu "CFLAGS=-m32" "CXXFLAGS=-m32" "LDFLAGS=-m32"
make -j8
sudo mkdir lib32
sudo cp ./objs/.libs/libfreetype.so* /opt/intelFPGA_lite/18.1/modelsim_ase/lib32
sudo nano /opt/intelFPGA_lite/18.1/modelsim_ase/bin/vsim
   # Add line `export LD_LIBRARY_PATH=${dir}/lib32` under `export LD_LIBRARY_PATH=${dir}/lib32`
cd ..
rm -rf freetype-2.4.12
```
