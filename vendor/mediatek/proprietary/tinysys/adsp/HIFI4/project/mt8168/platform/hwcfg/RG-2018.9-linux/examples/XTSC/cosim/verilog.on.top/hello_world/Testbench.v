// Customer ID=13943; Build=0x75f5e; Copyright (c) 2008-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

module Testbench;

  // Xtensa0 inputs
  reg           BReset = 1'b1;

  // Xtensa0 outputs
  wire          CLK;

  integer       count = 32'h0;

  // Instantiate wrapper Xtensa0
  Xtensa0 core0(
    .BReset (BReset),
    .CLK    (CLK)
  );


  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
    $display("%t BReset=%d", $time, BReset);
  end

  always @(posedge CLK) begin
    count = count + 1;
    if (count == 101) begin
      BReset = 1'b0;
      $display("%t BReset=%d", $time, BReset);
    end
  end

endmodule

