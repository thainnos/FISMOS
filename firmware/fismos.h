#ifndef FIRMWARE_FISMOS
#define FIRMWARE_FISMOS

#include <stdint.h>
#include <stdbool.h>

// FISMOS
int32_t get_control_register(void);
int32_t get_status_register(void);
void set_status_register(int in_out32bit);

// AXI 
int32_t read_32bit_from_AXI_memory(int offset);
void write_32bit_to_AXI_memory(int in_out32bit,unsigned int offset);

#endif
