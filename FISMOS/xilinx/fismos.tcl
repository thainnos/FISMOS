read_verilog ../../gateware/_config.v
read_verilog ../../gateware/picorv32_core.v
read_verilog ../../gateware/cpu_memory.v
read_verilog ../../gateware/io_memory.v
read_verilog ../../gateware/picorv32_axi_adapter.v
read_verilog ../../gateware/axil_dp_ram.v
read_verilog ../../gateware/resetn_oneshot.v
read_verilog ../../gateware/fismos_AXI_shared_memory.v
read_verilog ../../gateware/fismos.v
read_xdc pynq-z2/fismos_pins.xdc
synth_design -part XC7Z020clg400-1 -top fismos
opt_design
place_design
route_design

report_utilization
report_timing

write_verilog -force synth_fismos.v
write_bitstream -force synth_fismos.bit