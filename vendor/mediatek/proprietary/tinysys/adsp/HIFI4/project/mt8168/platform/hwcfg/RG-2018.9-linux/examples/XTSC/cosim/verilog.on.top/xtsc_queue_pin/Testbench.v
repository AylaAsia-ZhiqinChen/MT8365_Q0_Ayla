// Customer ID=13943; Build=0x75f5e; Copyright (c) 2008-2010 Tensilica Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// `timescale 1ns / 1ps

module Testbench;   // {

  wire                  CLK;

  wire                  TIE_OUTQ1_PushReq;
  wire          [95:0]  TIE_OUTQ1;
  wire                  TIE_OUTQ1_Full;

  wire                  TIE_INQ1_PopReq;   
  wire          [95:0]  TIE_INQ1;
  wire                  TIE_INQ1_Empty;


  Xtensa0 core0(
    .CLK                (CLK),
    .TIE_OUTQ1_PushReq  (TIE_OUTQ1_PushReq),
    .TIE_OUTQ1          (TIE_OUTQ1),
    .TIE_OUTQ1_Full     (TIE_OUTQ1_Full)
  );

  queue Q1(
    .CLK                (CLK),
    .TIE_OUTQ1_PushReq  (TIE_OUTQ1_PushReq),
    .TIE_OUTQ1          (TIE_OUTQ1),
    .TIE_OUTQ1_Full     (TIE_OUTQ1_Full),
    .TIE_INQ1_PopReq    (TIE_INQ1_PopReq),
    .TIE_INQ1           (TIE_INQ1),
    .TIE_INQ1_Empty     (TIE_INQ1_Empty)
  );

  Xtensa1 core1(
    .TIE_INQ1_PopReq    (TIE_INQ1_PopReq),
    .TIE_INQ1           (TIE_INQ1),
    .TIE_INQ1_Empty     (TIE_INQ1_Empty)
  );

endmodule   // }



