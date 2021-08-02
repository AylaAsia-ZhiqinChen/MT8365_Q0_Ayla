// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// `timescale 1ns / 1ps
`define DISPLAY_IO


// Latency is defined as the difference between the cycle in which the
// lookup request goes out from Xtensa and the cycle in which the response
// is due back in to Xtensa.  TIE designers must specify the latency in
// their TIE code.
// The minimum legal latency is 1.
`define LATENCY 1
`define DELAY   `LATENCY


// Theory of operation:
//
// data_next[0] is the combinatorial result of the lookup based on the inputs
// data_next[1] is data_next[0] from the previous posedge CLK
// data_next[2] is data_next[1] from the previous posedge CLK
// data_next[3] is data_next[2] from the previous posedge CLK
// and so on.
//
// The output (TIE_lut_In) is tapped from the data_next entry defined by `DELAY

module lookup(CLK, TIE_lut_Out, TIE_lut_Out_Req, TIE_lut_In, TIE_lut_Rdy);

  input                 CLK;
  input  [   7:0]       TIE_lut_Out;
  input                 TIE_lut_Out_Req;

  output [  31:0]       TIE_lut_In;
  output                TIE_lut_Rdy;

  reg                   TIE_lut_Rdy;
  reg    [  31:0]       data_next[0:`LATENCY];

  integer               i;

  // Output the lookup result after `DELAY posedges of CLK
  assign TIE_lut_In = data_next[`DELAY];

  // We're always ready
  initial TIE_lut_Rdy <= 1'h1;

  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
  end

  // Shift the lookup result each posedge CLK
  always @(posedge CLK) begin
    for (i=`LATENCY; i>0; i=i-1) begin
      data_next[i] <= data_next[i-1];
    end
  end

`ifdef DISPLAY_IO
  always @(TIE_lut_Out    ) $display("%t TIE_lut_Out     = 0x%h", $time, TIE_lut_Out);
  always @(TIE_lut_Out_Req) $display("%t TIE_lut_Out_Req = %d",   $time, TIE_lut_Out_Req);
  always @(TIE_lut_In     ) $display("%t TIE_lut_In      = 0x%h", $time, TIE_lut_In );
  always @(TIE_lut_Rdy    ) $display("%t TIE_lut_Rdy     = 0x%h", $time, TIE_lut_Rdy);
`endif


  function [31:0] do_lookup;
    input [7:0] address;
         if (address == 8'h00) do_lookup = 32'hfacef00d;
    else if (address == 8'h33) do_lookup = 32'h33333333;
    else if (address == 8'h77) do_lookup = 32'h77777777;
    else if (address == 8'hbb) do_lookup = 32'hbbbbbbbb;
    else if (address == 8'h32) do_lookup = 32'h12345678;
    else if (address == 8'hff) do_lookup = 32'hffffffff;
    else if (address == 8'h10) do_lookup = 32'h10101010;
    else if (address == 8'h11) do_lookup = 32'hcafebabe;
    else if (address == 8'h22) do_lookup = 32'h22222222;
    else if (address == 8'hdd) do_lookup = 32'hdddddddd;
    else                       do_lookup = 32'hDEADBEEF;
  endfunction


  always @(TIE_lut_Out_Req or TIE_lut_Out or TIE_lut_Rdy) begin
    if (TIE_lut_Out_Req && TIE_lut_Rdy) begin
     data_next[0] <= do_lookup(TIE_lut_Out);
    end
    else begin
      data_next[0] <= 32'h00000000;
    end
  end

endmodule



