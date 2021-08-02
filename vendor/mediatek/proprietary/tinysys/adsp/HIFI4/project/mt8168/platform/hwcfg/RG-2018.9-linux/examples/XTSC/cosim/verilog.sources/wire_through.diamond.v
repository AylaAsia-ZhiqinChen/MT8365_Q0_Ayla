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


module wire_through(CLK, TIE_EXPSTATE, TIE_IMPWIRE);

  input                         CLK;

  input         [31:0]          TIE_EXPSTATE;

  output        [31:0]          TIE_IMPWIRE;

  assign TIE_IMPWIRE = TIE_EXPSTATE;

  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
  end

`ifdef DISPLAY_IO
  always @(TIE_EXPSTATE) begin
      $display("%t TIE_EXPSTATE  = 0x%h", $time, TIE_EXPSTATE);
  end
`endif

endmodule

