#ifndef FIRMWARE_FISMOS
#define FIRMWARE_FISMOS

#include <stdint.h>
#include <stdbool.h>

// FISMOS
int32_t FISMOS_get_control_register(void);
int32_t FISMOS_get_status_register(void);
void FISMOS_set_status_register(int in_out32bit);

// Modes
int FISMOS_run_hmac_on_input_data(void);
int FISMOS_run_hmac_compare_on_input_data(void);
int FISMOS_rsa_sign_on_input_data(void);
int FISMOS_AES256_decrypt_CBC(void);
int FISMOS_keystorage(void);

// AXI
int32_t FISMOS_read_32bit_from_AXI_memory(int offset);
void FISMOS_write_32bit_to_AXI_memory(int in_out32bit, unsigned int offset);

#endif
