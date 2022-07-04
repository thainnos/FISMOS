open_hw_manager
connect_hw_server -allow_non_jtag
open_hw_target
set_property PROGRAM.FILE {synth_fismos.bit} [get_hw_devices xc7z020_1]
current_hw_device [get_hw_devices xc7z020_1]
set_property PROBES.FILE {} [get_hw_devices xc7z020_1]
set_property FULL_PROBES.FILE {} [get_hw_devices xc7z020_1]
set_property PROGRAM.FILE {synth_fismos.bit} [get_hw_devices xc7z020_1]
program_hw_devices [get_hw_devices xc7z020_1]
close_hw_target
disconnect_hw_server localhost:3121
