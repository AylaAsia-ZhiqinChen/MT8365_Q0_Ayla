// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2014 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.


module Testbench;   // {

  wire                  CLK;

  wire                  PIReqValid;
  wire                  POReqRdy;
  wire          [  7:0] PIReqCntl;
  wire          [ 31:0] PIReqAdrs;
  wire          [ 11:0] PIReqAttribute;
  wire          [ 63:0] PIReqData;
  wire          [  7:0] PIReqDataBE;
  wire          [  5:0] PIReqId;
  wire          [  1:0] PIReqPriority;

  wire                  PORespValid;
  wire                  PIRespRdy;
  wire          [  7:0] PORespCntl;
  wire          [ 63:0] PORespData;
  wire          [  5:0] PORespId;
  wire          [  1:0] PORespPriority;



  Xtensa0 core0(

    .PIReqValid         (PIReqValid),
    .POReqRdy           (POReqRdy),
    .PIReqCntl          (PIReqCntl),
    .PIReqAdrs          (PIReqAdrs),
    .PIReqAttribute     (PIReqAttribute),
    .PIReqData          (PIReqData),
    .PIReqDataBE        (PIReqDataBE),
    .PIReqId            (PIReqId),
    .PIReqPriority      (PIReqPriority),

    .PORespValid        (PORespValid),
    .PIRespRdy          (PIRespRdy),
    .PORespCntl         (PORespCntl),
    .PORespData         (PORespData),
    .PORespId           (PORespId),
    .PORespPriority     (PORespPriority),

    .CLK                (CLK)
  );



  Master master(

    .PIReqValid         (PIReqValid),
    .POReqRdy           (POReqRdy),
    .PIReqCntl          (PIReqCntl),
    .PIReqAdrs          (PIReqAdrs),
    .PIReqAttribute     (PIReqAttribute),
    .PIReqData          (PIReqData),
    .PIReqDataBE        (PIReqDataBE),
    .PIReqId            (PIReqId),
    .PIReqPriority      (PIReqPriority),

    .PORespValid        (PORespValid),
    .PIRespRdy          (PIRespRdy),
    .PORespCntl         (PORespCntl),
    .PORespData         (PORespData),
    .PORespId           (PORespId),
    .PORespPriority     (PORespPriority)

  );


endmodule   // }


