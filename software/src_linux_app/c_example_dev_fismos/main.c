#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>

int main() 
{     
int fd_instruction_register, bytes_read; 
char *c = (char *) calloc(100, sizeof(char)); 
  
fd_instruction_register = open("/dev/fismos_instruction_register", O_RDONLY); 
if (fd_instruction_register < 0) { perror("r1"); exit(1); } 
  
bytes_read = read(fd_instruction_register, c, 10); 
printf("%d bytes were read from /dev/fismos_instruction_register\n", bytes_read); 
c[bytes_read] = '\0'; 
printf("Those bytes are as follows: %s", c); 
} 
