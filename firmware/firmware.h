#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <stdint.h>
#include <stdbool.h>

// FISMOS
int32_t get_control_register(void);
int32_t get_status_register(void);
void set_status_register(int in_out32bit);

// irq.c
uint32_t *irq(uint32_t *regs, uint32_t irqs);

// print.c
void print_chr(char ch);
void print_str(const char *p);
void print_dec(unsigned int val);
void print_hex(unsigned int val, int digits);

// AXI 
int32_t read_32bit_from_AXI_memory(int offset);
void write_32bit_to_AXI_memory(int in_out32bit,unsigned int offset);

// main.c
void main(void);

#endif
