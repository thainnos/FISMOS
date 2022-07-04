#include <stdio.h>

int main()
{
	printf("Hello World after Interrupt!\n");

  int STATUS;
  // STATUS = mbedtls_md5_self_test(1);
  printf("%d\n",STATUS);
  printf("~");
	return 0;
}
