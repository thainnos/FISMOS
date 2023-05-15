#include "mbedtls/config.h"
#include "mbedtls/platform.h"

#include "mbedtls/rsa.h"
#include "mbedtls/md.h"

#include <stdio.h>
#include <string.h>
#include "fismos.h"

#define MBEDTLS_PKCS1_V21
// #define FISMOS_PRELOAD_MEMORY

#define KEY_FIXED

int FISMOS_rsa_sign_on_input_data(void)
{

#ifdef KEY_FIXED
  const char N_in[] = "A1D46FBA2318F8DCEF16C280948B1CF27966B9B47225ED2989F8D74B45BD36049C0AAB5AD0FF003553BA843C8E12782FC5873BB89A3DC84B883D25666CD22BF3ACD5B675969F8BEBFBCAC93FDD927C7442B178B10D1DFF9398E52316AAE0AF74E594650BDC3C670241D418684593CDA1A7B9DC4F20D2FDC6F66344074003E211";
  const char E_in[] = "010001";
  const char D_in[] = "589552BB4F2F023ADDDD5586D0C8FD857512D82080436678D07F984A29D892D31F1F7000FC5A39A0F73E27D885E47249A4148C8A5653EF69F91F8F736BA9F84841C2D99CD8C24DE8B72B5C9BE0EDBE23F93D731749FEA9CFB4A48DD2B7F35A2703E74AA2D4DB7DE9CEEA7D763AF0ADA7AC176C4E9A22C4CDA65CEC0C65964401";
  const char P_in[] = "CD083568D2D46C44C40C1FA0101AF2155E59C70B08423112AF0C1202514BBA5210765E29FF13036F56C7495894D80CF8C3BAEE2839BACBB0B86F6A2965F60DB1";
  const char Q_in[] = "CA0EEEA5E710E8E9811A6B846399420E3AE4A4C16647E426DDF8BBBCB11CD3F35CE2E4B6BCAD07AE2C0EC2ECBFCC601B207CDD77B5673E16382B1130BF465261";
  const char DP_in[] = "0D0E21C07BF434B4A83B116472C2147A11D8EB98A33CFBBCF1D275EF19D815941622435AAF3839B6C432CA53CE9E772CFBE1923A937A766FD93E96E6EDEC1DF1";
  const char DQ_in[] = "269CEBE6305DFEE4809377F078C814E37B45AE6677114DFC4F76F5097E1F3031D592567AC55B9B98213B40ECD54A4D2361F5FAACA1B1F51F71E4690893C4F081";
  const char QP_in[] = "97AC5BB885ABCA314375E9E4DB1BA4B2218C90619F61BD474F5785075ECA81750A735199A8C191FE2D3355E7CF601A70E5CABDE0E02C2538BB9FB4871540B3C1";
#endif

  int ret = 1;
  int exit_code = MBEDTLS_EXIT_FAILURE;
  size_t i, j;
  unsigned char hash[32];
  unsigned char signature_computed[MBEDTLS_MPI_MAX_SIZE];
  // unsigned char signature_computed[32];
  mbedtls_mpi E, P, Q, N, D, DP, DQ, QP;

#ifdef FISMOS_PRELOAD_MEMORY
  unsigned char data_block_incoming[32] = "ATVH9Tcu7KgRSserZtdPB7xjNAw7xZI"; // 32 * 8bit = 256 bit
  unsigned char signature_comparison[] = {109, 92, 162, 52, 89, 36, 171, 223, 193, 100, 34, 50, 86, 26, 245, 35, 254, 136, 76, 3, 179, 187, 224, 114, 29, 248, 47, 173, 182, 160, 230, 252};
#else
  // Load from memory interface
  uint32_t buffer_uint32[16];
  uint8_t data_block_incoming[32];
  for (i = 0; i < (sizeof(data_block_incoming) / sizeof(uint32_t)); i++)
  {
    buffer_uint32[i] = FISMOS_read_32bit_from_AXI_memory(i * 4);
    data_block_incoming[i * 4 + 3] = buffer_uint32[i] & 0xff;
    data_block_incoming[i * 4 + 2] = buffer_uint32[i] >> 8 & 0xff;
    data_block_incoming[i * 4 + 1] = buffer_uint32[i] >> 16 & 0xff;
    data_block_incoming[i * 4 + 0] = buffer_uint32[i] >> 24 & 0xff;
  }
#endif

  mbedtls_rsa_context rsa;
  mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);
  printf("Welcome to FISMOS RSA sign\n");

  printf("MPI init:");
  mbedtls_mpi_init(&N);
  mbedtls_mpi_init(&P);
  mbedtls_mpi_init(&Q);
  mbedtls_mpi_init(&D);
  mbedtls_mpi_init(&E);
  mbedtls_mpi_init(&DP);
  mbedtls_mpi_init(&DQ);
  mbedtls_mpi_init(&QP);
  printf(" success\n");

  printf("MPI read string:");
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&N, 16, N_in));
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&P, 16, P_in));
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&Q, 16, Q_in));
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&D, 16, D_in));
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&E, 16, E_in));
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&DP, 16, DP_in));
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&DQ, 16, DQ_in));
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&QP, 16, QP_in));
  printf(" success\n");

  printf("RSA import: ");
  if ((ret = mbedtls_rsa_import(&rsa, &N, &P, &Q, &D, &E)) != 0)
  {
    printf(" failed\n  ! mbedtls_rsa_import returned %d\n\n", ret);
    goto cleanup;
  }
  else
  {
    printf(" success\n");
  }

  printf("RSA completion: ");
  ret = mbedtls_rsa_complete(&rsa);
  printf(" ret: %x:", ret);
  if (ret != 0)
  {
    printf(" failed\n  ! mbedtls_rsa_complete returned %d\n\n", ret);
    goto cleanup;
  }
  else
  {
    printf(" success\n");
  }

  printf("Generate HASH:");
  ret = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                   data_block_incoming, sizeof(data_block_incoming), hash);
  printf(" ret: %x:", ret);
  if (ret != 0)
  {
    MBEDTLS_MPI_CHK(printf(" failed\n  ! Could not open or read data_block_incoming\n\n"));
    goto cleanup;
  }
  else
  {
    printf(" success\n");
  }

  printf("Checking the private key:");
  ret = mbedtls_rsa_check_privkey(&rsa);
  printf(" ret: %x:", ret);
  if (ret != 0)
  {
    printf(" failed\n  ! mbedtls_rsa_check_privkey failed with -0x%0x\n", (unsigned int)-ret);
    goto cleanup;
  }
  else
  {
    printf(" success\n");
  }

  printf("Generate Signature to signature_computed:");
  ret = mbedtls_rsa_pkcs1_sign(&rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 32, hash, signature_computed);
  printf(" ret: %x:", ret);
  if (ret != 0)
  {
    printf(" failed\n  ! mbedtls_rsa_pkcs1_sign returned -0x%0x\n\n", (unsigned int)-ret);
    goto cleanup;
  }
  else
  {
    printf(" success\n");
  }

  printf("Printout signature_computed:\n");
  for (i = 0; i < 32; i++)
  {
    printf("%X", signature_computed[i]);
  }

#ifdef FISMOS_PRELOAD_MEMORY
  printf("\nPrintout signature_comparison:\n");
  for (i = 0; i < 32; i++)
  {
    printf("%X", signature_comparison[i]);
  }

  printf("\nComparing signatures:");
  if ((ret = memcmp(signature_computed, signature_comparison, sizeof(signature_comparison))) != 0)
  // if( ( ret = compareArrays(hmac_sha256_computed, hmac_sha256_incoming, (int)sizeof(hmac_sha256_incoming)) ) != 0 )
  {
    printf(" failed! memcmp returned %d\n\n", ret);
    return -1;
  }
  else
  {
    printf(" success!\n");
    return 0;
  }
  printf("\n\nTEST SUCCESSFULL\n~\n");
  exit_code = MBEDTLS_EXIT_SUCCESS;
#endif

  // Write result to memory
  i = 0;
  j = 0;
  for (j = 0; j < 8; j++)
  {
    buffer_uint32[i] = (signature_computed[i * 4] << 24) + (signature_computed[i * 4 + 1] << 16) +
                       (signature_computed[i * 4 + 2] << 8) + signature_computed[i * 4 + 3];
    FISMOS_write_32bit_to_AXI_memory(buffer_uint32[i], i * 4);
    i += 1;
  }
  printf("\n\nTEST SUCCESSFULL\n~");
  return 0;

cleanup:
  mbedtls_rsa_free(&rsa);
  mbedtls_mpi_free(&N);
  mbedtls_mpi_free(&P);
  mbedtls_mpi_free(&Q);
  mbedtls_mpi_free(&D);
  mbedtls_mpi_free(&E);
  mbedtls_mpi_free(&DP);
  mbedtls_mpi_free(&DQ);
  mbedtls_mpi_free(&QP);

  if (exit_code != MBEDTLS_EXIT_SUCCESS)
  {
    printf("\nAn error occurred.\n");
  }

  mbedtls_exit(exit_code);
}
