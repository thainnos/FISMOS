update_module_reference design_1_fismos_0_1
update_compile_order -fileset sources_1
reset_run synth_1
launch_runs impl_1 -to_step write_bitstream -jobs 8

write_hw_platform -fixed -include_bit -force -file /home/phsc/vivado_working/FISMOSv0/design_1_wrapper.xsa