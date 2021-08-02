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

  wire   [   7:0]       TIE_lut_Out;
  wire                  TIE_lut_Out_Req;

  wire   [  31:0]       TIE_lut_In;
  wire                  TIE_lut_Rdy;


  Xtensa0 core0(
    .CLK                (CLK),
    .TIE_lut_Out_Req    (TIE_lut_Out_Req),
    .TIE_lut_Out        (TIE_lut_Out),
    .TIE_lut_Rdy        (TIE_lut_Rdy),
    .TIE_lut_In         (TIE_lut_In)
  );


  lookup tbl(
    .CLK                (CLK),
    .TIE_lut_Out_Req    (TIE_lut_Out_Req),
    .TIE_lut_Out        (TIE_lut_Out),
    .TIE_lut_Rdy        (TIE_lut_Rdy),
    .TIE_lut_In         (TIE_lut_In)
  );


endmodule   // }



