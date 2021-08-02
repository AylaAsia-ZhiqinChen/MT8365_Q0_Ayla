// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// `timescale 1ns / 1ps

// Latency is defined as def_stage minus use_stage (where def_stage and
// use_stage are specified in the TIE lookup section of the TIE code).
`define LATENCY         1
`define ABITS           8

`define INITIAL_VALUE   32'hBAD1BAD1
`define DELAY           `LATENCY

module lookup(CLK, TIE_lookup_ram_Out_Req, TIE_lookup_ram_Out, TIE_lookup_ram_In);

  input                         CLK;
  input                         TIE_lookup_ram_Out_Req;
  input         [40:0]          TIE_lookup_ram_Out;
  output        [31:0]          TIE_lookup_ram_In;

  reg           [31:0]          read_data_next[0:`LATENCY];
  reg           [31:0]          mem[0:(1<<`ABITS)-1];
  reg           [ 7:0]          delay;

  wire          [`ABITS-1:0]    address;
  wire          [31:0]          data;
  wire                          write;

  integer                       i;

  assign write                  = TIE_lookup_ram_Out[40:40];
  assign address                = TIE_lookup_ram_Out[39:32];
  assign data                   = TIE_lookup_ram_Out[31:0];
  assign TIE_lookup_ram_In      = read_data_next[`DELAY];

  initial begin
      delay = `DELAY;
      for (i=0; i<(1<<`ABITS); i=i+1) begin
          mem[i] = `INITIAL_VALUE;
      end
  end

  always @(mem[address]) begin
      read_data_next[0] <= mem[address];
  end

  always @(posedge CLK) begin
      for (i=`LATENCY; i>0; i=i-1) begin
          read_data_next[i] <= read_data_next[i-1];
      end
      if (TIE_lookup_ram_Out_Req) begin
          if (write) begin
              mem[address] <= data;
              $display("%t writing mem[0x%x] <= 0x%x", $time, address, data);
          end
          else begin
              $display("%t reading mem[0x%x] => 0x%x   Output delay%d", $time, address, mem[address], delay);
          end
      end
  end


endmodule



