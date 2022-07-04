RISCV_GNU_TOOLCHAIN_INSTALL_PREFIX = /opt/riscv32
PYTHON = python3
IVERILOG = iverilog

FIRMWARE_OBJS = start.o irq.o print.o fismos.o aes.o main.o
GCC_WARNS  = -Werror -Wall -Wextra -Wshadow -Wundef -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings
GCC_WARNS += -Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes -pedantic # -Wconversion
TOOLCHAIN_PREFIX = $(RISCV_GNU_TOOLCHAIN_INSTALL_PREFIX)i/bin/riscv32-unknown-elf-
COMPRESSED_ISA = C

FISMOS_firmware.hex: firmware.bin ../scripts/makehex.py
	$(PYTHON) ../scripts/makehex.py $< 32768 > $@
	cp $@ firmware.mem

firmware.bin: firmware.elf
	$(TOOLCHAIN_PREFIX)objcopy -O binary $< $@
	chmod -x $@

firmware.elf: $(FIRMWARE_OBJS) sections.lds
	$(TOOLCHAIN_PREFIX)gcc -Os -ffreestanding -nostdlib -o $@ -Wl,-Bstatic,-T,sections.lds,-Map,firmware.map,--strip-debug \
		$(FIRMWARE_OBJS) -lgcc
	chmod -x $@

start.o: start.S
	$(TOOLCHAIN_PREFIX)gcc -c -march=rv32im$(subst C,c,$(COMPRESSED_ISA)) -o $@ $<

%.o: %.c
	$(TOOLCHAIN_PREFIX)gcc -c -march=rv32i$(subst C,c,$(COMPRESSED_ISA)) -Os --std=c99 $(GCC_WARNS) -ffreestanding -nostdlib -o $@ $<

clean:
	rm -vrf $(FIRMWARE_OBJS) \
		firmware.elf firmware.bin firmware.hex firmware.map firmware.mem FISMOS_firmware.hex
