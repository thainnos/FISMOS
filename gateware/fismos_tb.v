`timescale 1 ns / 1 ps

`define IRQ_TEST

module fismos_tb;
	reg clk = 1;
	reg resetn = 0;

	integer i;
	integer top_address = 16;
	wire trap;
	wire [31:0] out32bit;
	wire out32bit_en;
	wire interrupt_to_linux;

	fismos DUT (
		.clk        (clk        ),
		.resetn     (1'b1     ),
		.trap       (trap       ),
		.out32bit (out32bit),
		.out32bit_en(out32bit_en),
		.interrupt_to_linux(interrupt_to_linux)
	);

	always #5 clk = ~clk;

	// Drop AXI memory content
	initial begin
	`ifdef FISMOS_DEBUG_PRELOAD_INPUTMEMORY
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[0][31:0] = 32'hf58c_4c04;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[1][31:0] = 32'hd6e5_f1ba;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[2][31:0] = 32'h779e_abfb;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[3][31:0] = 32'h5f7b_fbd6;

		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[4][31:0] = 32'h9cfc_4e96;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[5][31:0] = 32'h7edb_808d;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[6][31:0] = 32'h679f_777b;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[7][31:0] = 32'hc670_2c7d;

		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[8][31:0] = 32'h39f2_3369;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[9][31:0] = 32'ha9d9_bacf;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[10][31:0] = 32'ha530_e263;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[11][31:0] = 32'h0423_1461;

		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[12][31:0] = 32'hb2eb_05e2;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[13][31:0] = 32'hc39b_e9fc;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[14][31:0] = 32'hda6c_1907;
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[15][31:0] = 32'h8c6a_9d1b;
	`endif

	// Always drop AXI memory
		for (i = 0; i <= top_address; i = i + 1) begin
			$write("mem_in[%d] of AXI memory: 0x%h\n",i,DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[i][31:0]);
    	end
	end

	// Reset FISMOS
	initial begin
		$display("\n\nFISMOS_TB");
		repeat (1000) @(posedge clk);

	`ifdef FISMOS_TB_TRIGGER_INTERRUPT_TO_FISMOS
		// Set control register and therefore trigger interrupt
		$write("FISMOS_TB: control_register: %h\n",DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.control_register[31:0]);
		DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.control_register[31:0] = 32'hdf00_0001;
		$write("FISMOS_TB: control_register: %h\n",DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.control_register[31:0]);
	`endif

	end

	// Output of FISMOS
	always @(posedge out32bit_en) begin
					$write("%c", out32bit[7:0]);
	end

	// Trap
	always @(posedge clk) begin
		if (resetn && trap) begin
			repeat (10) @(posedge clk);
			$display("FISMOS_TB: TRAP from PicoRV32 reached");
			$finish;
		end
	end

	// Interrupt to Linux
	always @(posedge interrupt_to_linux) begin
			$write("FISMOS_TB: interrupt_to_linux: SET\n");
	end
	always @(negedge interrupt_to_linux) begin
			$write("FISMOS_TB: interrupt_to_linux: RE_SET\n");
	end

	// End
	// Stop testbench if end of main() is reached
	// This is done by printing of '~' from the PicoRV32
	always @(posedge clk) begin
		if (out32bit[31:0] == 32'h0000_007E) begin
			$write("\n");
			// Drop AXI memory
			for (i = 0; i <= top_address; i = i + 1) begin
				$write("FISMOS_TB: mem_in[%d] of AXI memory: 0x%h\n",i,DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_in[i][31:0]);
    		end
			for (i = 0; i <= top_address; i = i + 1) begin
				$write("FISMOS_TB: mem_out[%d] of AXI memory: 0x%h\n",i,DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.bram_out[i][31:0]);
    		end
			$write("FISMOS_TB: control_register: %h\n",DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.control_register[31:0]);
			$write("FISMOS_TB: status_register: %h\n",DUT.fismos_AXI_shared_memory_0.axil_dp_ram_0.status_register[31:0]);
			$write("FISMOS_TB: interrupt_to_linux: %h\n",interrupt_to_linux);
			$finish;
		end
	end

endmodule
