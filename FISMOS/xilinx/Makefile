#########
# Config
#########
export MEMORYSIZE = 128

#########
# Directories
#########
export BASH_ROOTDIR = $(PWD)
export GW_ROOTDIR = ../../gateware
export FW_ROOTDIR = ../../firmware
export SCRIPT_ROOTDIR = ../../scripts

#####
# Xilinx
#####
VIVADO_BASE = /opt/Xilinx/Vivado/2021.2
VIVADO = $(VIVADO_BASE)/bin/vivado
XVLOG = $(VIVADO_BASE)/bin/xvlog
XELAB = $(VIVADO_BASE)/bin/xelab
GLBL = $(VIVADO_BASE)/data/verilog/src/glbl.v
export VIVADO
# work-around for http://svn.clifford.at/handicraft/2016/vivadosig11
export RDI_VERBOSE = False

#####
# Misc
#####
VLOG = iverilog

#########
# Top level
#########
PROJECT = fismos

#########
# Board
#########
# PYNQ-Z2
DEVICE = XC7Z020clg400-1
HW_DEVICE = xc7z020_1
BOARD = pynq-z2

#########
# Source files
#########
####
# Hardware
####
GW_SOURCE_FILES = $(GW_ROOTDIR)/_config.v \
					$(GW_ROOTDIR)/picorv32_core.v \
					$(GW_ROOTDIR)/cpu_memory.v \
					$(GW_ROOTDIR)/io_memory.v \
					$(GW_ROOTDIR)/picorv32_axi_adapter.v \
					$(GW_ROOTDIR)/axil_dp_ram.v \
					$(GW_ROOTDIR)/resetn_oneshot.v \
					$(GW_ROOTDIR)/fismos_AXI_shared_memory.v \
					$(GW_ROOTDIR)/$(PROJECT).v
SIM_GW_SOURCE_FILES = 	$(GW_SOURCE_FILES) \
					$(GW_ROOTDIR)/$(PROJECT)_tb.v

#########
# Make
#########
help:
	@echo "Welcome to FISMOS"
	@echo ""
	@echo "Compile firmware:"
	@echo "  "make firmware32_hex""
	@echo ""
	@echo "Simulation with Icarus Verilog:"
	@echo "  "make icarus_sim""
	@echo ""
	@echo "Synth and Program with Vivado / xilinx"
	@echo "  "make synth"      - to synthesize the design"
	@echo "  "make synth_prog" - to synthesize and configure FPGA"
	@echo "  "make prog"       - to reload config to FPGA"
	@echo "  "make vivado"    - to start Vivado graphical user interface"
	@echo ""
	@echo "Cleanup:"
	@echo "  "make clean""

####
# Firmware
####

firmware32_hex: 
	cd $(FW_ROOTDIR) && make 

####
# Icarus
####
icarus_sim: firmware32_hex
	$(VLOG) -o $(PROJECT)_tb $(SIM_GW_SOURCE_FILES)
	  ./$(PROJECT)_tb

####
# Vivado
####
##
# TCL
##
open_gui_tcl:
	@echo start_gui >> $(PROJECT).tcl

add_synth_tcl:
	for source_file in $(GW_SOURCE_FILES); do \
	  echo read_verilog $$source_file >> $(PROJECT).tcl; \
	done
	@echo read_xdc $(BOARD)/fismos_pins.xdc >> $(PROJECT).tcl
	@echo synth_design -part $(DEVICE) -top $(PROJECT) >> $(PROJECT).tcl
	@echo opt_design >> $(PROJECT).tcl
	@echo place_design >> $(PROJECT).tcl
	@echo route_design >> $(PROJECT).tcl
	@echo >> $(PROJECT).tcl
	@echo report_utilization >> $(PROJECT).tcl
	@echo report_timing >> $(PROJECT).tcl
	@echo >> $(PROJECT).tcl
	@echo write_verilog -force synth_$(PROJECT).v >> $(PROJECT).tcl
	@echo write_bitstream -force synth_$(PROJECT).bit >> $(PROJECT).tcl

add_prog_tcl:
	@echo open_hw_manager >> $(PROJECT).tcl
	@echo connect_hw_server -allow_non_jtag >> $(PROJECT).tcl
	@echo open_hw_target >> $(PROJECT).tcl
	@echo set_property PROGRAM.FILE {synth_$(PROJECT).bit} [get_hw_devices xc7z020_1] >> $(PROJECT).tcl
	@echo current_hw_device [get_hw_devices xc7z020_1] >> $(PROJECT).tcl
	@echo refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7z020_1] 0]
	@echo set_property PROBES.FILE {} [get_hw_devices xc7z020_1] >> $(PROJECT).tcl
	@echo set_property FULL_PROBES.FILE {} [get_hw_devices xc7z020_1] >> $(PROJECT).tcl
	@echo set_property PROGRAM.FILE {synth_$(PROJECT).bit} [get_hw_devices xc7z020_1] >> $(PROJECT).tcl
	@echo program_hw_devices [get_hw_devices xc7z020_1] >> $(PROJECT).tcl
	@echo close_hw_target >> $(PROJECT).tcl
	@echo disconnect_hw_server localhost:3121 >> $(PROJECT).tcl

##
# Sim
##
sim:
	$(XVLOG) $(SIM_GW_SOURCE_FILES)
	$(XVLOG) $(GLBL)
	$(XELAB) -L unifast_ver -L unisims_ver -R $(PROJECT)_tb.v

##
# Prog
##
synth: firmware32_hex clean_tcl add_synth_tcl
	$(VIVADO) -nojournal -log $(PROJECT).log -mode batch -source $(PROJECT).tcl
	#rm -rf .Xil fsm_encoding.os synth_*.backup.log usage_statistics_webtalk.*
	#-grep -B4 -A10 'Slice LUTs' $(PROJECT).log
	#-grep -B1 -A9 ^Slack $(PROJECT).log && echo

prog: firmware32_hex clean_tcl add_prog_tcl
	$(VIVADO) -nojournal -log $(PROJECT).log -mode batch -source $(PROJECT).tcl

synth_prog: firmware32_hex clean_tcl add_synth_tcl add_prog_tcl
	$(VIVADO) -nojournal -log $(PROJECT).log -mode batch -source $(PROJECT).tcl

vivado: clean_tcl open_gui_tcl
	$(VIVADO) -nojournal -log $(PROJECT).log -source $(PROJECT).tcl

####
# Petalinux
####
add_hardwareexport_tcl: add_synth_tcl
	@echo write_hw_platform -fixed -force -include_bit -file $(BASH_ROOTDIR)/petalinux/system_wrapper.xsa >> $(PROJECT).tcl
	@echo exit 0 >> $(PROJECT).tcl
	
hardwareexport: add_hardwareexport_tcl
	$(VIVADO) -nojournal -log $(PROJECT).log -mode batch -source $(PROJECT).tcl

petalinux_build: write_tcl
	$(SCRIPT_ROOTDIR)/run_petalinux.sh
	
####
# Clean
####
.PHONY: clean clean_firmware clean_tcl
clean_tcl:
	rm -rf *.tcl

clean_firmware:
	cd $(FW_ROOTDIR) && make clean

clean: clean_tcl clean_firmware
	# Vivado
	rm -rf .Xil/ firmware.bin firmware.elf firmware.hex firmware.map synth_*.log
	rm -rf synth_*.mmi synth_*.bit synth_system.v table.txt tab_*/ webtalk.jou
	rm -rf webtalk.log webtalk_*.jou webtalk_*.log xelab.* xsim[._]* xvlog.*
	rm -rf *.v *.log *.html *.xml
	# Icarus
	rm -rf $(PROJECT)_tb

