read_verilog ../../hw/_config.v
read_verilog ../../hw/picorv32_core.v
read_verilog ../../hw/cpu_memory.v
read_verilog ../../hw/io_memory.v
read_verilog ../../hw/picorv32_axi_adapter.v
read_verilog ../../hw/axil_dp_ram.v
read_verilog ../../hw/resetn_oneshot.v
read_verilog ../../hw/fismos_AXI_shared_memory.v
read_verilog ../../hw/fismos.v
read_xdc pynq-z2/fismos_pins.xdc
synth_design -part XC7Z020clg400-1 -top fismos
opt_design
place_design
route_design

report_utilization
report_timing

write_verilog -force synth_fismos.v
write_bitstream -force synth_fismos.bit
write_hw_platform -fixed -force -include_bit -file /home/phsc/Documents/fismos/sys/xilinx/petalinux/system_wrapper.xsa
exit 0
