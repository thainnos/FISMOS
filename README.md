<img align="right" height="100" alt="FISMOS" title="FISMOS" src="misc/logo_FISMOS/Logo_FISMOS.conv.png"/>

# FPGA Implementation of a Security Module as Open Source

## Overview

* [Getting Started](getting_started/Readme.md): Scripts to get you set up for Development

  ---
* [FISMOS](sys/Readme.md):  `Makefiles` to call IDE with TCL scripts from `./scripts/` and start programmer
* [Scripts](scripts/Readme.md): Scripts for synthesizing, compiling and deploying

  ---
* [Gateware](hw/Readme.md): RTL in `Verilog`
* [Firmware](sw/Readme.md): `C`-Code to be compiled and run on the PicoRV32
* [Software](linux/README.md): Contains Linux Kernel Modules and User Applications for ARM-Cores

  ---
* [misc](misc/Readme.md) (optional): Logos, tutorials, write-ups, etc.

---

## Internal FISMOS memory map

| Address Range            | Description                             |
| ------------------------ | --------------------------------------- |
| 0x0000_0000 : 0x0300_0000 | Internal SRAM                           |
| 0x0400_0000 | 32bit Output from PicoRV32                           |
| 0x0500_0000 : 0xFeFF_FFFF| Reserved for Additional Security Hardware                           |
| 0xFF00_0000 | FISMOS Instruction Register (32bit)                           |
| 0xFF00_0004 | Return Register (32bit)                           |
| 0xFF00_0008 : 0xFFFF_FFFF | Shared Memory                           |

> As seen from the PicoRV32 processor. These addresses are used in the [Firmware](sw/Readme.md).

---

## AXI Interface

| Address Range            | Description                             |
| ------------------------ | --------------------------------------- |
| AXI_BASE_ADDRESS + 0x00 | FISMOS Instruction Register (32bit)                           |
| AXI_BASE_ADDRESS + 0x04 | Return Register (32bit)                           |
| AXI_BASE_ADDRESS + 0x08 | Start of Shared Memory                           |

> As seen from the external AXI4-Lite connection. These addresses are used in the Linux [Software](sw/Readme.md).  
> AXI_BASE_ADDRESS is configured via `TCL` script or chosen manually during synthesis

---

### FISMOS Instruction Register

| Bit | Description                                               |
| -----: | --------------------------------------------------------- |
|  31:25 | Don't Care                                        |
|     24 | Interrupt  from Linux to FISMOS                               |
|     23:0 | Selected Mode for FISMOS                                 |

#### Mode

| Value [15:0] | Mode |
|--|--|
| 0x0001 | AES-256-CBC-Decrypt with tinyAES|
| 0x0002 | AES-256-CBC-Encrypt with tinyAES|
| 0x0003 | **Compute** HMAC (SHA256) with mbedTLS|
| 0x0004 | **Compare** HMAC (SHA256) with mbedTLS|
| 0x0005 | Sign Data with RSA and mbedTLS|
| 0x000F | Get key, based on requested 256bit identifier|
| 0xFFFF | RESERVED |

---

### Return Register

| Bit | Description                                               |
| -----: | --------------------------------------------------------- |
|  31:25 | Don't Care                                       |
|     24 | Interrupt from FISMOS to Linux                                |
|     23:0 | State of FISMOS                                 |

#### State of FISMOS

| Value [23:0] | Status |
|--|--|
| 0x00_FFFF | Interrupt from Linux accepted |
| 0xFF_FFFF | Reset Interrupt Line to Linux |
| 0x0F_{MODE} | Working on Requested Mode |
| 0xFF_{MODE} | Requested Mode Done |

---

## Acknowledgement and Dependencies

Motivation of the presented FISMOS is to overcome the need for external components for security features.
In case, [OpenTitan](https://opentitan.org/) seems a little overpowered or too big for the considered design, the FISMOS might be ideal.
Furthermore, the proposed system might replace external secure elements, like basic functions of a TPM or [ATECC](https://www.microchip.com/en-us/product/ATECC608B).

The FISMOS is implemented on the programmable logic inside a SoC, It is utilizing surplus elements, adding a trusted environment to the ARM cores.
Communicating to Linux on mentioned cores, the FISMOS is connected via the [AXI4-Lite](https://www.xilinx.com/products/intellectual-property/axi_lite_ipif.html) bus, either directly or via a LKM.
This repository was created during the [MAPR](https://www.hs-augsburg.de/en/Electrical-Engineering/Applied-Research-in-Engineering-Sciences-MSc.html) of Philipp Schubaur at [HSA_innos](https://hsainnos.de) and was financally funded by [SiS 4.0](https://www.fim-rc.de/en/sis-4-0-secure-industry-4-0-in-swabia/).

This project was inspired by the [PicoSoc](https://github.com/YosysHQ/picorv32) from Cliffard Wolf, as well as basic structures for [AXI Components](https://github.com/alexforencich/verilog-axi) from Alex Forencich in combination with tutorials from [ZipCPU](https://zipcpu.com/).

Long-term, programming in `Rust` is considered as well as making use of dedicated security modules in programmable logic.

---

## License

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)  
This repository is planned to be compliant to [reuse](https://reuse.software/) in the near future.

---

# Paper

A first research paper can be found [here](http://gateway-bayern.de/BV047383875). A follow-up paper is planed for 2022.
To cite the FISMOS, you can use the official BibTex entry:

<pre>
@InProceedings{10.25929/7mtt-nz04,\
  author    = {Schubaur, Philipp and Merli, Dominik},\
  booktitle = {{Applied Research Conference 2021}},\
  pages = {76-80},\
  date      = {2021-07-23},\
  title     = {{Choosing a RISC-V Core for Implementing an FPGA-Based Security Module}},\
  url       = {https://doi.org/10.25929/7mtt-nz04},\
}
</pre>

[website]: https://hsainnos.de
