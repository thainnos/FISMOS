`timescale 1 ns / 1 ps

module fismos_AXI_shared_memory #
(
	// Width of data bus in bits
    parameter AXI_DATA_WIDTH = 0,
    // Width of address bus in bits
    parameter AXI_ADDR_WIDTH = 0,
    // Width of wstrb (width of data bus in words)
    parameter AXI_STRB_WIDTH = (AXI_DATA_WIDTH/8),
    // Extra pipeline register on output
    parameter AXIRAM_PIPELINE_OUTPUT = 0
)	(
    // IRQ
    output wire interrupt_to_softcore,
	output wire interrupt_to_linux,
	
	// AXI slave interface
	input								S_AXI_ACLK,
	input								S_AXI_ARESETN,
	//
	input								S_AXI_AWVALID,
	output								S_AXI_AWREADY,
	input	[AXI_ADDR_WIDTH-1:0]		S_AXI_AWADDR,
	input	[2:0]						S_AXI_AWPROT,
	//
	input								S_AXI_WVALID,
	output								S_AXI_WREADY,
	input	[AXI_DATA_WIDTH-1:0]		S_AXI_WDATA,
	input	[AXI_DATA_WIDTH/8-1:0]		S_AXI_WSTRB,
	//
	output								S_AXI_BVALID,
	input								S_AXI_BREADY,
	output	[1:0]						S_AXI_BRESP,
	//
	input								S_AXI_ARVALID,
	output								S_AXI_ARREADY,
	input	[AXI_ADDR_WIDTH-1:0]		S_AXI_ARADDR,
	input	[2:0]						S_AXI_ARPROT,
	//
	output								S_AXI_RVALID,
	input								S_AXI_RREADY,
	output	[AXI_DATA_WIDTH-1:0]		S_AXI_RDATA,
	output	[1:0]						S_AXI_RRESP,

    // PicoRV32 memory interface
	input		                        clk,
	input		                        resetn,
	input         					    mem_valid,
	input         					    mem_instr,
	output        					    mem_ready,
	input  [31:0] 					    mem_addr,
	input  [31:0] 					    mem_wdata,
	input  [ 3:0] 					    mem_wstrb,
	output [31:0] 					    mem_rdata
);

	// AXI Lite wires from Pico-to-AXI-Adapter to Dual-Port RAM
	wire								M_AXIL_PICO_AWVALID;
	wire								M_AXIL_PICO_AWREADY;
	wire	[AXI_ADDR_WIDTH-1:0]		M_AXIL_PICO_AWADDR;
	wire	[2:0]						M_AXIL_PICO_AWPROT;
	//
	wire								M_AXIL_PICO_WVALID;
	wire								M_AXIL_PICO_WREADY;
	wire	[AXI_DATA_WIDTH-1:0]		M_AXIL_PICO_WDATA;
	wire	[AXI_DATA_WIDTH/8-1:0]		M_AXIL_PICO_WSTRB;
	//
	wire								M_AXIL_PICO_BVALID;
	wire								M_AXIL_PICO_BREADY;
	wire	[1:0]						M_AXIL_PICO_BRESP;
	//
	wire								M_AXIL_PICO_ARVALID;
	wire								M_AXIL_PICO_ARREADY;
	wire	[AXI_ADDR_WIDTH-1:0]		M_AXIL_PICO_ARADDR;
	wire	[2:0]						M_AXIL_PICO_ARPROT;
	//
	wire								M_AXIL_PICO_RVALID;
	wire								M_AXIL_PICO_RREADY;
	wire	[AXI_DATA_WIDTH-1:0]		M_AXIL_PICO_RDATA;
	wire	[1:0]						M_AXIL_PICO_RRESP;


	// Register
	reg [31:0] control_register = 0;
	reg [31:0] status_register = 0;

	always @(*) begin
		control_register = status_register;
	end

	picorv32_axi_adapter #(
		.C_AXI_ADDR_WIDTH(AXI_ADDR_WIDTH),
		.C_AXI_DATA_WIDTH(AXI_DATA_WIDTH)
	) picorv32_axi_adapter_0 (
		.M_AXI_ACLK(clk), 
		.M_AXI_ARESETN(resetn),
		//
		.M_AXI_AWVALID(M_AXIL_PICO_AWVALID),
		.M_AXI_AWREADY(M_AXIL_PICO_AWREADY),
		.M_AXI_AWADDR(M_AXIL_PICO_AWADDR),
		.M_AXI_AWPROT(M_AXIL_PICO_AWPROT),
		//
		.M_AXI_WVALID(M_AXIL_PICO_WVALID),
		.M_AXI_WREADY(M_AXIL_PICO_WREADY),
		.M_AXI_WDATA(M_AXIL_PICO_WDATA),
		.M_AXI_WSTRB(M_AXIL_PICO_WSTRB),
		//
		.M_AXI_BVALID(M_AXIL_PICO_BVALID),
		.M_AXI_BREADY(M_AXIL_PICO_BREADY),
		//
		.M_AXI_ARVALID(M_AXIL_PICO_ARVALID),
		.M_AXI_ARREADY(M_AXIL_PICO_ARREADY),
		.M_AXI_ARADDR(M_AXIL_PICO_ARADDR),
		.M_AXI_ARPROT(M_AXIL_PICO_ARPROT),
		//
		.M_AXI_RVALID(M_AXIL_PICO_RVALID),
		.M_AXI_RREADY(M_AXIL_PICO_RREADY),
		.M_AXI_RDATA(M_AXIL_PICO_RDATA),
		//
		.mem_valid		(mem_valid   ),
		.mem_instr		(mem_instr   ),
		.mem_ready		(mem_ready   ),
		.mem_addr		(mem_addr    ),
		.mem_wdata		(mem_wdata   ),
		.mem_wstrb		(mem_wstrb   ),
		.mem_rdata		(mem_rdata   )
	);

	axil_dp_ram #(
		.AXI_DATA_WIDTH(AXI_DATA_WIDTH),
		.AXI_ADDR_WIDTH(AXI_ADDR_WIDTH),
		.AXI_STRB_WIDTH(AXI_DATA_WIDTH/8),
		.AXIRAM_PIPELINE_OUTPUT(0)
	) axil_dp_ram_0 (
		.interrupt_to_softcore(interrupt_to_softcore),
		.interrupt_to_linux(interrupt_to_linux),
        // Port A from extern into FISMOS
		.S_AXIL_A_CLK(S_AXI_ACLK),
		.S_AXIL_A_RESETN(S_AXI_ARESETN),
		//
		.S_AXIL_A_AWVALID(S_AXI_AWVALID),
		.S_AXIL_A_AWREADY(S_AXI_AWREADY),
		.S_AXIL_A_AWADDR(S_AXI_AWADDR),
		.S_AXIL_A_AWPROT(S_AXI_AWPROT),
		//
		.S_AXIL_A_WVALID(S_AXI_WVALID),
		.S_AXIL_A_WREADY(S_AXI_WREADY),
		.S_AXIL_A_WDATA(S_AXI_WDATA),
		.S_AXIL_A_WSTRB(S_AXI_WSTRB),
		//L_A
		.S_AXIL_A_BVALID(S_AXI_BVALID),
		.S_AXIL_A_BREADY(S_AXI_BREADY),
		.S_AXIL_A_BRESP(S_AXI_BRESP),
		//
		.S_AXIL_A_ARVALID(S_AXI_ARVALID),
		.S_AXIL_A_ARREADY(S_AXI_ARREADY),
		.S_AXIL_A_ARADDR(S_AXI_ARADDR),
		.S_AXIL_A_ARPROT(S_AXI_ARPROT),
		//
		.S_AXIL_A_RVALID(S_AXI_RVALID),
		.S_AXIL_A_RREADY(S_AXI_RREADY),
		.S_AXIL_A_RDATA(S_AXI_RDATA),
		.S_AXIL_A_RRESP(S_AXI_RRESP),

        // Port B from Pico-to-AXI bridge
		.S_AXIL_B_CLK(clk),
		.S_AXIL_B_RESETN(resetn),
		//
		.S_AXIL_B_AWVALID(M_AXIL_PICO_AWVALID),
		.S_AXIL_B_AWREADY(M_AXIL_PICO_AWREADY),
		.S_AXIL_B_AWADDR(M_AXIL_PICO_AWADDR),
		.S_AXIL_B_AWPROT(M_AXIL_PICO_AWPROT),
		//
		.S_AXIL_B_WVALID(M_AXIL_PICO_WVALID),
		.S_AXIL_B_WREADY(M_AXIL_PICO_WREADY),
		.S_AXIL_B_WDATA(M_AXIL_PICO_WDATA),
		.S_AXIL_B_WSTRB(M_AXIL_PICO_WSTRB),
		//L_A
		.S_AXIL_B_BVALID(M_AXIL_PICO_BVALID),
		.S_AXIL_B_BREADY(M_AXIL_PICO_BREADY),
		.S_AXIL_B_BRESP(M_AXIL_PICO_BRESP),
		//
		.S_AXIL_B_ARVALID(M_AXIL_PICO_ARVALID),
		.S_AXIL_B_ARREADY(M_AXIL_PICO_ARREADY),
		.S_AXIL_B_ARADDR(M_AXIL_PICO_ARADDR),
		.S_AXIL_B_ARPROT(M_AXIL_PICO_ARPROT),
		//
		.S_AXIL_B_RVALID(M_AXIL_PICO_RVALID),
		.S_AXIL_B_RREADY(M_AXIL_PICO_RREADY),
		.S_AXIL_B_RDATA(M_AXIL_PICO_RDATA),
		.S_AXIL_B_RRESP(M_AXIL_PICO_RRESP)
	);

endmodule
