// Customer ID=13943; Build=0x75f5e; Copyright (c) 2008-2014 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.




// To enable route ID, uncomment the following and change xxx to the number of bits minus 1
// `define hifi4_Aquila_E2_PROD_loopback_ROUTE_ID



module hifi4_Aquila_E2_PROD_pif_loopback(
  CLK,

  POReqValid_M,
  PIReqRdy_M,
  POReqCntl_M,
  POReqAdrs_M,
  POReqData_M,
  POReqDataBE_M,
  POReqId_M,
  POReqPriority_M,
  POReqAttribute_M,

  PIRespValid_M,
  PORespRdy_M,
  PIRespCntl_M,
  PIRespData_M,
  PIRespId_M,
  PIRespPriority_M,

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  POReqRouteId_M,
  PIRespRouteId_M,
`endif

  POReqValid_S,
  PIReqRdy_S,
  POReqCntl_S,
  POReqAdrs_S,
  POReqData_S,
  POReqDataBE_S,
  POReqId_S,
  POReqPriority_S,
  POReqAttribute_S,

  PIRespValid_S,
  PORespRdy_S,
  PIRespCntl_S,
  PIRespData_S,
  PIRespId_S,
  PIRespPriority_S

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  ,
  POReqRouteId_S,
  PIRespRouteId_S
`endif
);

  input                 CLK;


  // The master side

  input                 POReqValid_M;
  output                PIReqRdy_M;
  input         [  7:0] POReqCntl_M;
  input         [ 31:0] POReqAdrs_M;
  input         [ 63:0] POReqData_M;
  input         [  7:0] POReqDataBE_M;
  input         [  5:0] POReqId_M;
  input         [  1:0] POReqPriority_M;
  input         [ 11:0] POReqAttribute_M;

  output                PIRespValid_M;
  input                 PORespRdy_M;
  output        [  7:0] PIRespCntl_M;
  output        [ 63:0] PIRespData_M;
  output        [  5:0] PIRespId_M;
  output        [  1:0] PIRespPriority_M;

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  // Change xxx to the number of bits minus 1
  input         [xxx:0] POReqRouteId_M;
  output        [xxx:0] PIRespRouteId_M;
`endif


 // The slave side

  output                POReqValid_S;
  input                 PIReqRdy_S;
  output        [  7:0] POReqCntl_S;
  output        [ 31:0] POReqAdrs_S;
  output        [ 63:0] POReqData_S;
  output        [  7:0] POReqDataBE_S;
  output        [  5:0] POReqId_S;
  output        [  1:0] POReqPriority_S;
  output        [ 11:0] POReqAttribute_S;

  input                 PIRespValid_S;
  output                PORespRdy_S;
  input         [  7:0] PIRespCntl_S;
  input         [ 63:0] PIRespData_S;
  input         [  5:0] PIRespId_S;
  input         [  1:0] PIRespPriority_S;

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  // Change xxx to the number of bits minus 1
  output        [xxx:0] POReqRouteId_S;
  input         [xxx:0] PIRespRouteId_S;
`endif



  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
  end


  assign PIReqRdy_M       = PIReqRdy_S;
  assign PIRespValid_M    = PIRespValid_S;
  assign PIRespCntl_M     = PIRespCntl_S;
  assign PIRespData_M     = PIRespData_S;
  assign PIRespId_M       = PIRespId_S;
  assign PIRespPriority_M = PIRespPriority_S;

  assign POReqValid_S     = POReqValid_M;
  assign POReqCntl_S      = POReqCntl_M;
  assign POReqAdrs_S      = POReqAdrs_M;
  assign POReqData_S      = POReqData_M;
  assign POReqDataBE_S    = POReqDataBE_M;
  assign POReqId_S        = POReqId_M;
  assign POReqPriority_S  = POReqPriority_M;
  assign POReqAttribute_S  = POReqAttribute_M;

  assign PORespRdy_S      = PORespRdy_M;

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  assign PIRespRouteId_M  = PIRespRouteId_S;
  assign POReqRouteId_S   = POReqRouteId_M;
`endif


endmodule



