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


module queue(CLK, TIE_OUTQ1_PushReq, TIE_OUTQ1, TIE_OUTQ1_Full, TIE_INQ1_PopReq, TIE_INQ1, TIE_INQ1_Empty);

  input                         CLK;

  input                         TIE_OUTQ1_PushReq;
  input         [95:0]          TIE_OUTQ1;
  output                        TIE_OUTQ1_Full;

  input                         TIE_INQ1_PopReq;   
  output        [95:0]          TIE_INQ1;
  output                        TIE_INQ1_Empty;

  reg                           TIE_OUTQ1_Full  = 1'b0;
  reg           [1:0]           wp              = 1'b0;
  reg           [1:0]           rp              = 1'b0;

  reg           [95:0]          store[3:0];


  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
  end

  initial begin
    store[0]    <= 0;
  end

  assign TIE_INQ1       = store[rp];
  assign TIE_INQ1_Empty = (wp == rp) && !TIE_OUTQ1_Full;

`ifdef DISPLAY_IO
  always @(TIE_INQ1) begin
      $display("%t TIE_INQ1  = 0x%h", $time, TIE_INQ1);
  end
`endif

  always @(posedge CLK) begin
    if ((TIE_OUTQ1_PushReq && !TIE_OUTQ1_Full) && (TIE_INQ1_PopReq && !TIE_INQ1_Empty)) begin
`ifdef DISPLAY_IO
      $display("%t TIE_OUTQ1 = 0x%h", $time, TIE_OUTQ1);
`endif
      store[wp]         <= #1 TIE_OUTQ1;
      wp                <= #1 wp + 1;
      rp                <= #1 rp + 1;
      TIE_OUTQ1_Full    <= #1 TIE_OUTQ1_Full;
    end
    else if (TIE_OUTQ1_PushReq && !TIE_OUTQ1_Full) begin
`ifdef DISPLAY_IO
      $display("%t TIE_OUTQ1 = 0x%h", $time, TIE_OUTQ1);
`endif
      store[wp]         <= #1 TIE_OUTQ1;
      wp                <= #1 wp + 1;
      rp                <= #1 rp;
      TIE_OUTQ1_Full    <= #1 ((((wp + 1) % 4) == rp) ? 1'b1 : 1'b0);
    end
    else if (TIE_INQ1_PopReq && !TIE_INQ1_Empty) begin
      wp                <= #1 wp;
      rp                <= #1 rp + 1;
      TIE_OUTQ1_Full    <= #1 1'b0;
    end
    else begin
      wp                <= #1 wp;
      rp                <= #1 rp;
      TIE_OUTQ1_Full    <= #1 TIE_OUTQ1_Full;
    end
  end


endmodule

