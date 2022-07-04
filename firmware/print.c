#include "firmware.h"
#define OUT_32BIT_ADDRESS 0x04000000

void print_chr(char ch) { *((volatile uint32_t *)OUT_32BIT_ADDRESS) = ch; }

void print_str(const char *p) {
  while (*p != 0)
    *((volatile uint32_t *)OUT_32BIT_ADDRESS) = *(p++);
}

void print_dec(unsigned int val) {
  char buffer[10];
  char *p = buffer;
  while (val || p == buffer) {
    *(p++) = val % 10;
    val = val / 10;
  }
  while (p != buffer) {
    *((volatile uint32_t *)OUT_32BIT_ADDRESS) = '0' + *(--p);
  }
}

void print_hex(unsigned int val, int digits) {
  for (int i = (4 * digits) - 4; i >= 0; i -= 4)
    *((volatile uint32_t *)OUT_32BIT_ADDRESS) = "0123456789ABCDEF"[(val >> i) % 16];
}
