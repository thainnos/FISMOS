// FISMOS
#include "firmware.h"
#include <stdint.h>

#define  CONTROL_REGISTER_ADDRESS 0xff000000
#define  STATUS_REGISTER_ADDRESS 0xff000004
#define SHARED_MEM_BASE_ADDRESS  0xff000008

int32_t read_32bit_from_AXI_memory(int offset) {
  return *(volatile int32_t *)((SHARED_MEM_BASE_ADDRESS + offset));
}

void write_32bit_to_AXI_memory(int in_out32bit, unsigned int offset) {
  *(volatile int *)((SHARED_MEM_BASE_ADDRESS + offset)) = in_out32bit;
}

int32_t get_control_register() {
  return *(volatile int32_t *)((CONTROL_REGISTER_ADDRESS));
}

int32_t get_status_register() {
  return *(volatile int32_t *)((STATUS_REGISTER_ADDRESS));
}

void set_status_register(int in_out32bit) {
  *(volatile int *)(STATUS_REGISTER_ADDRESS) = in_out32bit;
}
