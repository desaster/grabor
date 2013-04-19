/* Copyright (C) 2013 Upi Tamminen
 * See the COPYRIGHT file for more information */

module blinker(clk, LED);

input clk;
output LED;

reg [32:0] cnt;
always @(posedge clk) cnt <= cnt + 1;
assign LED = cnt[23];

endmodule

/* vim: set sw=4 et: */
