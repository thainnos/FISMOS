`timescale 1 ns / 1 ps

module cpu_memory #(
	// Default case for error handling
	parameter MEM_DEPTH = -1 
) (
	input clk,

	// PicoRV32 memory interface
	input mem_valid,
	output reg mem_ready,
	input [31:0] mem_addr,
	input [31:0] mem_wdata,
	input [3:0] mem_wstrb,
	output reg  [31:0] mem_rdata
);

	// Do not generate cpu_memory if MEM_DEPTH is not defined
	if (MEM_DEPTH == -1) begin
		illegal_parameter_condition_triggered_will_instantiate_an non_existing_module();
	end 

	// Memory RAM
	(* RAM_STYLE="auto" *) reg [31:0] memory [0:(MEM_DEPTH/4-1)];
	initial $readmemh("../../firmware/FISMOS_firmware.hex", memory);

	// Memory logic
	always @(posedge clk) begin
		mem_ready <= 0;
		if (mem_valid && !mem_ready && mem_addr < MEM_DEPTH) begin
			mem_ready <= 1;
			if	(mem_addr < MEM_DEPTH) begin
					if (|mem_wstrb) begin
						if (mem_wstrb[0]) memory[mem_addr >> 2][ 7: 0] <= mem_wdata[ 7: 0];
						if (mem_wstrb[1]) memory[mem_addr >> 2][15: 8] <= mem_wdata[15: 8];
						if (mem_wstrb[2]) memory[mem_addr >> 2][23:16] <= mem_wdata[23:16];
						if (mem_wstrb[3]) memory[mem_addr >> 2][31:24] <= mem_wdata[31:24];
					end else begin
						mem_rdata <= memory[mem_addr >> 2];
					end
				end
			end
		end

endmodule
