// Customer ID=13943; Build=0x75f5e; Copyright (c) 2008-2010 Tensilica Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

module Testbench;   // {

  wire                  CLK;

  wire                  TIE_OPQ_PushReq;
  wire          [31:0]  TIE_OPQ;
  wire                  TIE_OPQ_Full;

  wire                  TIE_IPQ_PopReq;   
  wire          [31:0]  TIE_IPQ;
  wire                  TIE_IPQ_Empty;


  Xtensa0 core0(
    .CLK                (CLK),
    .TIE_OPQ_PushReq    (TIE_OPQ_PushReq),
    .TIE_OPQ            (TIE_OPQ),
    .TIE_OPQ_Full       (TIE_OPQ_Full)
  );

  queue Q1(
    .CLK                (CLK),
    .TIE_OPQ_PushReq    (TIE_OPQ_PushReq),
    .TIE_OPQ            (TIE_OPQ),
    .TIE_OPQ_Full       (TIE_OPQ_Full),
    .TIE_IPQ_PopReq     (TIE_IPQ_PopReq),
    .TIE_IPQ            (TIE_IPQ),
    .TIE_IPQ_Empty      (TIE_IPQ_Empty)
  );

  Xtensa1 core1(
    .TIE_IPQ_PopReq     (TIE_IPQ_PopReq),
    .TIE_IPQ            (TIE_IPQ),
    .TIE_IPQ_Empty      (TIE_IPQ_Empty)
  );

endmodule   // }




