# FISMOS Firmware

This folder contains the source code, Makefiles and all required information for building the firmware.
The respective firmware runs via the RISC-V Softcore.

## mbedTLS

The current implementation of the FISMOS makes use of the `mbedTLS` crypto library.
Hence, the Makefile loads the respective repository from the GitHub link [https://github.com/Mbed-TLS/mbedtls](https://github.com/Mbed-TLS/mbedtls)

### Adjustments of the `mbedtls_config.h`

As the FISMOS only implements the necessary basic to be able to utilizie the respective library, some adjustments need to be made.
This process is automatized via `sed` commands in the Makefile.

The resptive configuration is altered:

- Disable x509_cert.c file for compatibility reasons [link](https://tls.mbed.org/discussions/platform-specific/is-dirent-h-absolutely-necessary)
- **UN**define `MBEDTLS_NET_C`
- **UN**define `MBEDTLS_TIMING_C`
- Define `MBEDTLS_NO_PLATFORM_ENTROPY`
