/* Based on the fpga4fun SPI tutorial */

/*
Since the SPI bus is typically much slower than the FPGA operating clock
speed, we choose to over-sample the SPI bus using the FPGA clock. That makes
the slave code slightly more complicated, but has the advantage of having the
SPI logic run in the FPGA clock domain, which will make things easier
afterwards.
*/

/* 16bit spi here */

module spihandler(clk, SCK, MOSI, MISO, SSEL,
    sendme, byte_received, byte_data_received);

input clk;
input SCK, SSEL, MOSI;
output MISO;
input [15:0] sendme;
output reg byte_received;
output reg [15:0] byte_data_received;
//output byte_received;  // high when a byte has been received
//output [15:0] byte_data_received;

// sync SCK to the FPGA clock using a 3-bits shift register
reg [2:0] SCKr;
always @(posedge clk) SCKr <= {SCKr[1:0], SCK};

// now we can detect SCK rising and falling edges
wire SCK_risingedge = (SCKr[2:1] == 2'b01);
wire SCK_fallingedge = (SCKr[2:1] == 2'b10);

// same thing for SSEL
reg [2:0] SSELr;
always @(posedge clk) SSELr <= {SSELr[1:0], SSEL};
wire SSEL_active = ~SSELr[1]; // SSEL is active low

// message starts at falling edge and stops at rising edge
wire SSEL_startmessage = (SSELr[2:1] == 2'b10);
wire SSEL_endmessage = (SSELr[2:1] == 2'b01);

// and for MOSI
reg [1:0] MOSIr;  always @(posedge clk) MOSIr <= {MOSIr[0], MOSI};
wire MOSI_data = MOSIr[1];

/* Now receiving data from the SPI bus is easy. */

reg [4:0] bitcnt; // 4 bits to count 16 bits
//reg byte_received;  // high when a byte has been received
//reg [15:0] byte_data_received;

always @(posedge clk)
begin
    if (~SSEL_active)
        bitcnt <= 4'b0000;
    else
    if (SCK_risingedge)
    begin
        bitcnt <= bitcnt + 4'b0001;
        // implement a shift-left register (since we receive the data MSB first)
        byte_data_received <= {byte_data_received[14:0], MOSI_data};
    end
end

always @(posedge clk)
begin
    byte_received <= SSEL_active && SCK_risingedge && (bitcnt == 4'b1111);
end

/* Finally the transmission part. */

reg [15:0] byte_data_sent;
//reg [15:0] cnt;
//always @(posedge clk) cnt <= 43690; /* test data to be sent out */
//always @(posedge clk) if (SSEL_startmessage) cnt<=cnt+8'h1;  // count the messages

always @(posedge clk)
if (SSEL_active)
begin
    if (SSEL_startmessage)
        byte_data_sent <= sendme;
    else
    if (SCK_fallingedge)
    begin
        if (bitcnt == 4'b0000)
            byte_data_sent <= 8'h0000;  // after that, we send 0s
        else
            byte_data_sent <= {byte_data_sent[14:0], 1'b0};
    end
end

assign MISO = byte_data_sent[15];  // send MSB first

// we assume that there is only one slave on the SPI bus
// so we don't bother with a tri-state buffer for MISO
// otherwise we would need to tri-state MISO when SSEL is inactive

endmodule

/* vim: set sw=4 et: */
