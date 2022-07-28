#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "fismos.h"
#include "fismos_memcmp.h"

#include "aes.h"

int FISMOS_AES256_decrypt_CBC(void){
  uint16_t i, j;

  uint32_t in_32[16];
  uint8_t in[64];

  uint8_t iv[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  uint8_t key[] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
                   0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                   0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
                   0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};

#ifdef FISMOS_DEBUG
  uint8_t out[] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d,
                   0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, 0xae, 0x2d, 0x8a, 0x57,
                   0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf,
                   0x8e, 0x51, 0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
                   0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef, 0xf6, 0x9f,
                   0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b,
                   0xe6, 0x6c, 0x37, 0x10};
#endif

  struct AES_ctx ctx;

  for (i = 0; i < (sizeof(in_32) / sizeof(uint32_t)); i++) {
    in_32[i] = FISMOS_read_32bit_from_AXI_memory(i * 4);
    in[i * 4 + 3] = in_32[i] & 0xff;
    in[i * 4 + 2] = in_32[i] >> 8 & 0xff;
    in[i * 4 + 1] = in_32[i] >> 16 & 0xff;
    in[i * 4 + 0] = in_32[i] >> 24 & 0xff;
  }

#ifdef FISMOS_DEBUG
  printf("\nPrinting plaintext in 8 bit\n");
  for (i = 0; i < (sizeof(in) / sizeof(uint8_t));) {
    print_hex(in[i], 2);
    printf(" ");
    i += 1;
    if (i % 16 == 0) {
      printf("\n");
    }
  }
#endif

  printf("\nStarting AES256 decrypt in CBC mode with 128bit data\n");

  AES_init_ctx_iv(&ctx, key, iv);
  AES_CBC_decrypt_buffer(&ctx, in, (sizeof(in) / sizeof(uint8_t)));

#ifdef FISMOS_DEBUG
  printf("\nPrinting cipher in 8 bit\n");
  for (i = 0; i < (sizeof(in) / sizeof(uint8_t));) {
    print_hex(in[i], 2);
    printf(" ");
    i += 1;
    if (i % 16 == 0) {
      printf("\n");
    }
  }
#endif

  // Write result to memory
  i = 0;
  for (j = (sizeof(in_32) / sizeof(uint32_t) + 2);
       j < (sizeof(in_32) / sizeof(uint32_t) + 18); j++) {
    in_32[i] = (in[i * 4] << 24) + (in[i * 4 + 1] << 16) +
               (in[i * 4 + 2] << 8) + in[i * 4 + 3];
    FISMOS_write_32bit_to_AXI_memory(in_32[i],i * 4);
    i += 1;
  }

#ifdef FISMOS_DEBUG
  if (0 == memcmp((char *)out, (char *)in, (sizeof(in) / sizeof(uint8_t)))) {
    printf("Decryption YES!\n");
  } else {
    printf("Decryption NO!\n");
  }
#endif

  return 0;
}

