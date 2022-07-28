`timescale 1 ns / 1 ps

`define RESET_ONESHOT

module fismos #(
	parameter	AXI_ADDR_WIDTH = `FISMOS_SET_AXI_ADDR_WIDTH,
	parameter	AXI_DATA_WIDTH = `FISMOS_SET_AXI_DATA_WIDTH,

	// BlockRAM for processor memory
	// MEM_DEPTH = 1 -> 32bit, 4byte of memory
	// MEM_DEPTH = 1024: 32768bit, 4KByte
	parameter MEM_SIZE_KBYTE = `FISMOS_SET_CPU_MEM_SIZE_KBYTE,
	parameter MEM_DEPTH = MEM_SIZE_KBYTE * 1024
	)	(
	input								clk,
	input								resetn,

	`ifdef AXI_SLAVE_INTERFACE_ACTIVATE
	 // AXI slave interface
	 input								S_AXI_CLK,
	 input								S_AXI_RESETN,
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
	`endif

	`ifdef OUTLINE_INTERRUPT_TO_LINUX
	output							interrupt_to_linux,
	`endif

	output								trap,
	output								out32bit_en,
	output [31:0]						out32bit
);

	// PicoRV32 memory interface
	wire mem_valid;
	wire mem_instr;
	wire mem_ready;
	wire mem_ready_mem;
	wire mem_ready_io;
	wire mem_ready_AXI;
	wire interrupt_to_linux;
	wire [31:0] mem_addr;
	wire [31:0] mem_wdata;
	wire [3:0] mem_wstrb;
	wire [31:0] mem_rdata;
	wire [31:0] mem_rdata_mem;
	wire [31:0] mem_rdata_AXI;


	`ifndef AXI_SLAVE_INTERFACE_ACTIVATE
	// AXI slave interface
	wire								S_AXI_CLK;
	wire								S_AXI_RESETN;
	//
	wire								S_AXI_AWVALID;
	wire								S_AXI_AWREADY;
	wire	[AXI_ADDR_WIDTH-1:0]		S_AXI_AWADDR;
	wire	[2:0]						S_AXI_AWPROT;
	//
	wire								S_AXI_WVALID;
	wire								S_AXI_WREADY;
	wire	[AXI_DATA_WIDTH-1:0]		S_AXI_WDATA;
	wire	[AXI_DATA_WIDTH/8-1:0]		S_AXI_WSTRB;
	//
	wire								S_AXI_BVALID;
	wire								S_AXI_BREADY;
	wire	[1:0]						S_AXI_BRESP;
	//
	wire								S_AXI_ARVALID;
	wire								S_AXI_ARREADY;
	wire	[AXI_ADDR_WIDTH-1:0]		S_AXI_ARADDR;
	wire	[2:0]						S_AXI_ARPROT;
	//
	wire								S_AXI_RVALID;
	wire								S_AXI_RREADY;
	wire	[AXI_DATA_WIDTH-1:0]		S_AXI_RDATA;
	wire	[1:0]						S_AXI_RRESP;
	`endif

	`ifndef OUTLINE_INTERRUPT_TO_LINUX
	wire							interrupt_to_linux;
	`endif

	// Assign correct memory ready signal
	assign mem_ready = mem_ready_io ^ mem_ready_mem ^ mem_ready_AXI;

	// Assign correct memory data
	assign mem_rdata = mem_ready_AXI ? mem_rdata_AXI : mem_rdata_mem;

	// IRQ
	wire interrupt_to_softcore;
		
	// Reset control for run up of core
	wire resetn_init;

	`ifdef RESET_ONESHOT
		resetn_oneshot resetn_oneshot_0 (
			.clk(clk),
			.in_reset(resetn),
			.out_reset(resetn_init)
		);
	`else
		assign resetn_init = resetn;
	`endif
	
	picorv32 #(
		.ENABLE_IRQ(`FISMOS_ENABLE_INTERRUPT),
		.MASKED_IRQ(`FISMOS_MASKED_IRQ),
		.LATCHED_IRQ(`FISMOS_LATCHED_IRQ),
		.STACKADDR(`FISMOS_STACK_ADDRESS),
		.COMPRESSED_ISA(1)
	) picorv32_0 (
		.clk			(clk         ),
		.resetn			(resetn_init      ),
		.trap			(trap        ),
		.mem_valid		(mem_valid   ),
		.mem_instr		(mem_instr   ),
		.mem_ready		(mem_ready),
		.mem_addr		(mem_addr    ),
		.mem_wdata		(mem_wdata   ),
		.mem_wstrb		(mem_wstrb   ),
		.mem_rdata		( mem_rdata ),
		.irq({31'b0, interrupt_to_softcore})
	);

	cpu_memory #(
		.MEM_DEPTH (MEM_DEPTH)
	) cpu_memory_0 (
		.clk			(clk         ),
		.mem_valid		(mem_valid   ),
		.mem_ready		(mem_ready_mem),
		.mem_addr		(mem_addr    ),
		.mem_wdata		(mem_wdata   ),
		.mem_wstrb		(mem_wstrb   ),
		.mem_rdata		(mem_rdata_mem   )
	);

	io_memory #(
		.IO_MEM_DEPTH (1)
	) io_memory_0 (
		.clk			(clk         ),
		.resetn			(resetn_init      ),
		.out32bit_en	(out32bit_en ),
		.out32bit		(out32bit    ),
		.mem_valid		(mem_valid   ),
		.mem_ready		(mem_ready_io),
		.mem_addr		(mem_addr    ),
		.mem_wdata		(mem_wdata   ),
		.mem_wstrb		(mem_wstrb   )
	);

	fismos_AXI_shared_memory #(
		.AXI_DATA_WIDTH(AXI_DATA_WIDTH),
		.AXI_ADDR_WIDTH(AXI_ADDR_WIDTH),
		.AXI_STRB_WIDTH(AXI_DATA_WIDTH/8),
		.AXIRAM_PIPELINE_OUTPUT(0)
	) fismos_AXI_shared_memory_0 (
		// Interrupt lines
		.interrupt_to_softcore(interrupt_to_softcore),
		.interrupt_to_linux(interrupt_to_linux),
		// Global
		.S_AXI_ACLK(S_AXI_CLK),
		.S_AXI_ARESETN(S_AXI_RESETN),
		// Write Address
		.S_AXI_AWVALID(S_AXI_AWVALID),
		.S_AXI_AWREADY(S_AXI_AWREADY),
		.S_AXI_AWADDR(S_AXI_AWADDR),
		.S_AXI_AWPROT(S_AXI_AWPROT),
		// Write Data
		.S_AXI_WVALID(S_AXI_WVALID),
		.S_AXI_WREADY(S_AXI_WREADY),
		.S_AXI_WDATA(S_AXI_WDATA),
		.S_AXI_WSTRB(S_AXI_WSTRB),
		// Write Return
		.S_AXI_BVALID(S_AXI_BVALID),
		.S_AXI_BREADY(S_AXI_BREADY),
		.S_AXI_BRESP(S_AXI_BRESP),
		// Read Address
		.S_AXI_ARVALID(S_AXI_ARVALID),
		.S_AXI_ARREADY(S_AXI_ARREADY),
		.S_AXI_ARADDR(S_AXI_ARADDR),
		.S_AXI_ARPROT(S_AXI_ARPROT),
		// Read Return
		.S_AXI_RVALID(S_AXI_RVALID),
		.S_AXI_RREADY(S_AXI_RREADY),
		.S_AXI_RDATA(S_AXI_RDATA),
		.S_AXI_RRESP(S_AXI_RRESP),

		// PicoRV32 memory interface
		.clk(clk),
		.resetn(resetn_init),
		.mem_valid(mem_valid   ),
		.mem_instr(mem_instr   ),
		.mem_ready(mem_ready_AXI   ),
		.mem_addr(mem_addr    ),
		.mem_wdata(mem_wdata   ),
		.mem_wstrb(mem_wstrb   ),
		.mem_rdata(mem_rdata_AXI   )
	);

endmodule
