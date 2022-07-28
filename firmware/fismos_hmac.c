#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/rsa.h"
#include "mbedtls/md.h"
#include "fismos_memcmp.h"

#include "mbedtls/aes.h"

#include <stdio.h>
#include <string.h>

#include "fismos.h"

#define FISMOS_DEBUG
// #define FISMOS_PRELOAD_MEMORY
#define FISMOS_VERBOSE

// FISMOS_run_hmac_compare_on_input_data
int FISMOS_run_hmac_compare_on_input_data(void)
{
    int i = -1;
    int ret = -1;


#ifdef FISMOS_PRELOAD_MEMORY
    unsigned char data_block_incoming[32] = {0x57,0x4f,0x48,0x56,0x4d,0x4d,0x58,0x52,0x58,0x44,0x4d,0x4d,0x56,0x44,0x4d,0x52,0x57,0x58,0x4e,0x4f,0x43,0x59,0x4d,0x56,0x42,0x45,0x47,0x42,0x59,0x43,0x49,0x4c}; // 32 * 8bit = 256 bit
    unsigned char hmac_sha256_incoming[32] = {0xED,0xE0,0xA4,0xD5,0xEC,0x75,0x7E,0xAE,0xE,0x4A,0xBB,0xCA,0x5,0x12,0x90,0x2F,0xA8,0x84,0x98,0xF4,0x86,0x6C,0xF4,0x94,0xD1,0xB0,0xF2,0xA9,0x46,0x5A,0x64,0xFD}; // 32 * 8bit = 256 bit
#else
    // Load from memory interface
    uint32_t input_buffer_uint32[16];
    uint8_t data_block_incoming[32];
    uint8_t hmac_sha256_incoming[32];
    for (i = 0; i < (sizeof(data_block_incoming) / sizeof(uint32_t)); i++) {
    input_buffer_uint32[i] = FISMOS_read_32bit_from_AXI_memory(i * 4);
    data_block_incoming[i * 4 + 3] = input_buffer_uint32[i] & 0xff;
    data_block_incoming[i * 4 + 2] = input_buffer_uint32[i] >> 8 & 0xff;
    data_block_incoming[i * 4 + 1] = input_buffer_uint32[i] >> 16 & 0xff;
    data_block_incoming[i * 4 + 0] = input_buffer_uint32[i] >> 24 & 0xff;
    }
    for (i = 0; i < (sizeof(hmac_sha256_incoming) / sizeof(uint32_t)); i++) {
    input_buffer_uint32[i] = FISMOS_read_32bit_from_AXI_memory(i * 4 + 8*4);
    hmac_sha256_incoming[i * 4 + 3] = input_buffer_uint32[i] & 0xff;
    hmac_sha256_incoming[i * 4 + 2] = input_buffer_uint32[i] >> 8 & 0xff;
    hmac_sha256_incoming[i * 4 + 1] = input_buffer_uint32[i] >> 16 & 0xff;
    hmac_sha256_incoming[i * 4 + 0] = input_buffer_uint32[i] >> 24 & 0xff;
    }
#endif

    unsigned char signing_key[32] = "Bt6GGG8YGJ2MqJPOF6gi4joW6qIGCHAC";

    unsigned char hmac_sha256_computed[33] = {}; // 256/8 = 32
    const mbedtls_md_info_t *md_info_sha256;
    md_info_sha256 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

#ifdef FISMOS_VERBOSE
    printf("data_block_incoming (char):\n");
    for(i=0;i<sizeof(data_block_incoming);i=i+1)
    {
        printf("%X",data_block_incoming[i]);
    }
    printf("\nhmac_sha256_incoming (hex):\n");
    for(i=0;i<sizeof(hmac_sha256_incoming);i++)
    {
        printf( "%X", hmac_sha256_incoming[i]);
    }
#endif

    printf( "\nSignging data_block_incoming with HMAC:" );
    if( ( ret = mbedtls_md_hmac(md_info_sha256, signing_key, sizeof(signing_key), data_block_incoming, sizeof(data_block_incoming), hmac_sha256_computed) ) != 0 )
    {
        printf( " failed! mbedtls_md_hmac returned %d\n", ret );
        goto error;
    } else {
        printf( " success!\n");
    }

#ifdef FISMOS_VERBOSE
    printf("hmac_sha256_computed (hex):\n");
    for(i=0;i<sizeof(hmac_sha256_computed);i++)
    {
        printf( "%X", hmac_sha256_computed[i]);
    }
    printf("\nsizeof(hmac_sha256_computed): %dbyte\n", (int)sizeof(hmac_sha256_computed));

    printf( "Comparing HMAC: " );
    if( ( ret = memcmp(hmac_sha256_computed, hmac_sha256_incoming, sizeof(hmac_sha256_incoming)) ) != 0 )
    {
        printf( "signatures mismatch! memcmp returned %d\n",ret );
        FISMOS_write_32bit_to_AXI_memory(0xffffffff,0);
        printf("Hash not correct!\nFISMOS trapped!\n");
        while (1)
        {
            __asm__ volatile("nop");
        }
    }
    else {
        printf( "signatures correct\n" );
        FISMOS_write_32bit_to_AXI_memory(0x11111111,0);
        return 0;
    }
#endif

    return 0;

error:
    printf( "GOTO Error called!\n" );
    return -1;
};


int FISMOS_run_hmac_on_input_data(void)
{
    int i = -1;
    int j = -1;
    int ret = -1;

#ifdef FISMOS_PRELOAD_MEMORY
    unsigned char data_block_incoming[32] = {0x57,0x4f,0x48,0x56,0x4d,0x4d,0x58,0x52,0x58,0x44,0x4d,0x4d,0x56,0x44,0x4d,0x52,0x57,0x58,0x4e,0x4f,0x43,0x59,0x4d,0x56,0x42,0x45,0x47,0x42,0x59,0x43,0x49,0x4c}; // 32 * 8bit = 256 bit
#else
    // Load from memory interface
    uint32_t input_buffer_uint32[16];
    uint8_t data_block_incoming[32];
    for (i = 0; i < (sizeof(data_block_incoming) / sizeof(uint32_t)); i++) {
    input_buffer_uint32[i] = FISMOS_read_32bit_from_AXI_memory(i * 4);
    data_block_incoming[i * 4 + 3] = input_buffer_uint32[i] & 0xff;
    data_block_incoming[i * 4 + 2] = input_buffer_uint32[i] >> 8 & 0xff;
    data_block_incoming[i * 4 + 1] = input_buffer_uint32[i] >> 16 & 0xff;
    data_block_incoming[i * 4 + 0] = input_buffer_uint32[i] >> 24 & 0xff;
  }
#endif

    unsigned char signing_key[32] = "Bt6GGG8YGJ2MqJPOF6gi4joW6qIGCHAC";

    unsigned char hmac_sha256_computed[32] = {}; // 256/8 = 32
    const mbedtls_md_info_t *md_info_sha256;
    md_info_sha256 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

#ifdef FISMOS_VERBOSE
    printf("data_block_incoming (char):\n");
    for(i=0;i<sizeof(data_block_incoming);i=i+1)
    {
        printf("%X",data_block_incoming[i]);
    }
#endif

    printf( "\nSignging data_block_incoming with HMAC" );
    if( ( ret = mbedtls_md_hmac(md_info_sha256, signing_key, sizeof(signing_key), data_block_incoming, sizeof(data_block_incoming), hmac_sha256_computed) ) != 0 )
    {
        printf( " failed! mbedtls_md_hmac returned %d\n", ret );
        goto error;
    } else {
        printf( " success!\n");
    }

#ifdef FISMOS_VERBOSE
    printf("hmac_sha256_computed (hex):\n");
    for(i=0;i<sizeof(hmac_sha256_computed);i++)
    {
        printf( "%X", hmac_sha256_computed[i]);
    }
    printf("\nsizeof(hmac_sha256_computed): %dbyte\n", (int)sizeof(hmac_sha256_computed));
#endif

    // Write result to memory
    i = 0;
    j = 0;
    for (j = 0;j < 8; j++)
    {
        input_buffer_uint32[i] = (hmac_sha256_computed[i * 4] << 24) + (hmac_sha256_computed[i * 4 + 1] << 16) +
                                 (hmac_sha256_computed[i * 4 + 2] << 8) + hmac_sha256_computed[i * 4 + 3];
        FISMOS_write_32bit_to_AXI_memory(input_buffer_uint32[i], i * 4);
        i += 1;
    }

    return 0;

error:
    printf( "GOTO Error called!\n" );
    return -1;
}
