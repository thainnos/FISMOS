#include <stdio.h>
#include "fismos.h"

#define FISMOS_DEBUG
#define FISMOS_AES256_decrypt_CBC_AVAILABLE
#define FISMOS_run_hmac_on_input_data_AVAILABLE
#define FISMOS_keystorage_AVAILABLE
#define FISMOS_run_hmac_compare_on_input_data_AVAILABLE
#define FISMOS_rsa_sign_on_input_data_AVAILABLE

int main()
{

  // /////////////////////////////////////////////////////////////////////////////
  // Variables
  // /////////////////////////////////////////////////////////////////////////////
  uint32_t control_register_value;
  int status;
#ifdef FISMOS_AES256_decrypt_CBC_AVAILABLE
  bool FISMOS_AES256_decrypt_CBC_RUN;
  FISMOS_AES256_decrypt_CBC_RUN = false;
#endif
#ifdef FISMOS_run_hmac_on_input_data_AVAILABLE
  bool FISMOS_run_hmac_on_input_data_RUN;
  FISMOS_run_hmac_on_input_data_RUN = false;
#endif
#ifdef FISMOS_keystorage_AVAILABLE
  bool FISMOS_keystorage_RUN;
  FISMOS_keystorage_RUN = false;
#endif
#ifdef FISMOS_run_hmac_compare_on_input_data_AVAILABLE
  bool FISMOS_run_hmac_compare_on_input_data_RUN;
  FISMOS_run_hmac_compare_on_input_data_RUN = false;
#endif
#ifdef FISMOS_rsa_sign_on_input_data_AVAILABLE
  bool FISMOS_rsa_sign_on_input_data_RUN;
  FISMOS_rsa_sign_on_input_data_RUN = false;
#endif

  // /////////////////////////////////////////////////////////////////////////////
  // Init
  // /////////////////////////////////////////////////////////////////////////////
  printf("Welcome to the FISMOS!\n");

  // /////////////////////////////////////////////////////////////////////////////
  // Loop
  // /////////////////////////////////////////////////////////////////////////////
  while (true)
  {
    // Get control value
    control_register_value = FISMOS_get_control_register();
    printf("control_register_value: %lx\n", control_register_value);

    if ((control_register_value << 16) == 0x00000000 << 16)
    {
#ifdef FISMOS_DEBUG
      printf("No value to control register set!\n");
#endif
#ifdef FISMOS_AES256_decrypt_CBC_AVAILABLE
      FISMOS_AES256_decrypt_CBC_RUN = false;
#endif
#ifdef FISMOS_run_hmac_on_input_data_AVAILABLE
      FISMOS_run_hmac_on_input_data_RUN = false;
#endif
#ifdef FISMOS_keystorage_AVAILABLE
      FISMOS_keystorage_RUN = false;
#endif
#ifdef FISMOS_run_hmac_compare_on_input_data_AVAILABLE
      FISMOS_run_hmac_compare_on_input_data_RUN = false;
#endif
#ifdef FISMOS_rsa_sign_on_input_data_AVAILABLE
      FISMOS_rsa_sign_on_input_data_RUN = false;
#endif
    }
    // /////////////////////////////////////////////////////////////////////////////
    // AES-256-CBC-Decrypt with tinyAES
    // /////////////////////////////////////////////////////////////////////////////
#ifdef FISMOS_AES256_decrypt_CBC_AVAILABLE
    else if (((control_register_value << 16) == 0x00000001 << 16) && !FISMOS_AES256_decrypt_CBC_RUN)
    {
      FISMOS_set_status_register(0x0000ffff); // Accept Interrupt to Linux
      FISMOS_set_status_register(0x00ffffff); // Reset Interrupt Line to Linux
      FISMOS_set_status_register(0x000F0001); // Start work on AES-256-CBC-Decrypt with tinyAES
      status = FISMOS_AES256_decrypt_CBC();
      if (status)
      {
        printf("AES256_decrypt_CBC failed!\n");
        while (1)
        {
          __asm__ volatile("nop");
        }
      }
      else
      {
        printf("AES256_decrypt_CBC SUCCESS!\n");
        FISMOS_set_status_register(0x00FF0001); // Finish work on AES-256-CBC-Decrypt with tinyAES
      }
      FISMOS_AES256_decrypt_CBC_RUN = true;
    }
#endif

    // /////////////////////////////////////////////////////////////////////////////
    // Check HMAC (SHA256) with mbedTLS
    // /////////////////////////////////////////////////////////////////////////////
#ifdef FISMOS_run_hmac_on_input_data_AVAILABLE
    else if (((control_register_value << 16) == 0x00000003 << 16) && !FISMOS_run_hmac_on_input_data_RUN)
    {
      FISMOS_set_status_register(0x0000ffff); // Accept Interrupt to Linux
      FISMOS_set_status_register(0x00ffffff); // Reset Interrupt Line to Linux
      FISMOS_set_status_register(0x000F0003); // Start work on Check HMAC (SHA256) with mbedTLS
      status = FISMOS_run_hmac_on_input_data();
      if (status)
      {
        printf("FISMOS_run_hmac_on_input_data failed!\nFISMOS trapped!\n");
        while (1)
        {
          __asm__ volatile("nop");
        }
      }
      else
      {
        printf("FISMOS_run_hmac_on_input_data SUCCESS!\n");
        FISMOS_set_status_register(0x00FF0003); // Finish work on Check HMAC (SHA256) with mbedTLS
        printf("~");
      }
      FISMOS_run_hmac_on_input_data_RUN = true;
    }
#endif

    // /////////////////////////////////////////////////////////////////////////////
    // Compare HMAC (SHA256) with mbedTLS
    // /////////////////////////////////////////////////////////////////////////////
#ifdef FISMOS_run_hmac_compare_on_input_data_AVAILABLE
    else if (((control_register_value << 16) == 0x00000004 << 16) && !FISMOS_run_hmac_compare_on_input_data_RUN)
    {
      FISMOS_set_status_register(0x0000ffff); // Accept Interrupt to Linux
      FISMOS_set_status_register(0x00ffffff); // Reset Interrupt Line to Linux
      FISMOS_set_status_register(0x000F0003); // Start work on Check HMAC (SHA256) with mbedTLS
      status = FISMOS_run_hmac_compare_on_input_data();
      if (status)
      {
        printf("FISMOS_run_hmac_compare_on_input_data failed!\nFISMOS trapped!\n");
        while (1)
        {
          __asm__ volatile("nop");
        }
      }
      else
      {
        printf("FISMOS_run_hmac_compare_on_input_data SUCCESS!\n");
        FISMOS_set_status_register(0x00FF0003); // Finish work on Check HMAC (SHA256) with mbedTLS
        printf("~");
      }
      FISMOS_run_hmac_compare_on_input_data_RUN = true;
    }
#endif

    // /////////////////////////////////////////////////////////////////////////////
    // FISMOS keystorage based on Hash
    // /////////////////////////////////////////////////////////////////////////////
#ifdef FISMOS_keystorage_AVAILABLE
    else if (((control_register_value << 16) == 0x0000000F << 16) && !FISMOS_keystorage_RUN)
    {
      FISMOS_set_status_register(0x0000ffff); // Accept Interrupt to Linux
      FISMOS_set_status_register(0x00ffffff); // Reset Interrupt Line to Linux
      FISMOS_set_status_register(0x000F000F); // Start work on Check HMAC (SHA256) with mbedTLS
      status = FISMOS_keystorage();
      if (status)
      {
        printf("FISMOS_keystorage failed!\nFISMOS trapped!\n");
        while (1)
        {
          __asm__ volatile("nop");
        }
      }
      else
      {
        printf("FISMOS_keystorage SUCCESS!\n");
        FISMOS_set_status_register(0x00FF000F); // Finish work on Check HMAC (SHA256) with mbedTLS
        printf("~");
      }
      FISMOS_keystorage_RUN = true;
    }
#endif

    // /////////////////////////////////////////////////////////////////////////////
    // FISMOS keystorage based on Hash
    // /////////////////////////////////////////////////////////////////////////////
#ifdef FISMOS_rsa_sign_on_input_data_AVAILABLE
    else if (((control_register_value << 16) == 0x00000005 << 16) && !FISMOS_rsa_sign_on_input_data_RUN)
    {
      FISMOS_set_status_register(0x0000ffff); // Accept Interrupt to Linux
      FISMOS_set_status_register(0x00ffffff); // Reset Interrupt Line to Linux
      FISMOS_set_status_register(0x000F0005); // Start work on Check HMAC (SHA256) with mbedTLS
      status = FISMOS_rsa_sign_on_input_data();
      if (status)
      {
        printf("FISMOS_rsa_sign_on_input_data failed!\nFISMOS trapped!\n");
        while (1)
        {
          __asm__ volatile("nop");
        }
      }
      else
      {
        printf("FISMOS_rsa_sign_on_input_data SUCCESS!\n");
        FISMOS_set_status_register(0x00FF0005); // Finish work on Check HMAC (SHA256) with mbedTLS
        printf("~");
      }
      FISMOS_rsa_sign_on_input_data_RUN = true;
    }
#endif

    // /////////////////////////////////////////////////////////////////////////////
    // Default case
    // /////////////////////////////////////////////////////////////////////////////
    //     else {
    //         printf("Command not defined!\nFISMOS trapped!\n");
    //         while (1) {
    //           __asm__ volatile("nop");
    //         }
    //         return -1;
    //     }
  }

  // Ending main()
  return 0;
}
