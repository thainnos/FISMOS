#include "firmware.h"

void main(void)
{
	print_str("MAIN function called!\n\n~");
	while(1){
		// Wait for interrupts with useless inline asm
		__asm__ volatile("nop");
	}
    return;
}
