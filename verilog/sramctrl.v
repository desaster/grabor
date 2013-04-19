/*
 * Based on:
 * http://greenbird.tweakblogs.net/blog/3459/sram-controller.html
 */
module sramctrl(clk,
    wren, d, q, address,
    WE, CE, OE, IO, A);

input clk;
input wren;
input [7:0] d;
input [18:0] address;
output reg [7:0] q;

output reg WE;
output reg CE;
output reg OE;
inout [7:0] IO;
output reg [18:0] A;

reg io_oe;
reg [7:0] buffer_d;

assign IO = io_oe ? buffer_d : 8'bZ;

always @(posedge clk) begin
    if (wren) begin
        // write
        A <= address;
        OE <= 1;
        CE <= 0;
        WE <= 0;
        io_oe <= 1;
    end
    else
    begin
        // read
        A <= address;
        CE <= 0;
        OE <= 0;
        WE <= 1;
        io_oe <= 0;
    end
    buffer_d <= d;
end

always @(negedge clk) begin
    if (wren == 0) begin
        // read
        q <= IO;
    end
end

endmodule
