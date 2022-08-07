make && sudo rmmod fismos && sudo insmod fismos.ko 

echo ff000001 > /dev/fismos_instruction_register

cat /dev/fismos_status_register

cat /dev/fismos_data_buffer | hexdump
