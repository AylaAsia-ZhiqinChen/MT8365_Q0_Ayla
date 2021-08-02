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



module hifi4_Aquila_E2_PROD_inbound_pif_loopback(
  CLK,

  PIReqValid_M,
  POReqRdy_M,
  PIReqCntl_M,
  PIReqAdrs_M,
  PIReqData_M,
  PIReqDataBE_M,
  PIReqId_M,
  PIReqPriority_M,
  PIReqAttribute_M,

  PORespValid_M,
  PIRespRdy_M,
  PORespCntl_M,
  PORespData_M,
  PORespId_M,
  PORespPriority_M,

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  PIReqRouteId_M,
  PORespRouteId_M,
`endif

  PIReqValid_S,
  POReqRdy_S,
  PIReqCntl_S,
  PIReqAdrs_S,
  PIReqData_S,
  PIReqDataBE_S,
  PIReqId_S,
  PIReqPriority_S,
  PIReqAttribute_S,

  PORespValid_S,
  PIRespRdy_S,
  PORespCntl_S,
  PORespData_S,
  PORespId_S,
  PORespPriority_S

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  ,
  PIReqRouteId_S,
  PORespRouteId_S
`endif
);

  input                 CLK;


  // The master side

  input                 PIReqValid_M;
  output                POReqRdy_M;
  input         [  7:0] PIReqCntl_M;
  input         [ 31:0] PIReqAdrs_M;
  input         [ 63:0] PIReqData_M;
  input         [  7:0] PIReqDataBE_M;
  input         [  5:0] PIReqId_M;
  input         [  1:0] PIReqPriority_M;
  input         [ 11:0] PIReqAttribute_M;

  output                PORespValid_M;
  input                 PIRespRdy_M;
  output        [  7:0] PORespCntl_M;
  output        [ 63:0] PORespData_M;
  output        [  5:0] PORespId_M;
  output        [  1:0] PORespPriority_M;

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  // Change xxx to the number of bits minus 1
  input         [xxx:0] PIReqRouteId_M;
  output        [xxx:0] PORespRouteId_M;
`endif


 // The slave side

  output                PIReqValid_S;
  input                 POReqRdy_S;
  output        [  7:0] PIReqCntl_S;
  output        [ 31:0] PIReqAdrs_S;
  output        [ 63:0] PIReqData_S;
  output        [  7:0] PIReqDataBE_S;
  output        [  5:0] PIReqId_S;
  output        [  1:0] PIReqPriority_S;
  output        [ 11:0] PIReqAttribute_S;

  input                 PORespValid_S;
  output                PIRespRdy_S;
  input         [  7:0] PORespCntl_S;
  input         [ 63:0] PORespData_S;
  input         [  5:0] PORespId_S;
  input         [  1:0] PORespPriority_S;

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  // Change xxx to the number of bits minus 1
  output        [xxx:0] PIReqRouteId_S;
  input         [xxx:0] PORespRouteId_S;
`endif



  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
  end


  assign POReqRdy_M       = POReqRdy_S;
  assign PORespValid_M    = PORespValid_S;
  assign PORespCntl_M     = PORespCntl_S;
  assign PORespData_M     = PORespData_S;
  assign PORespId_M       = PORespId_S;
  assign PORespPriority_M = PORespPriority_S;

  assign PIReqValid_S     = PIReqValid_M;
  assign PIReqCntl_S      = PIReqCntl_M;
  assign PIReqAdrs_S      = PIReqAdrs_M;
  assign PIReqData_S      = PIReqData_M;
  assign PIReqDataBE_S    = PIReqDataBE_M;
  assign PIReqId_S        = PIReqId_M;
  assign PIReqPriority_S  = PIReqPriority_M;
  assign PIReqAttribute_S  = PIReqAttribute_M;

  assign PIRespRdy_S      = PIRespRdy_M;

`ifdef hifi4_Aquila_E2_PROD_loopback_ROUTE_ID
  assign PORespRouteId_M  = PORespRouteId_S;
  assign PIReqRouteId_S   = PIReqRouteId_M;
`endif


endmodule



