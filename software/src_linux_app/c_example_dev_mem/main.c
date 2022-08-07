#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
  
#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)
 
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

int read_memory(off_t target){
	int fd;
    void *map_base, *virt_addr; 
	unsigned long read_result, writeval;


    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
    fflush(stdout);
    
    /* Map one page */
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
    if(map_base == (void *) -1) FATAL;
    fflush(stdout);
    
    virt_addr = map_base + (target & MAP_MASK);
	read_result = *((unsigned long *) virt_addr);
    printf("Value at address 0x%X (%p): 0x%X\n", target, virt_addr, read_result); 
    fflush(stdout);

	if(munmap(map_base, MAP_SIZE) == -1) FATAL;
    close(fd);
    return 0;

}

int write_memory (off_t target, unsigned long writeval){
	int fd;
    void *map_base, *virt_addr; 
	unsigned long read_result;

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
    fflush(stdout);
    
    /* Map one page */
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
    if(map_base == (void *) -1) FATAL;
    fflush(stdout);
    
    virt_addr = map_base + (target & MAP_MASK);
	read_result = *((unsigned long *) virt_addr);
    printf("Value at address 0x%X (%p): 0x%X\n", target, virt_addr, read_result); 
    fflush(stdout);

	*((unsigned long *) virt_addr) = writeval;
	read_result = *((unsigned long *) virt_addr);
		printf("Written 0x%X; readback 0x%X\n", writeval, read_result); 
	fflush(stdout);
	
	if(munmap(map_base, MAP_SIZE) == -1) FATAL;
    close(fd);
    return 0;
}

int main(int argc, char **argv) {
	u_int32_t CONTROL_REGISTER=0x43c00000;
	u_int32_t STATUS_REGISTER=0x43c00004;
	u_int32_t SHARED_MEMORY_ADRESS=0x43c00008;

	read_memory(CONTROL_REGISTER);
	write_memory(CONTROL_REGISTER,0xf0000001);
	write_memory(CONTROL_REGISTER,0xff000001);

	for(int loop=0; loop<16; loop++)
	{
		read_memory(SHARED_MEMORY_ADRESS);
		SHARED_MEMORY_ADRESS = SHARED_MEMORY_ADRESS+4;
	}

    return 0;
}

