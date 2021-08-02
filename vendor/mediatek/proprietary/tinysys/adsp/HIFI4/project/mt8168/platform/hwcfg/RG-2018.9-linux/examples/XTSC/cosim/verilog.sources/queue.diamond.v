// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// `timescale 1ns / 1ps

`define DISPLAY_IO


module queue(CLK, TIE_OPQ_PushReq, TIE_OPQ, TIE_OPQ_Full, TIE_IPQ_PopReq, TIE_IPQ, TIE_IPQ_Empty);

  input                         CLK;

  input                         TIE_OPQ_PushReq;
  input         [31:0]          TIE_OPQ;
  output                        TIE_OPQ_Full;

  input                         TIE_IPQ_PopReq;   
  output        [31:0]          TIE_IPQ;
  output                        TIE_IPQ_Empty;

  reg                           TIE_OPQ_Full    = 1'b0;
  reg           [1:0]           wp              = 1'b0;
  reg           [1:0]           rp              = 1'b0;

  reg           [31:0]          store[3:0];


  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
  end

  initial begin
    store[0]    <= 0;
  end

  assign TIE_IPQ        = store[rp];
  assign TIE_IPQ_Empty  = (wp == rp) && !TIE_OPQ_Full;

`ifdef DISPLAY_IO
  always @(TIE_IPQ) begin
      $display("%t TIE_IPQ = 0x%h", $time, TIE_IPQ);
  end
`endif

  always @(posedge CLK) begin
    if ((TIE_OPQ_PushReq && !TIE_OPQ_Full) && (TIE_IPQ_PopReq && !TIE_IPQ_Empty)) begin
`ifdef DISPLAY_IO
      $display("%t TIE_OPQ = 0x%h", $time, TIE_OPQ);
`endif
      store[wp]         <= #1 TIE_OPQ;
      wp                <= #1 wp + 1;
      rp                <= #1 rp + 1;
      TIE_OPQ_Full      <= #1 TIE_OPQ_Full;
    end
    else if (TIE_OPQ_PushReq && !TIE_OPQ_Full) begin
`ifdef DISPLAY_IO
      $display("%t TIE_OPQ = 0x%h", $time, TIE_OPQ);
`endif
      store[wp]         <= #1 TIE_OPQ;
      wp                <= #1 wp + 1;
      rp                <= #1 rp;
      TIE_OPQ_Full      <= #1 ((((wp + 1) % 4) == rp) ? 1'b1 : 1'b0);
    end
    else if (TIE_IPQ_PopReq && !TIE_IPQ_Empty) begin
      wp                <= #1 wp;
      rp                <= #1 rp + 1;
      TIE_OPQ_Full      <= #1 1'b0;
    end
    else begin
      wp                <= #1 wp;
      rp                <= #1 rp;
      TIE_OPQ_Full      <= #1 TIE_OPQ_Full;
    end
  end


endmodule

