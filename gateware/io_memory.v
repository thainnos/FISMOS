module io_memory #(
	// 32 bit per IO_MEM_DEPTH
	parameter IO_MEM_DEPTH = 1
) (
	input clk,
	input resetn,

	// IO-Memory
	output reg out32bit_en = 0,
	output reg [31:0] out32bit,

	// PicoRV32 memory interface
	input mem_valid,
	output reg mem_ready,
	input [31:0] mem_addr,
	input [31:0] mem_wdata,
	input [3:0] mem_wstrb
);

    always @(posedge clk) begin
			mem_ready <= 0;

			out32bit_en <= 0;

			if (mem_valid && !mem_ready && |mem_wstrb && (mem_addr == `FISMOS_OUT32BIT_ADDRESS)) begin
					if (mem_wstrb[0]) out32bit[ 7: 0] <= mem_wdata[ 7: 0];
					if (mem_wstrb[1]) out32bit[15: 8] <= mem_wdata[15: 8];
					if (mem_wstrb[2]) out32bit[23:16] <= mem_wdata[23:16];
					if (mem_wstrb[3]) out32bit[31:24] <= mem_wdata[31:24];
					out32bit_en <= 1;
					mem_ready <= 1;
				end
			else if (!resetn) begin
			        out32bit <= 32'h0000_0000;
			    	out32bit_en <= 0;
			end
	end
endmodule