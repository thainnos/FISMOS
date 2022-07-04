#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"

int main()
{
	int write_data=0,read_data=0;
	u32 CONTROL_REGISTER=0x43c00000;
	u32 STATUS_REGISTER=0x43c00004;
	u32 SHARED_MEMORY_ADRESS=0x43c00008;

    init_platform();

    print("\r\nFISMOS AES-256-CBC test\n\r");

    xil_printf("\r\nWrite Data to Input Buffer \r\n");


    xil_printf("\r\nActivate FISMOS with 0xFF000001 to Control Register \r\n");
    write_data=0xFF000001;
    Xil_Out32(CONTROL_REGISTER,write_data);
    // Check for correct setting of Control Register
    read_data=Xil_In32(CONTROL_REGISTER);
    xil_printf("0x%08x read from Control Register at 0x%p\r\n",read_data,CONTROL_REGISTER);

    // Wait for FISMOS to be ready
    do {
        read_data=Xil_In32(STATUS_REGISTER);
    }while(read_data != 0xffff0001);

    xil_printf("\r\n\r\nRead result from Output Buffer\r\n");
	for(int loop=0; loop<16; loop++)
	{
		read_data=Xil_In32(SHARED_MEMORY_ADRESS);
		xil_printf("0x%p  :  0x%08x\r\n",SHARED_MEMORY_ADRESS,read_data);
		SHARED_MEMORY_ADRESS = SHARED_MEMORY_ADRESS+4;
	}

    xil_printf("\r\nReset FISMOS with 0x00000000 to Control Register \r\n");
    write_data=0x00000000;
    Xil_Out32(CONTROL_REGISTER,write_data);
    // Check for correct setting of Control Register
    read_data=Xil_In32(CONTROL_REGISTER);
    xil_printf("0x%08x read from Control Register at 0x%p\r\n",read_data,CONTROL_REGISTER);

    xil_printf("\r\nActivate FISMOS with 0xFF000002 to Control Register \r\n");
    write_data=0xFF000002;
    Xil_Out32(CONTROL_REGISTER,write_data);
    // Check for correct setting of Control Register
    read_data=Xil_In32(CONTROL_REGISTER);
    xil_printf("0x%08x read from Control Register at 0x%p\r\n",read_data,CONTROL_REGISTER);

	// Finish test
    cleanup_platform();
    xil_printf("\r\n\r\nFISMOS test DONE!\r\n");
    return 0;
}
