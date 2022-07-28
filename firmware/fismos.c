// FISMOS
#include "fismos.h"
#include <stdint.h>

#define  FISMOS_CONTROL_REGISTER_ADDRESS 0xff000000
#define  FISMOS_STATUS_REGISTER_ADDRESS 0xff000004
#define FISMOS_SHARED_MEM_BASE_ADDRESS  0xff000008

int32_t FISMOS_read_32bit_from_AXI_memory(int offset) {
  return *(volatile int32_t *)((FISMOS_SHARED_MEM_BASE_ADDRESS + offset));
}

void FISMOS_write_32bit_to_AXI_memory(int in_out32bit, unsigned int offset) {
  *(volatile int *)((FISMOS_SHARED_MEM_BASE_ADDRESS + offset)) = in_out32bit;
}

int32_t FISMOS_get_control_register() {
  return *(volatile int32_t *)((FISMOS_CONTROL_REGISTER_ADDRESS));
}

int32_t FISMOS_get_status_register() {
  return *(volatile int32_t *)((FISMOS_STATUS_REGISTER_ADDRESS));
}

void FISMOS_set_status_register(int in_out32bit) {
  *(volatile int *)(FISMOS_STATUS_REGISTER_ADDRESS) = in_out32bit;
}
