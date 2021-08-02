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

  wire   [  40:0]       TIE_lookup_ram_Out;
  wire                  TIE_lookup_ram_Out_Req;

  wire   [  31:0]       TIE_lookup_ram_In;


  Xtensa0 core0(
    .CLK                    (CLK),
    .TIE_lookup_ram_Out_Req (TIE_lookup_ram_Out_Req),
    .TIE_lookup_ram_Out     (TIE_lookup_ram_Out),
    .TIE_lookup_ram_In      (TIE_lookup_ram_In)
  );


  lookup lram(
    .CLK                    (CLK),
    .TIE_lookup_ram_Out_Req (TIE_lookup_ram_Out_Req),
    .TIE_lookup_ram_Out     (TIE_lookup_ram_Out),
    .TIE_lookup_ram_In      (TIE_lookup_ram_In)
  );


endmodule   // }




