/***************************************************************
 * picorv32_axi_adapter
 ***************************************************************/

module picorv32_axi_adapter #(
		parameter	C_AXI_ADDR_WIDTH = 4,
		parameter 	C_AXI_DATA_WIDTH = 32
	) (
	input 							M_AXI_ACLK, 
	input 							M_AXI_ARESETN,
	// AXI4-lite master memory interface
	output        					M_AXI_AWVALID,
	input         					M_AXI_AWREADY,
	output [C_AXI_ADDR_WIDTH-1:0] 	M_AXI_AWADDR,
	output [ 2:0] 					M_AXI_AWPROT,
	//					
	output        					M_AXI_WVALID,
	input         					M_AXI_WREADY,
	output [C_AXI_DATA_WIDTH-1:0]	M_AXI_WDATA,
	output [C_AXI_DATA_WIDTH/8-1:0]	M_AXI_WSTRB,
	//				
	input         					M_AXI_BVALID,
	output        					M_AXI_BREADY,
	//				
	output        					M_AXI_ARVALID,
	input         					M_AXI_ARREADY,
	output [C_AXI_ADDR_WIDTH-1:0]	M_AXI_ARADDR,
	output [ 2:0] 					M_AXI_ARPROT,
	//				
	input         					M_AXI_RVALID,
	output        					M_AXI_RREADY,
	input  [C_AXI_DATA_WIDTH-1:0]	M_AXI_RDATA,

	// Native Pico	RV32 memory interface
	input         					mem_valid,
	input         					mem_instr,
	output        					mem_ready,
	input  [31:0] 					mem_addr,
	input  [31:0] 					mem_wdata,
	input  [ 3:0] 					mem_wstrb,
	output [31:0] 					mem_rdata
);
	reg ack_awvalid;
	reg ack_arvalid;
	reg ack_wvalid;
	reg xfer_done;

	assign M_AXI_AWVALID = mem_valid && |mem_wstrb && !ack_awvalid && mem_addr >= `FISMOS_CONTROL_REGISTER_ADDRESS;

	assign M_AXI_AWADDR = mem_addr[C_AXI_ADDR_WIDTH-1:0];

	assign M_AXI_AWPROT = 2'b00;
	assign M_AXI_ARVALID = mem_valid && !mem_wstrb && !ack_arvalid && mem_addr >= `FISMOS_CONTROL_REGISTER_ADDRESS;

	assign M_AXI_ARADDR = mem_addr[C_AXI_ADDR_WIDTH-1:0];

	assign M_AXI_ARPROT = mem_instr ? 3'b100 : 3'b000;

	assign M_AXI_WVALID = mem_valid && |mem_wstrb && !ack_wvalid && mem_addr >= `FISMOS_CONTROL_REGISTER_ADDRESS;
	assign M_AXI_WDATA = mem_wdata;
	assign M_AXI_WSTRB = mem_wstrb;

	assign mem_ready = M_AXI_BVALID || M_AXI_RVALID;
	assign M_AXI_BREADY = mem_valid && |mem_wstrb && mem_addr >= `FISMOS_CONTROL_REGISTER_ADDRESS;
	assign M_AXI_RREADY = mem_valid && !mem_wstrb && mem_addr >= `FISMOS_CONTROL_REGISTER_ADDRESS;
	assign mem_rdata = M_AXI_RDATA;

	always @(posedge M_AXI_ACLK) begin
		if (!M_AXI_ARESETN) begin
			ack_awvalid <= 0;
		end else begin
			xfer_done <= mem_valid && mem_ready;
			if (M_AXI_AWREADY && M_AXI_AWVALID)
				ack_awvalid <= 1;
			if (M_AXI_ARREADY && M_AXI_ARVALID)
				ack_arvalid <= 1;
			if (M_AXI_WREADY && M_AXI_WVALID)
				ack_wvalid <= 1;
			if (xfer_done || !mem_valid) begin
				ack_awvalid <= 0;
				ack_arvalid <= 0;
				ack_wvalid <= 0;
			end
		end
	end
endmodule
