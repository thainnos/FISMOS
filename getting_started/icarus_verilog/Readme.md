# Icarus Verilog

[Icarus Verilog](http://iverilog.icarus.com/) is a `Verilog` synthesis and simulation tool.

## Install from Package Manager

### Ubuntu

```shell
sudo apt-get install verilog
```

> Check e.g. [Fandom](https://iverilog.fandom.com/wiki/Installation_Guide) for other OS

---
## Install from Source on Linux

> Script based on https://github.com/steveicarus/iverilog

``` shell
## Get sources
sudo apt install autoconf gperf flex bison
sudo apt install wget unzip
wget https://github.com/steveicarus/iverilog/archive/refs/tags/v11_0.zip
unzip v11_0.zip
rm -f v11_0.zip

## Build and install
cd iverilog-11_0
sh autoconf.sh
./configure
make
sudo make install

## Housekeeping
cd ../
rm -r iverilog-11_0/
```

