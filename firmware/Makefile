RISCV_TOOLS_PREFIX = /opt/riscv32i/bin/riscv32-unknown-elf-
SCRIPT_DIR = ../scripts
CC = $(RISCV_TOOLS_PREFIX)gcc
CCFLAGS = -MD -Os -Wall -I ./include/psa -I ./include/mbedtls -I ./include -I ./library
LDFLAGS = -Wl,--gc-sections
LDLIBS = -I ./include/psa -I ./include/mbedtls -I ./include -I ./library

SUBDIR = ./library
SRCS = $(wildcard *.c $(foreach fd, $(SUBDIR), $(fd)/*.c)) fismos.c
OBJS = $(SRCS:.c=.o)

.PHONY: clean

FISMOS_firmware.hex: firmware.elf start.elf
	$(RISCV_TOOLS_PREFIX)objcopy -O verilog start.elf start.tmp
	$(RISCV_TOOLS_PREFIX)objcopy -O verilog firmware.elf firmware.tmp
	cat start.tmp firmware.tmp > firmware.hex
	python3 $(SCRIPT_DIR)/hex8tohex32.py firmware.hex > FISMOS_firmware.hex
	rm -f start.tmp firmware.tmp

firmware.elf: $(OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^ -T riscv.ld
	chmod -x firmware.elf

# Default C source file compilation
.c.o:
	$(CC) $(CCFLAGS) -c -o $@ $^ $(LDLIBS)
	chmod -x $<

# Bootloader
start.elf: start.S start.ld
	$(CC) -nostdlib -o start.elf start.S -T start.ld $(LDLIBS)
	chmod -x start.elf

# Set Up mbedtls
get_mbedtls_for_FISMOS_softcore:
	git clone https://github.com/Mbed-TLS/mbedtls.git DOWNLOAD_mbedtls -b v2.28.0
	mkdir -p include library
	mv DOWNLOAD_mbedtls/include .
	mv DOWNLOAD_mbedtls/library/*.h ./library
	mv DOWNLOAD_mbedtls/library/*.c ./library
	rm -rf DOWNLOAD_mbedtls
	# See ./Readme.md, why this is done
	sed -i 's/#define MBEDTLS_NET_C/\/\/ #define MBEDTLS_NET_C/g' include/mbedtls/config.h
	sed -i 's/\/\/#define MBEDTLS_NO_PLATFORM_ENTROPY/#define MBEDTLS_NO_PLATFORM_ENTROPY/g' include/mbedtls/config.h
	sed -i 's/#define MBEDTLS_TIMING_C/\/\/ #define MBEDTLS_TIMING_C/g' include/mbedtls/config.h
	mv ./library/x509_crt.c ./library/x509_crt.c._IGNORE

remove_mbedtls: clean
	mkdir NOTDELETE
	mv firmware.* ./NOTDELETE/
	mv fismos.* ./NOTDELETE/
	mv syscalls.c ./NOTDELETE/
	rm -f *.c *.c._IGNORE *.h
	rm -rf ./psa ./mbedtls ./DOWNLOAD_mbedtls
	mv ./NOTDELETE/* ./
	rm -rf ./NOTDELETE/

# Cleanup
clean:
	rm -f *.o *.d *.tmp *.elf
	rm -f firmware.elf firmware.hex FISMOS_firmware.hex
	rm -f testbench.vvp testbench.vcd

-include *.d
