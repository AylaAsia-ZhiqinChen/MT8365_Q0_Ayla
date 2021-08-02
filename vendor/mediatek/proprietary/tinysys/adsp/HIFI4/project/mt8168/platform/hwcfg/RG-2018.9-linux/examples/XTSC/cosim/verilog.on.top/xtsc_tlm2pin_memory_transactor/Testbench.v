// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.


module Testbench;   // {

  wire                  CLK;

  wire                  POReqValid;
  wire                  PIReqRdy;
  wire          [  7:0] POReqCntl;
  wire          [ 31:0] POReqAdrs;
  wire          [ 11:0] POReqAttribute;
  wire          [ 63:0] POReqData;
  wire          [  7:0] POReqDataBE;
  wire          [  5:0] POReqId;
  wire          [  1:0] POReqPriority;

  wire                  PIRespValid;
  wire                  PORespRdy;
  wire          [  7:0] PIRespCntl;
  wire          [ 63:0] PIRespData;
  wire          [  5:0] PIRespId;
  wire          [  1:0] PIRespPriority;

  wire          [ 11:0] IRam0Addr;
  wire                  IRam0En;
  wire                  IRam0Wr;
  wire          [  3:0] IRam0WordEn;
  wire          [127:0] IRam0WrData;
  wire                  IRam0LoadStore;
  wire                  IRam0Busy;
  wire          [127:0] IRam0Data;

  wire          [ 11:0] IRam1Addr;
  wire                  IRam1En;
  wire                  IRam1Wr;
  wire          [  3:0] IRam1WordEn;
  wire          [127:0] IRam1WrData;
  wire                  IRam1LoadStore;
  wire                  IRam1Busy;
  wire          [127:0] IRam1Data;

  wire          [ 13:0] DRam0AddrB0;
  wire                  DRam0EnB0;
  wire                  DRam0WrB0;
  wire          [  7:0] DRam0ByteEnB0;
  wire          [ 63:0] DRam0WrDataB0;
  wire                  DRam0BusyB0;
  wire          [ 63:0] DRam0DataB0;
  wire          [ 13:0] DRam0AddrB1;
  wire                  DRam0EnB1;
  wire                  DRam0WrB1;
  wire          [  7:0] DRam0ByteEnB1;
  wire          [ 63:0] DRam0WrDataB1;
  wire                  DRam0BusyB1;
  wire          [ 63:0] DRam0DataB1;

  wire          [ 13:0] DRam1AddrB0;
  wire                  DRam1EnB0;
  wire                  DRam1WrB0;
  wire          [  7:0] DRam1ByteEnB0;
  wire          [ 63:0] DRam1WrDataB0;
  wire                  DRam1BusyB0;
  wire          [ 63:0] DRam1DataB0;
  wire          [ 13:0] DRam1AddrB1;
  wire                  DRam1EnB1;
  wire                  DRam1WrB1;
  wire          [  7:0] DRam1ByteEnB1;
  wire          [ 63:0] DRam1WrDataB1;
  wire                  DRam1BusyB1;
  wire          [ 63:0] DRam1DataB1;



  Xtensa0 core0(

    .POReqValid         (POReqValid),
    .PIReqRdy           (PIReqRdy),
    .POReqCntl          (POReqCntl),
    .POReqAdrs          (POReqAdrs),
    .POReqAttribute     (POReqAttribute),
    .POReqData          (POReqData),
    .POReqDataBE        (POReqDataBE),
    .POReqId            (POReqId),
    .POReqPriority      (POReqPriority),

    .PIRespValid        (PIRespValid),
    .PORespRdy          (PORespRdy),
    .PIRespCntl         (PIRespCntl),
    .PIRespData         (PIRespData),
    .PIRespId           (PIRespId),
    .PIRespPriority     (PIRespPriority),

    .IRam0Addr          (IRam0Addr),
    .IRam0En            (IRam0En),
    .IRam0Wr            (IRam0Wr),
    .IRam0WordEn        (IRam0WordEn),
    .IRam0WrData        (IRam0WrData),
    .IRam0LoadStore     (IRam0LoadStore),
    .IRam0Busy          (IRam0Busy),
    .IRam0Data          (IRam0Data),

    .IRam1Addr          (IRam1Addr),
    .IRam1En            (IRam1En),
    .IRam1Wr            (IRam1Wr),
    .IRam1WordEn        (IRam1WordEn),
    .IRam1WrData        (IRam1WrData),
    .IRam1LoadStore     (IRam1LoadStore),
    .IRam1Busy          (IRam1Busy),
    .IRam1Data          (IRam1Data),

    .DRam0AddrB0        (DRam0AddrB0),
    .DRam0EnB0          (DRam0EnB0),
    .DRam0WrB0          (DRam0WrB0),
    .DRam0ByteEnB0      (DRam0ByteEnB0),
    .DRam0WrDataB0      (DRam0WrDataB0),
    .DRam0BusyB0        (DRam0BusyB0),
    .DRam0DataB0        (DRam0DataB0),
    .DRam0AddrB1        (DRam0AddrB1),
    .DRam0EnB1          (DRam0EnB1),
    .DRam0WrB1          (DRam0WrB1),
    .DRam0ByteEnB1      (DRam0ByteEnB1),
    .DRam0WrDataB1      (DRam0WrDataB1),
    .DRam0BusyB1        (DRam0BusyB1),
    .DRam0DataB1        (DRam0DataB1),

    .DRam1AddrB0        (DRam1AddrB0),
    .DRam1EnB0          (DRam1EnB0),
    .DRam1WrB0          (DRam1WrB0),
    .DRam1ByteEnB0      (DRam1ByteEnB0),
    .DRam1WrDataB0      (DRam1WrDataB0),
    .DRam1BusyB0        (DRam1BusyB0),
    .DRam1DataB0        (DRam1DataB0),
    .DRam1AddrB1        (DRam1AddrB1),
    .DRam1EnB1          (DRam1EnB1),
    .DRam1WrB1          (DRam1WrB1),
    .DRam1ByteEnB1      (DRam1ByteEnB1),
    .DRam1WrDataB1      (DRam1WrDataB1),
    .DRam1BusyB1        (DRam1BusyB1),
    .DRam1DataB1        (DRam1DataB1),

    .CLK                (CLK)
  );



  hifi4_Aquila_E2_PROD_pifmem pif(
    .POReqValid         (POReqValid),
    .PIReqRdy           (PIReqRdy),
    .POReqCntl          (POReqCntl),
    .POReqAdrs          (POReqAdrs),
    .POReqAttribute     (POReqAttribute),
    .POReqData          (POReqData),
    .POReqDataBE        (POReqDataBE),
    .POReqId            (POReqId),
    .POReqPriority      (POReqPriority),
    .PIRespValid        (PIRespValid),
    .PORespRdy          (PORespRdy),
    .PIRespCntl         (PIRespCntl),
    .PIRespData         (PIRespData),
    .PIRespId           (PIRespId),
    .PIRespPriority     (PIRespPriority),
    .CLK                (CLK)
  );



  hifi4_Aquila_E2_PROD_iram0 iram0(
    .IRam0Addr          (IRam0Addr),
    .IRam0En            (IRam0En),
    .IRam0Wr            (IRam0Wr),
    .IRam0WordEn        (IRam0WordEn),
    .IRam0WrData        (IRam0WrData),
    .IRam0LoadStore     (IRam0LoadStore),
    .IRam0Busy          (IRam0Busy),
    .IRam0Data          (IRam0Data),
    .CLK                (CLK)
  );



  hifi4_Aquila_E2_PROD_iram1 iram1(
    .IRam1Addr          (IRam1Addr),
    .IRam1En            (IRam1En),
    .IRam1Wr            (IRam1Wr),
    .IRam1WordEn        (IRam1WordEn),
    .IRam1WrData        (IRam1WrData),
    .IRam1LoadStore     (IRam1LoadStore),
    .IRam1Busy          (IRam1Busy),
    .IRam1Data          (IRam1Data),
    .CLK                (CLK)
  );



  hifi4_Aquila_E2_PROD_dram0 dram0(
    .DRam0AddrB0        (DRam0AddrB0),
    .DRam0EnB0          (DRam0EnB0),
    .DRam0WrB0          (DRam0WrB0),
    .DRam0ByteEnB0      (DRam0ByteEnB0),
    .DRam0WrDataB0      (DRam0WrDataB0),
    .DRam0BusyB0        (DRam0BusyB0),
    .DRam0DataB0        (DRam0DataB0),
    .DRam0AddrB1        (DRam0AddrB1),
    .DRam0EnB1          (DRam0EnB1),
    .DRam0WrB1          (DRam0WrB1),
    .DRam0ByteEnB1      (DRam0ByteEnB1),
    .DRam0WrDataB1      (DRam0WrDataB1),
    .DRam0BusyB1        (DRam0BusyB1),
    .DRam0DataB1        (DRam0DataB1),
    .CLK                (CLK)
  );



  hifi4_Aquila_E2_PROD_dram1 dram1(
    .DRam1AddrB0        (DRam1AddrB0),
    .DRam1EnB0          (DRam1EnB0),
    .DRam1WrB0          (DRam1WrB0),
    .DRam1ByteEnB0      (DRam1ByteEnB0),
    .DRam1WrDataB0      (DRam1WrDataB0),
    .DRam1BusyB0        (DRam1BusyB0),
    .DRam1DataB0        (DRam1DataB0),
    .DRam1AddrB1        (DRam1AddrB1),
    .DRam1EnB1          (DRam1EnB1),
    .DRam1WrB1          (DRam1WrB1),
    .DRam1ByteEnB1      (DRam1ByteEnB1),
    .DRam1WrDataB1      (DRam1WrDataB1),
    .DRam1BusyB1        (DRam1BusyB1),
    .DRam1DataB1        (DRam1DataB1),
    .CLK                (CLK)
  );


endmodule   // }


