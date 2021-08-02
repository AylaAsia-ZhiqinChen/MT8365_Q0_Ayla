// Customer ID=13943; Build=0x75f5e; Copyright (c) 2008-2010 Tensilica Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

`define DISPLAY_INPUTS

module Passthrough(
  mmio_BInterrupt06,
  EXPSTATE,
  TIE_EXPSTATE,
  BInterrupt06 
);

  input                 mmio_BInterrupt06;
  input  [  31:0]       TIE_EXPSTATE;

  output [  31:0]       EXPSTATE;
  output                BInterrupt06;

  assign EXPSTATE     = TIE_EXPSTATE;
  assign BInterrupt06 = mmio_BInterrupt06;

`ifdef DISPLAY_INPUTS
  always @(mmio_BInterrupt06) $display("%t mmio_BInterrupt06 = 0x%h", $time, mmio_BInterrupt06);
  always @(TIE_EXPSTATE     ) $display("%t TIE_EXPSTATE      = 0x%h", $time, TIE_EXPSTATE     );
`endif

endmodule
