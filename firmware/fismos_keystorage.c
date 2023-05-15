
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "fismos.h"
#include "fismos_memcmp.h"

#define FISMOS_DEBUG
// #define FISMOS_VERBOSE

int FISMOS_keystorage()
{
  int i;
  int j;
  int ret;
  uint8_t hash_luks_1[] = {
      0xf5, 0x8c, 0x4c, 0x4, 0xd6, 0xe5, 0xf1, 0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb,
      0xd6, 0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d, 0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70,
      0x2c, 0x7d, 0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf, 0xa5, 0x30, 0xe2, 0x63, 0x4,
      0x23, 0x14, 0x61, 0xb2, 0xeb, 0x5, 0xe2, 0xc3, 0x9b, 0xe9, 0xfc, 0xda, 0x6c, 0x19, 0x7,
      0x8c, 0x6a, 0x9d, 0x1b};
  uint8_t password_luks_1[] = {
      0x5a, 0x54, 0x39, 0x38, 0x51, 0x6f, 0x42, 0x4a, 0x35, 0x32, 0x6b, 0x77, 0x34, 0x39,
      0x58, 0x50, 0x49, 0x49, 0x73, 0x4d, 0x7a, 0x6, 0xd7, 0x56, 0x53, 0x75, 0x04, 0xe7,
      0x54, 0x36, 0x67, 0x45, 0x5a, 0x54, 0x39, 0x38, 0x51, 0x6f, 0x42, 0x4a, 0x35, 0x32,
      0x6b, 0x77, 0x34, 0x39, 0x58, 0x50, 0x49, 0x49, 0x73, 0x4d, 0x7a, 0x6, 0xd7, 0x56,
      0x53, 0x75, 0x04, 0xe7, 0x54, 0x36, 0x67, 0x45};

  uint32_t buffer_uint32[16];
  uint8_t input_data[64];

  for (i = 0; i < (sizeof(buffer_uint32) / sizeof(uint32_t)); i++)
  {
    buffer_uint32[i] = FISMOS_read_32bit_from_AXI_memory(i * 4);
    input_data[i * 4 + 3] = buffer_uint32[i] & 0xff;
    input_data[i * 4 + 2] = buffer_uint32[i] >> 8 & 0xff;
    input_data[i * 4 + 1] = buffer_uint32[i] >> 16 & 0xff;
    input_data[i * 4 + 0] = buffer_uint32[i] >> 24 & 0xff;
  }

#ifdef FISMOS_VERBOSE
  printf("Printing input_data in 8 bit\n");
  for (i = 0; i < (sizeof(input_data) / sizeof(uint8_t));)
  {
    printf("%x", input_data[i]);
    printf(" ");
    i += 1;
    if (i % 16 == 0)
    {
      printf("\n");
    }
  }
  printf("Printing hash_luks_1 in 8 bit\n");
  for (i = 0; i < (sizeof(hash_luks_1) / sizeof(uint8_t));)
  {
    printf("%x", hash_luks_1[i]);
    printf(" ");
    i += 1;
    if (i % 16 == 0)
    {
      printf("\n");
    }
  }
#endif

  ret = memcmp((char *)hash_luks_1, (char *)input_data, sizeof(hash_luks_1) / sizeof(uint8_t));
  printf("ret of memcmp: %d\n", ret);
  if (ret == 0) // memcmp successfull
  {
    printf("Hash for Luks_1 correct!\n");
    printf("Writing key to output data memory\n");
    // Write result to memory
    i = j = 0;
    for (j = 0; j < 16; j++)
    {
      buffer_uint32[i] = (password_luks_1[i * 4] << 24) + (password_luks_1[i * 4 + 1] << 16) +
                         (password_luks_1[i * 4 + 2] << 8) + password_luks_1[i * 4 + 3];
      FISMOS_write_32bit_to_AXI_memory(buffer_uint32[i], i * 4);
      i += 1;
    }
  }

  // Trap FISMOS if wrong hash is given
  else
  {
    printf("Hash not allowed!\nFISMOS trapped!\n");
    while (1)
    {
      __asm__ volatile("nop");
    }
  }

  return 0;
}