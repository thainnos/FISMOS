// Language: Verilog 2001
`timescale 1ns / 1ps

/*
 * AXI4-Lite dual port RAM
 */
module axil_dp_ram #
(
    // Width of data bus in bits
    parameter AXI_DATA_WIDTH = 0,
    // Width of address bus in bits
    parameter AXI_ADDR_WIDTH = 0,
    // Width of wstrb (width of data bus in words)
    parameter AXI_STRB_WIDTH = (AXI_DATA_WIDTH/8),
    // Extra pipeline register on output
    parameter AXIRAM_PIPELINE_OUTPUT = 0    
)
(    
    // IRQ
    output reg interrupt_to_softcore,
    output reg interrupt_to_linux,

    // Port A
    input  wire                   S_AXIL_A_CLK,
    input  wire                   S_AXIL_A_RESETN,
    // AXI4-lite slave memory interface
    input  wire                   S_AXIL_A_AWVALID,
    output wire                   S_AXIL_A_AWREADY,
    input  wire [AXI_ADDR_WIDTH-1:0]  S_AXIL_A_AWADDR,
    input  wire [2:0]             S_AXIL_A_AWPROT,
    //
    input  wire                   S_AXIL_A_WVALID,
    output wire                   S_AXIL_A_WREADY,
    input  wire [AXI_DATA_WIDTH-1:0]  S_AXIL_A_WDATA,
    input  wire [AXI_STRB_WIDTH-1:0]  S_AXIL_A_WSTRB,
    //
    output wire                   S_AXIL_A_BVALID,
    input  wire                   S_AXIL_A_BREADY,
    output wire [1:0]             S_AXIL_A_BRESP,
    //
    input  wire                   S_AXIL_A_ARVALID,
    output wire                   S_AXIL_A_ARREADY,
    input  wire [AXI_ADDR_WIDTH-1:0]  S_AXIL_A_ARADDR,
    input  wire [2:0]             S_AXIL_A_ARPROT,
    //
    output wire                   S_AXIL_A_RVALID,
    input  wire                   S_AXIL_A_RREADY,
    output wire [AXI_DATA_WIDTH-1:0]  S_AXIL_A_RDATA,
    output wire [1:0]             S_AXIL_A_RRESP,

    //
    // Port B
    input  wire                   S_AXIL_B_CLK,
    input  wire                   S_AXIL_B_RESETN,
    // AXI4-lite slave memory interface
    input  wire                   S_AXIL_B_AWVALID,
    output wire                   S_AXIL_B_AWREADY,
    input  wire [AXI_ADDR_WIDTH-1:0]  S_AXIL_B_AWADDR,
    input  wire [2:0]             S_AXIL_B_AWPROT,
    //
    input  wire                   S_AXIL_B_WVALID,
    output wire                   S_AXIL_B_WREADY,
    input  wire [AXI_DATA_WIDTH-1:0]  S_AXIL_B_WDATA,
    input  wire [AXI_STRB_WIDTH-1:0]  S_AXIL_B_WSTRB,
    //
    output wire                   S_AXIL_B_BVALID,
    input  wire                   S_AXIL_B_BREADY,
    output wire [1:0]             S_AXIL_B_BRESP,
    //
    input  wire                   S_AXIL_B_ARVALID,
    output wire                   S_AXIL_B_ARREADY,
    input  wire [AXI_ADDR_WIDTH-1:0]  S_AXIL_B_ARADDR,
    input  wire [2:0]             S_AXIL_B_ARPROT,
    //
    output wire                   S_AXIL_B_RVALID,
    input  wire                   S_AXIL_B_RREADY,
    output wire [AXI_DATA_WIDTH-1:0]  S_AXIL_B_RDATA,
    output wire [1:0]             S_AXIL_B_RRESP
);

localparam VALID_AXI_ADDR_WIDTH = AXI_ADDR_WIDTH - $clog2(AXI_STRB_WIDTH);
localparam WORD_WIDTH = AXI_STRB_WIDTH;
localparam WORD_SIZE = AXI_DATA_WIDTH/WORD_WIDTH;

reg read_eligible_a;
reg write_eligible_a;

reg read_eligible_b;
reg write_eligible_b;

reg mem_wr_en_a;
reg mem_rd_en_a;

reg mem_wr_en_b;
reg mem_rd_en_b;

reg last_read_a_reg = 1'b0, last_read_a_next;
reg last_read_b_reg = 1'b0, last_read_b_next;

reg S_AXIL_A_AWREADY_reg = 1'b0, S_AXIL_A_AWREADY_next;
reg S_AXIL_A_WREADY_reg = 1'b0, S_AXIL_A_WREADY_next;
reg S_AXIL_A_BVALID_reg = 1'b0, S_AXIL_A_BVALID_next;
reg S_AXIL_A_ARREADY_reg = 1'b0, S_AXIL_A_ARREADY_next;
reg [AXI_DATA_WIDTH-1:0] S_AXIL_A_RDATA_reg = {AXI_DATA_WIDTH{1'b0}}, S_AXIL_A_RDATA_next;
reg S_AXIL_A_RVALID_reg = 1'b0, S_AXIL_A_RVALID_next;
reg [AXI_DATA_WIDTH-1:0] S_AXIL_A_RDATA_pipe_reg = {AXI_DATA_WIDTH{1'b0}};
reg S_AXIL_A_RVALID_pipe_reg = 1'b0;

reg S_AXIL_B_AWREADY_reg = 1'b0, S_AXIL_B_AWREADY_next;
reg S_AXIL_B_WREADY_reg = 1'b0, S_AXIL_B_WREADY_next;
reg S_AXIL_B_BVALID_reg = 1'b0, S_AXIL_B_BVALID_next;
reg S_AXIL_B_ARREADY_reg = 1'b0, S_AXIL_B_ARREADY_next;
reg [AXI_DATA_WIDTH-1:0] S_AXIL_B_RDATA_reg = {AXI_DATA_WIDTH{1'b0}}, S_AXIL_B_RDATA_next;
reg S_AXIL_B_RVALID_reg = 1'b0, S_AXIL_B_RVALID_next;
reg [AXI_DATA_WIDTH-1:0] S_AXIL_B_RDATA_pipe_reg = {AXI_DATA_WIDTH{1'b0}};
reg S_AXIL_B_RVALID_pipe_reg = 1'b0;


// Control register
reg [31:0] control_register = 0;
reg interrupt_to_softcore_triggered = 0;

// Status register
reg [31:0] status_register = 0;

// Block RAM
(* RAM_STYLE="auto" *) reg [AXI_DATA_WIDTH-1:0] bram_in[(2**(VALID_AXI_ADDR_WIDTH - 1))-1:0];
(* RAM_STYLE="auto" *) reg [AXI_DATA_WIDTH-1:0] bram_out[(2**(VALID_AXI_ADDR_WIDTH - 1))-1:0];

// Adress lines
wire [VALID_AXI_ADDR_WIDTH-1:0] S_AXIL_A_AWADDR_valid = S_AXIL_A_AWADDR >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH);
wire [VALID_AXI_ADDR_WIDTH-1:0] S_AXIL_A_ARADDR_valid = S_AXIL_A_ARADDR >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH);

wire [VALID_AXI_ADDR_WIDTH-1:0] S_AXIL_B_AWADDR_valid = S_AXIL_B_AWADDR >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH);
wire [VALID_AXI_ADDR_WIDTH-1:0] S_AXIL_B_ARADDR_valid = S_AXIL_B_ARADDR >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH);

// Unify memory address for successfull synth in Vivado
wire [VALID_AXI_ADDR_WIDTH-1:0] S_AXIL_A_ADDR_access = mem_rd_en_a ? S_AXIL_A_ARADDR_valid : S_AXIL_A_AWADDR_valid;
wire [VALID_AXI_ADDR_WIDTH-1:0] S_AXIL_B_ADDR_access = mem_rd_en_b ? S_AXIL_B_ARADDR_valid : S_AXIL_B_AWADDR_valid;

assign S_AXIL_A_AWREADY = S_AXIL_A_AWREADY_reg;
assign S_AXIL_A_WREADY = S_AXIL_A_WREADY_reg;
assign S_AXIL_A_BRESP = 2'b00;
assign S_AXIL_A_BVALID = S_AXIL_A_BVALID_reg;
assign S_AXIL_A_ARREADY = S_AXIL_A_ARREADY_reg;
assign S_AXIL_A_RDATA = AXIRAM_PIPELINE_OUTPUT ? S_AXIL_A_RDATA_pipe_reg : S_AXIL_A_RDATA_reg;
assign S_AXIL_A_RRESP = 2'b00;
assign S_AXIL_A_RVALID = AXIRAM_PIPELINE_OUTPUT ? S_AXIL_A_RVALID_pipe_reg : S_AXIL_A_RVALID_reg;

assign S_AXIL_B_AWREADY = S_AXIL_B_AWREADY_reg;
assign S_AXIL_B_WREADY = S_AXIL_B_WREADY_reg;
assign S_AXIL_B_BRESP = 2'b00;
assign S_AXIL_B_BVALID = S_AXIL_B_BVALID_reg;
assign S_AXIL_B_ARREADY = S_AXIL_B_ARREADY_reg;
assign S_AXIL_B_RDATA = AXIRAM_PIPELINE_OUTPUT ? S_AXIL_B_RDATA_pipe_reg : S_AXIL_B_RDATA_reg;
assign S_AXIL_B_RRESP = 2'b00;
assign S_AXIL_B_RVALID = AXIRAM_PIPELINE_OUTPUT ? S_AXIL_B_RVALID_pipe_reg : S_AXIL_B_RVALID_reg;

integer i, j;

// Set bram_in to 0x00000000
initial begin
    // two nested loops for smaller number of iterations per loop
    // workaround for synthesizer complaints about large loop counts
    for (i = 0; i < 2**VALID_AXI_ADDR_WIDTH; i = i + 2**(VALID_AXI_ADDR_WIDTH/2)) begin
        for (j = i; j < i + 2**(VALID_AXI_ADDR_WIDTH/2); j = j + 1) begin
            bram_in[j] = 0;
            bram_out[j] = 0;
        end
    end
    interrupt_to_linux = 1;
    interrupt_to_softcore = 0;
end

// Asynchronous logic for assigning control signals
always @* begin
    // Port A
    mem_wr_en_a = 1'b0;
    mem_rd_en_a = 1'b0;

    last_read_a_next = last_read_a_reg;

    S_AXIL_A_AWREADY_next = 1'b0;
    S_AXIL_A_WREADY_next = 1'b0;
    S_AXIL_A_BVALID_next = S_AXIL_A_BVALID_reg && !S_AXIL_A_BREADY;

    S_AXIL_A_ARREADY_next = 1'b0;
    S_AXIL_A_RVALID_next = S_AXIL_A_RVALID_reg && !(S_AXIL_A_RREADY || (AXIRAM_PIPELINE_OUTPUT && !S_AXIL_A_RVALID_pipe_reg));

    write_eligible_a = S_AXIL_A_AWVALID && S_AXIL_A_WVALID && (!S_AXIL_A_BVALID || S_AXIL_A_BREADY) && (!S_AXIL_A_AWREADY && !S_AXIL_A_WREADY);
    read_eligible_a = S_AXIL_A_ARVALID && (!S_AXIL_A_RVALID || S_AXIL_A_RREADY || (AXIRAM_PIPELINE_OUTPUT && !S_AXIL_A_RVALID_pipe_reg)) && (!S_AXIL_A_ARREADY);

    if (write_eligible_a && (!read_eligible_a || last_read_a_reg)) begin
        last_read_a_next = 1'b0;

        S_AXIL_A_AWREADY_next = 1'b1;
        S_AXIL_A_WREADY_next = 1'b1;
        S_AXIL_A_BVALID_next = 1'b1;

        mem_wr_en_a = 1'b1;
    end else if (read_eligible_a) begin
        last_read_a_next = 1'b1;

        S_AXIL_A_ARREADY_next = 1'b1;
        S_AXIL_A_RVALID_next = 1'b1;

        mem_rd_en_a = 1'b1;
    end
    
    // Port B   
    mem_wr_en_b = 1'b0;
    mem_rd_en_b = 1'b0;

    last_read_b_next = last_read_b_reg;

    S_AXIL_B_AWREADY_next = 1'b0;
    S_AXIL_B_WREADY_next = 1'b0;
    S_AXIL_B_BVALID_next = S_AXIL_B_BVALID_reg && !S_AXIL_B_BREADY;

    S_AXIL_B_ARREADY_next = 1'b0;
    S_AXIL_B_RVALID_next = S_AXIL_B_RVALID_reg && !(S_AXIL_B_RREADY || (AXIRAM_PIPELINE_OUTPUT && !S_AXIL_B_RVALID_pipe_reg));

    write_eligible_b = S_AXIL_B_AWVALID && S_AXIL_B_WVALID && (!S_AXIL_B_BVALID || S_AXIL_B_BREADY) && (!S_AXIL_B_AWREADY && !S_AXIL_B_WREADY);
    read_eligible_b = S_AXIL_B_ARVALID && (!S_AXIL_B_RVALID || S_AXIL_B_RREADY || (AXIRAM_PIPELINE_OUTPUT && !S_AXIL_B_RVALID_pipe_reg)) && (!S_AXIL_B_ARREADY);

    if (write_eligible_b && (!read_eligible_b || last_read_b_reg)) begin
        last_read_b_next = 1'b0;

        S_AXIL_B_AWREADY_next = 1'b1;
        S_AXIL_B_WREADY_next = 1'b1;
        S_AXIL_B_BVALID_next = 1'b1;

        mem_wr_en_b = 1'b1;
    end else if (read_eligible_b) begin
        last_read_b_next = 1'b1;

        S_AXIL_B_ARREADY_next = 1'b1;
        S_AXIL_B_RVALID_next = 1'b1;

        mem_rd_en_b = 1'b1;
    end
end

// Linux bram_in access
always @(posedge S_AXIL_A_CLK) begin
    last_read_a_reg <= last_read_a_next;

    S_AXIL_A_AWREADY_reg <= S_AXIL_A_AWREADY_next;
    S_AXIL_A_WREADY_reg <= S_AXIL_A_WREADY_next;
    S_AXIL_A_BVALID_reg <= S_AXIL_A_BVALID_next;

    S_AXIL_A_ARREADY_reg <= S_AXIL_A_ARREADY_next;
    S_AXIL_A_RVALID_reg <= S_AXIL_A_RVALID_next;

    // Read status register
    if (mem_rd_en_a && (S_AXIL_A_ADDR_access == (`FISMOS_STATUS_REGISTER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
        S_AXIL_A_RDATA_reg <= status_register;
    end    
    // Read control register
    else if (mem_rd_en_a && (S_AXIL_A_ADDR_access == (`FISMOS_CONTROL_REGISTER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
        S_AXIL_A_RDATA_reg <= control_register;
    end
    // Write control register
    else if (mem_wr_en_a && (S_AXIL_A_ADDR_access == (`FISMOS_CONTROL_REGISTER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
     control_register <= S_AXIL_A_WDATA;
    end
    else begin

    // Default memory access
    if (mem_rd_en_a && (S_AXIL_A_ADDR_access >= (`FISMOS_EXCHANGE_BUFFER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
        S_AXIL_A_RDATA_reg <= bram_out[S_AXIL_A_ADDR_access - (`FISMOS_EXCHANGE_BUFFER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH))];
    end else begin
        for (i = 0; i < WORD_WIDTH; i = i + 1) begin
            if (mem_wr_en_a && S_AXIL_A_WSTRB[i] && (S_AXIL_A_ADDR_access >= (`FISMOS_EXCHANGE_BUFFER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
                bram_in[S_AXIL_A_ADDR_access - (`FISMOS_EXCHANGE_BUFFER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH))][WORD_SIZE*i +: WORD_SIZE] <= S_AXIL_A_WDATA[WORD_SIZE*i +: WORD_SIZE];
            end
        end
    end
end

    if (!S_AXIL_A_RVALID_pipe_reg || S_AXIL_A_RREADY) begin
        S_AXIL_A_RDATA_pipe_reg <= S_AXIL_A_RDATA_reg;
        S_AXIL_A_RVALID_pipe_reg <= S_AXIL_A_RVALID_reg;
    end

    if (!S_AXIL_A_RESETN) begin
        last_read_a_reg <= 1'b0;

        S_AXIL_A_AWREADY_reg <= 1'b0;
        S_AXIL_A_WREADY_reg <= 1'b0;
        S_AXIL_A_BVALID_reg <= 1'b0;

        S_AXIL_A_ARREADY_reg <= 1'b0;
        S_AXIL_A_RVALID_reg <= 1'b0;
        S_AXIL_A_RVALID_pipe_reg <= 1'b0;
    end
end

// PicoRV32 bram_in access
always @(posedge S_AXIL_B_CLK) begin
    last_read_b_reg <= last_read_b_next;

    S_AXIL_B_AWREADY_reg <= S_AXIL_B_AWREADY_next;
    S_AXIL_B_WREADY_reg <= S_AXIL_B_WREADY_next;
    S_AXIL_B_BVALID_reg <= S_AXIL_B_BVALID_next;

    S_AXIL_B_ARREADY_reg <= S_AXIL_B_ARREADY_next;
    S_AXIL_B_RVALID_reg <= S_AXIL_B_RVALID_next;

    // Read control register
    if (mem_rd_en_b && (S_AXIL_B_ADDR_access == (`FISMOS_CONTROL_REGISTER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
        S_AXIL_B_RDATA_reg <= control_register;
 end   
  // Read status register
 else if (mem_rd_en_b && (S_AXIL_B_ADDR_access == (`FISMOS_STATUS_REGISTER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
        S_AXIL_B_RDATA_reg <= status_register;
 end
    // Write status register
 else if (mem_wr_en_b && (S_AXIL_B_ADDR_access == (`FISMOS_STATUS_REGISTER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
     status_register <= S_AXIL_B_WDATA;
 end
 else begin

     // Default memory access
    if (mem_rd_en_b && (S_AXIL_B_ADDR_access >= (`FISMOS_EXCHANGE_BUFFER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
        S_AXIL_B_RDATA_reg <= bram_in[S_AXIL_B_ADDR_access - (`FISMOS_EXCHANGE_BUFFER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH))];
    end else begin
        for (i = 0; i < WORD_WIDTH; i = i + 1) begin
            if (mem_wr_en_b && S_AXIL_B_WSTRB[i] && (S_AXIL_B_ADDR_access >= (`FISMOS_EXCHANGE_BUFFER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH)))) begin
                bram_out[S_AXIL_B_ADDR_access - (`FISMOS_EXCHANGE_BUFFER_OFFSET >> (AXI_ADDR_WIDTH - VALID_AXI_ADDR_WIDTH))][WORD_SIZE*i +: WORD_SIZE] <= S_AXIL_B_WDATA[WORD_SIZE*i +: WORD_SIZE];
            end
        end
    end
    
 end

    if (!S_AXIL_B_RVALID_pipe_reg || S_AXIL_B_RREADY) begin
        S_AXIL_B_RDATA_pipe_reg <= S_AXIL_B_RDATA_reg;
        S_AXIL_B_RVALID_pipe_reg <= S_AXIL_B_RVALID_reg;
    end

    if (!S_AXIL_B_RESETN) begin
        last_read_b_reg <= 1'b0;

        S_AXIL_B_AWREADY_reg <= 1'b0;
        S_AXIL_B_WREADY_reg <= 1'b0;
        S_AXIL_B_BVALID_reg <= 1'b0;

        S_AXIL_B_ARREADY_reg <= 1'b0;
        S_AXIL_B_RVALID_reg <= 1'b0;
        S_AXIL_B_RVALID_pipe_reg <= 1'b0;
    end
end

// Interrupt incoming from Linux to Softcore
`ifdef FISMOS_STATIC_INTERRUPT_TO_PICORV32
    always @(posedge S_AXIL_B_CLK) begin
            interrupt_to_softcore = 1;
    end
`else
    always @(posedge S_AXIL_B_CLK) begin
       if (status_register == 32'h0000ffff) begin
           interrupt_to_softcore = 0;        
       end else if (control_register[24] && !interrupt_to_softcore_triggered) begin
           interrupt_to_softcore = 1;
           interrupt_to_softcore_triggered = 1;
       end else if (!control_register[24] && interrupt_to_softcore_triggered) begin
           interrupt_to_softcore_triggered = 0;
       end
    end
`endif

// // Interrupt outgoing from Softcore to Linux
`ifdef FISMOS_DEBUG_STATIC_INTERRUPT_TO_LINUX
    always @* begin
            interrupt_to_linux = 1;
    end
`else
    always @* begin
       if (status_register[24]) begin
           interrupt_to_linux = 1;
       end else if (status_register[23:0] == 24'hff_ffff) begin
           interrupt_to_linux = 0;
       end
    end
`endif

endmodule
