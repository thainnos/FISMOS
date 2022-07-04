module resetn_oneshot(
    input clk,
    input in_reset,
    output reg out_reset
);
	reg osout;
	reg q;
	reg oneshot;

initial begin
    oneshot <= 1'b0;
end

always @(posedge clk) begin
    if (!oneshot) begin
        out_reset <= 1'b0;
        oneshot <= 1'b1;
    end else begin
        out_reset <= in_reset; 
    end
end

endmodule