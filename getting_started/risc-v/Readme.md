# Installing the RISC-V compiler on Debian-based System

> Based on https://github.com/cliffordwolf/picorv32#building-a-pure-rv32i-toolchain

```shell
# Ubuntu packages needed:
sudo apt-get install -y autoconf automake autotools-dev curl libmpc-dev \
    libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo \
    gperf libtool patchutils bc zlib1g-dev git libexpat1-dev

# Create directory
sudo mkdir /opt/riscv32i
sudo chown $USER:$USER /opt/riscv32i

# Get sources
cd ~/Documents
git clone https://github.com/riscv/riscv-gnu-toolchain riscv-gnu-toolchain-rv32i
cd riscv-gnu-toolchain-rv32i
git checkout 411d134
git submodule update --init --recursive

# Build and install
mkdir build && cd build
../configure --with-arch=rv32i --prefix=/opt/riscv32i
make -j$(nproc)

# Housekeeping
cd ~/Documents
rm -rf ./riscv-gnu-toolchain-rv32i
```
