// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.


module Subsystem;   // {

  reg           [  31:0] AltResetVec                   = 32'h0;

  reg           [  63:0] DCacheADataB0                 = 64'h0;
  reg           [  63:0] DCacheADataB1                 = 64'h0;
  wire                   DCacheAEnB0;
  wire                   DCacheAEnB1;
  wire                   DCacheAWrB0;
  wire                   DCacheAWrB1;
  wire          [   8:0] DCacheAddrB0;
  wire          [   8:0] DCacheAddrB1;
  reg           [  63:0] DCacheBDataB0                 = 64'h0;
  reg           [  63:0] DCacheBDataB1                 = 64'h0;
  wire                   DCacheBEnB0;
  wire                   DCacheBEnB1;
  wire                   DCacheBWrB0;
  wire                   DCacheBWrB1;
  wire          [   7:0] DCacheByteEnB0;
  wire          [   7:0] DCacheByteEnB1;
  reg           [  63:0] DCacheCDataB0                 = 64'h0;
  reg           [  63:0] DCacheCDataB1                 = 64'h0;
  wire                   DCacheCEnB0;
  wire                   DCacheCEnB1;
  wire                   DCacheCWrB0;
  wire                   DCacheCWrB1;
  reg           [  63:0] DCacheDDataB0                 = 64'h0;
  reg           [  63:0] DCacheDDataB1                 = 64'h0;
  wire                   DCacheDEnB0;
  wire                   DCacheDEnB1;
  wire                   DCacheDWrB0;
  wire                   DCacheDWrB1;
  wire          [  63:0] DCacheWrDataB0;
  wire          [  63:0] DCacheWrDataB1;

  reg           [  22:0] DTagAData0                    = 23'h0;
  wire                   DTagAEn0;
  wire                   DTagAWr0;
  wire          [   5:0] DTagAddr0;
  reg           [  22:0] DTagBData0                    = 23'h0;
  wire                   DTagBEn0;
  wire                   DTagBWr0;
  reg           [  22:0] DTagCData0                    = 23'h0;
  wire                   DTagCEn0;
  wire                   DTagCWr0;
  reg           [  22:0] DTagDData0                    = 23'h0;
  wire                   DTagDEn0;
  wire                   DTagDWr0;
  wire          [  22:0] DTagWrData0;

  reg           [  22:0] DTagAData1                    = 23'h0;
  wire                   DTagAEn1;
  wire                   DTagAWr1;
  wire          [   5:0] DTagAddr1;
  reg           [  22:0] DTagBData1                    = 23'h0;
  wire                   DTagBEn1;
  wire                   DTagBWr1;
  reg           [  22:0] DTagCData1                    = 23'h0;
  wire                   DTagCEn1;
  wire                   DTagCWr1;
  reg           [  22:0] DTagDData1                    = 23'h0;
  wire                   DTagDEn1;
  wire                   DTagDWr1;
  wire          [  22:0] DTagWrData1;

  reg           [  31:0] TIE_ERI_RD_In                 = 32'h0;
  wire          [  13:0] TIE_ERI_RD_Out;
  wire                   TIE_ERI_RD_Out_Req;
  reg                    TIE_ERI_RD_Rdy                = 1'h1;

  reg                    TIE_ERI_WR_In                 = 1'h0;
  wire          [  45:0] TIE_ERI_WR_Out;
  wire                   TIE_ERI_WR_Out_Req;

  wire          [  31:0] TIE_EXPSTATE;

  wire          [   4:0] EncodedExceptionVector_W;

  reg           [ 127:0] ICacheAData                   = 128'h0;
  wire                   ICacheAEn;
  wire          [   3:0] ICacheAWordEn;
  wire                   ICacheAWr;
  wire          [   8:0] ICacheAddr;
  reg           [ 127:0] ICacheBData                   = 128'h0;
  wire                   ICacheBEn;
  wire          [   3:0] ICacheBWordEn;
  wire                   ICacheBWr;
  reg           [ 127:0] ICacheCData                   = 128'h0;
  wire                   ICacheCEn;
  wire          [   3:0] ICacheCWordEn;
  wire                   ICacheCWr;
  reg           [ 127:0] ICacheDData                   = 128'h0;
  wire                   ICacheDEn;
  wire          [   3:0] ICacheDWordEn;
  wire                   ICacheDWr;
  wire          [ 127:0] ICacheWrData;
  reg           [  21:0] ITagAData                     = 22'h0;
  wire                   ITagAEn;
  wire                   ITagAWr;
  wire          [   5:0] ITagAddr;
  reg           [  21:0] ITagBData                     = 22'h0;
  wire                   ITagBEn;
  wire                   ITagBWr;
  reg           [  21:0] ITagCData                     = 22'h0;
  wire                   ITagCEn;
  wire                   ITagCWr;
  reg           [  21:0] ITagDData                     = 22'h0;
  wire                   ITagDEn;
  wire                   ITagDWr;
  wire          [  21:0] ITagWrData;

  reg           [  31:0] TIE_IMPWIRE                   = 32'h0;

  reg           [  17:0] BInterrupt                    = 18'h0;
  wire                   PWaitMode;

  reg           [  31:0] TIE_MFP1_In                   = 32'h0;
  wire          [  34:0] TIE_MFP1_Out;
  wire                   TIE_MFP1_Out_Req;

  reg           [  31:0] TIE_MFP2_In                   = 32'h0;
  wire          [  38:0] TIE_MFP2_Out;
  wire                   TIE_MFP2_Out_Req;

  reg           [  87:0] DebugInstr                    = 88'h0;
  reg                    DebugMode                     = 1'h0;
  reg                    EnableRunStall                = 1'h0;
  reg                    ExecDebugInstr                = 1'h0;
  wire                   GlobalStall;
  wire                   InOCDMode_ps_C4;
  wire                   Interrupt4_1hot_EXC;
  reg                    InterruptAllConds             = 1'h0;
  reg                    OCDDbgInt                     = 1'h0;
  reg                    OCDEnabled_W                  = 1'h0;
  reg                    OCDOverride                   = 1'h0;
  reg           [  31:0] OCDRSRBus_M                   = 32'h0;
  wire          [   3:0] PSCurIntLvl_W;
  wire                   RFDD_W;
  wire                   RFDO_W;
  wire                   RealExcept_W;
  wire                   ReceivedRunStall;
  wire          [   7:0] SRAddr_W;
  wire                   SRRead_W;
  wire                   SRWrite_W;
  wire                   Valid_W;
  wire          [  31:0] WSRBus_W;

  reg           [  15:0] AXIExclID                     = 16'h0;
  reg           [  31:0] PIReqAdrs                     = 32'h0;
  reg           [  11:0] PIReqAttribute                = 12'h0;
  reg           [   7:0] PIReqCntl                     = 8'h0;
  reg           [  63:0] PIReqData                     = 64'h0;
  reg           [   7:0] PIReqDataBE                   = 8'h0;
  reg           [   5:0] PIReqId                       = 6'h0;
  reg           [   1:0] PIReqPriority                 = 2'h0;
  reg                    PIReqValid                    = 1'h0;
  reg                    PIRespRdy                     = 1'h0;
  wire                   POReqRdy;
  wire          [   7:0] PORespCntl;
  wire          [  63:0] PORespData;
  wire          [   5:0] PORespId;
  wire          [   1:0] PORespPriority;
  wire                   PORespValid;

  reg           [  15:0] PRID                          = 16'h0;

  reg                    PerfMonInt                    = 1'h0;

  wire          [   6:0] PrefetchRamAddr;
  reg           [ 127:0] PrefetchRamData               = 128'h0;
  wire                   PrefetchRamEn;
  wire          [   1:0] PrefetchRamPIFWEn;
  wire                   PrefetchRamWr;
  wire          [ 127:0] PrefetchRamWrData;

  reg                    RunStall                      = 1'h0;

  reg                    TMode                         = 1'h0;
  reg                    TModeClkGateOverride          = 1'h0;

  wire          [  31:0] PDebugData;
  reg                    PDebugEnable                  = 1'h0;
  wire          [   7:0] PDebugInbPif;
  wire          [  31:0] PDebugInst;
  wire          [  31:0] PDebugLS0Addr;
  wire          [  31:0] PDebugLS0Data;
  wire          [  31:0] PDebugLS0Stat;
  wire          [  31:0] PDebugLS1Addr;
  wire          [  31:0] PDebugLS1Data;
  wire          [  31:0] PDebugLS1Stat;
  wire          [   7:0] PDebugOutPif;
  wire          [  31:0] PDebugPC;
  wire          [   3:0] PDebugPrefetchL1Fill;
  wire          [   7:0] PDebugPrefetchLookup;
  wire          [   7:0] PDebugStatus;

  reg                    StatVectorSel                 = 1'h0;

  reg           [   7:0] TIE_mfp1status                = 8'h0;

  reg           [   7:0] TIE_mfp2status                = 8'h0;

  wire          [   4:0] TIE_qi;

  wire          [   4:0] TIE_qo;

  wire          [   4:0] TIE_qo_trig;

  wire                   POReqValid;
  wire                   PIReqRdy;
  wire          [   7:0] POReqCntl;
  wire          [  31:0] POReqAdrs;
  wire          [  11:0] POReqAttribute;
  wire          [  63:0] POReqData;
  wire          [   7:0] POReqDataBE;
  wire          [   5:0] POReqId;
  wire          [   1:0] POReqPriority;

  wire                   PIRespValid;
  wire                   PORespRdy;
  wire          [   7:0] PIRespCntl;
  wire          [  63:0] PIRespData;
  wire          [   5:0] PIRespId;
  wire          [   1:0] PIRespPriority;

  wire          [  11:0] IRam0Addr;
  wire                   IRam0En;
  wire                   IRam0Wr;
  wire          [   3:0] IRam0WordEn;
  wire          [ 127:0] IRam0WrData;
  wire                   IRam0LoadStore;
  wire                   IRam0Busy;
  wire          [ 127:0] IRam0Data;

  wire          [  11:0] IRam1Addr;
  wire                   IRam1En;
  wire                   IRam1Wr;
  wire          [   3:0] IRam1WordEn;
  wire          [ 127:0] IRam1WrData;
  wire                   IRam1LoadStore;
  wire                   IRam1Busy;
  wire          [ 127:0] IRam1Data;

  wire          [  13:0] DRam0AddrB0;
  wire                   DRam0EnB0;
  wire                   DRam0WrB0;
  wire          [   7:0] DRam0ByteEnB0;
  wire          [  63:0] DRam0WrDataB0;
  wire                   DRam0BusyB0;
  wire          [  63:0] DRam0DataB0;
  wire          [  13:0] DRam0AddrB1;
  wire                   DRam0EnB1;
  wire                   DRam0WrB1;
  wire          [   7:0] DRam0ByteEnB1;
  wire          [  63:0] DRam0WrDataB1;
  wire                   DRam0BusyB1;
  wire          [  63:0] DRam0DataB1;

  wire          [  13:0] DRam1AddrB0;
  wire                   DRam1EnB0;
  wire                   DRam1WrB0;
  wire          [   7:0] DRam1ByteEnB0;
  wire          [  63:0] DRam1WrDataB0;
  wire                   DRam1BusyB0;
  wire          [  63:0] DRam1DataB0;
  wire          [  13:0] DRam1AddrB1;
  wire                   DRam1EnB1;
  wire                   DRam1WrB1;
  wire          [   7:0] DRam1ByteEnB1;
  wire          [  63:0] DRam1WrDataB1;
  wire                   DRam1BusyB1;
  wire          [  63:0] DRam1DataB1;

  reg                    CLK                           = 1'h1;
  reg                    BReset                        = 1'h1;


  Xm_hifi4_Aquila_E2_PRODXtensa core0(

    .AltResetVec                  (AltResetVec),
    .DCacheADataB0                (DCacheADataB0),
    .DCacheADataB1                (DCacheADataB1),
    .DCacheAEnB0                  (DCacheAEnB0),
    .DCacheAEnB1                  (DCacheAEnB1),
    .DCacheAWrB0                  (DCacheAWrB0),
    .DCacheAWrB1                  (DCacheAWrB1),
    .DCacheAddrB0                 (DCacheAddrB0),
    .DCacheAddrB1                 (DCacheAddrB1),
    .DCacheBDataB0                (DCacheBDataB0),
    .DCacheBDataB1                (DCacheBDataB1),
    .DCacheBEnB0                  (DCacheBEnB0),
    .DCacheBEnB1                  (DCacheBEnB1),
    .DCacheBWrB0                  (DCacheBWrB0),
    .DCacheBWrB1                  (DCacheBWrB1),
    .DCacheByteEnB0               (DCacheByteEnB0),
    .DCacheByteEnB1               (DCacheByteEnB1),
    .DCacheCDataB0                (DCacheCDataB0),
    .DCacheCDataB1                (DCacheCDataB1),
    .DCacheCEnB0                  (DCacheCEnB0),
    .DCacheCEnB1                  (DCacheCEnB1),
    .DCacheCWrB0                  (DCacheCWrB0),
    .DCacheCWrB1                  (DCacheCWrB1),
    .DCacheDDataB0                (DCacheDDataB0),
    .DCacheDDataB1                (DCacheDDataB1),
    .DCacheDEnB0                  (DCacheDEnB0),
    .DCacheDEnB1                  (DCacheDEnB1),
    .DCacheDWrB0                  (DCacheDWrB0),
    .DCacheDWrB1                  (DCacheDWrB1),
    .DCacheWrDataB0               (DCacheWrDataB0),
    .DCacheWrDataB1               (DCacheWrDataB1),
    .DTagAData0                   (DTagAData0),
    .DTagAEn0                     (DTagAEn0),
    .DTagAWr0                     (DTagAWr0),
    .DTagAddr0                    (DTagAddr0),
    .DTagBData0                   (DTagBData0),
    .DTagBEn0                     (DTagBEn0),
    .DTagBWr0                     (DTagBWr0),
    .DTagCData0                   (DTagCData0),
    .DTagCEn0                     (DTagCEn0),
    .DTagCWr0                     (DTagCWr0),
    .DTagDData0                   (DTagDData0),
    .DTagDEn0                     (DTagDEn0),
    .DTagDWr0                     (DTagDWr0),
    .DTagWrData0                  (DTagWrData0),
    .DTagAData1                   (DTagAData1),
    .DTagAEn1                     (DTagAEn1),
    .DTagAWr1                     (DTagAWr1),
    .DTagAddr1                    (DTagAddr1),
    .DTagBData1                   (DTagBData1),
    .DTagBEn1                     (DTagBEn1),
    .DTagBWr1                     (DTagBWr1),
    .DTagCData1                   (DTagCData1),
    .DTagCEn1                     (DTagCEn1),
    .DTagCWr1                     (DTagCWr1),
    .DTagDData1                   (DTagDData1),
    .DTagDEn1                     (DTagDEn1),
    .DTagDWr1                     (DTagDWr1),
    .DTagWrData1                  (DTagWrData1),
    .TIE_ERI_RD_In                (TIE_ERI_RD_In),
    .TIE_ERI_RD_Out               (TIE_ERI_RD_Out),
    .TIE_ERI_RD_Out_Req           (TIE_ERI_RD_Out_Req),
    .TIE_ERI_RD_Rdy               (TIE_ERI_RD_Rdy),
    .TIE_ERI_WR_In                (TIE_ERI_WR_In),
    .TIE_ERI_WR_Out               (TIE_ERI_WR_Out),
    .TIE_ERI_WR_Out_Req           (TIE_ERI_WR_Out_Req),
    .TIE_EXPSTATE                 (TIE_EXPSTATE),
    .EncodedExceptionVector_W     (EncodedExceptionVector_W),
    .ICacheAData                  (ICacheAData),
    .ICacheAEn                    (ICacheAEn),
    .ICacheAWordEn                (ICacheAWordEn),
    .ICacheAWr                    (ICacheAWr),
    .ICacheAddr                   (ICacheAddr),
    .ICacheBData                  (ICacheBData),
    .ICacheBEn                    (ICacheBEn),
    .ICacheBWordEn                (ICacheBWordEn),
    .ICacheBWr                    (ICacheBWr),
    .ICacheCData                  (ICacheCData),
    .ICacheCEn                    (ICacheCEn),
    .ICacheCWordEn                (ICacheCWordEn),
    .ICacheCWr                    (ICacheCWr),
    .ICacheDData                  (ICacheDData),
    .ICacheDEn                    (ICacheDEn),
    .ICacheDWordEn                (ICacheDWordEn),
    .ICacheDWr                    (ICacheDWr),
    .ICacheWrData                 (ICacheWrData),
    .ITagAData                    (ITagAData),
    .ITagAEn                      (ITagAEn),
    .ITagAWr                      (ITagAWr),
    .ITagAddr                     (ITagAddr),
    .ITagBData                    (ITagBData),
    .ITagBEn                      (ITagBEn),
    .ITagBWr                      (ITagBWr),
    .ITagCData                    (ITagCData),
    .ITagCEn                      (ITagCEn),
    .ITagCWr                      (ITagCWr),
    .ITagDData                    (ITagDData),
    .ITagDEn                      (ITagDEn),
    .ITagDWr                      (ITagDWr),
    .ITagWrData                   (ITagWrData),
    .TIE_IMPWIRE                  (TIE_IMPWIRE),
    .BInterrupt                   (BInterrupt),
    .PWaitMode                    (PWaitMode),
    .TIE_MFP1_In                  (TIE_MFP1_In),
    .TIE_MFP1_Out                 (TIE_MFP1_Out),
    .TIE_MFP1_Out_Req             (TIE_MFP1_Out_Req),
    .TIE_MFP2_In                  (TIE_MFP2_In),
    .TIE_MFP2_Out                 (TIE_MFP2_Out),
    .TIE_MFP2_Out_Req             (TIE_MFP2_Out_Req),
    .DebugInstr                   (DebugInstr),
    .DebugMode                    (DebugMode),
    .EnableRunStall               (EnableRunStall),
    .ExecDebugInstr               (ExecDebugInstr),
    .GlobalStall                  (GlobalStall),
    .InOCDMode_ps_C4              (InOCDMode_ps_C4),
    .Interrupt4_1hot_EXC          (Interrupt4_1hot_EXC),
    .InterruptAllConds            (InterruptAllConds),
    .OCDDbgInt                    (OCDDbgInt),
    .OCDEnabled_W                 (OCDEnabled_W),
    .OCDOverride                  (OCDOverride),
    .OCDRSRBus_M                  (OCDRSRBus_M),
    .PSCurIntLvl_W                (PSCurIntLvl_W),
    .RFDD_W                       (RFDD_W),
    .RFDO_W                       (RFDO_W),
    .RealExcept_W                 (RealExcept_W),
    .ReceivedRunStall             (ReceivedRunStall),
    .SRAddr_W                     (SRAddr_W),
    .SRRead_W                     (SRRead_W),
    .SRWrite_W                    (SRWrite_W),
    .Valid_W                      (Valid_W),
    .WSRBus_W                     (WSRBus_W),
    .AXIExclID                    (AXIExclID),
    .PIReqAdrs                    (PIReqAdrs),
    .PIReqAttribute               (PIReqAttribute),
    .PIReqCntl                    (PIReqCntl),
    .PIReqData                    (PIReqData),
    .PIReqDataBE                  (PIReqDataBE),
    .PIReqId                      (PIReqId),
    .PIReqPriority                (PIReqPriority),
    .PIReqValid                   (PIReqValid),
    .PIRespRdy                    (PIRespRdy),
    .POReqRdy                     (POReqRdy),
    .PORespCntl                   (PORespCntl),
    .PORespData                   (PORespData),
    .PORespId                     (PORespId),
    .PORespPriority               (PORespPriority),
    .PORespValid                  (PORespValid),
    .PRID                         (PRID),
    .PerfMonInt                   (PerfMonInt),
    .PrefetchRamAddr              (PrefetchRamAddr),
    .PrefetchRamData              (PrefetchRamData),
    .PrefetchRamEn                (PrefetchRamEn),
    .PrefetchRamPIFWEn            (PrefetchRamPIFWEn),
    .PrefetchRamWr                (PrefetchRamWr),
    .PrefetchRamWrData            (PrefetchRamWrData),
    .RunStall                     (RunStall),
    .TMode                        (TMode),
    .TModeClkGateOverride         (TModeClkGateOverride),
    .PDebugData                   (PDebugData),
    .PDebugEnable                 (PDebugEnable),
    .PDebugInbPif                 (PDebugInbPif),
    .PDebugInst                   (PDebugInst),
    .PDebugLS0Addr                (PDebugLS0Addr),
    .PDebugLS0Data                (PDebugLS0Data),
    .PDebugLS0Stat                (PDebugLS0Stat),
    .PDebugLS1Addr                (PDebugLS1Addr),
    .PDebugLS1Data                (PDebugLS1Data),
    .PDebugLS1Stat                (PDebugLS1Stat),
    .PDebugOutPif                 (PDebugOutPif),
    .PDebugPC                     (PDebugPC),
    .PDebugPrefetchL1Fill         (PDebugPrefetchL1Fill),
    .PDebugPrefetchLookup         (PDebugPrefetchLookup),
    .PDebugStatus                 (PDebugStatus),
    .StatVectorSel                (StatVectorSel),
    .TIE_mfp1status               (TIE_mfp1status),
    .TIE_mfp2status               (TIE_mfp2status),
    .TIE_qi                       (TIE_qi),
    .TIE_qo                       (TIE_qo),
    .TIE_qo_trig                  (TIE_qo_trig),

    .POReqValid                   (POReqValid),
    .PIReqRdy                     (PIReqRdy),
    .POReqCntl                    (POReqCntl),
    .POReqAdrs                    (POReqAdrs),
    .POReqAttribute               (POReqAttribute),
    .POReqData                    (POReqData),
    .POReqDataBE                  (POReqDataBE),
    .POReqId                      (POReqId),
    .POReqPriority                (POReqPriority),

    .PIRespValid                  (PIRespValid),
    .PORespRdy                    (PORespRdy),
    .PIRespCntl                   (PIRespCntl),
    .PIRespData                   (PIRespData),
    .PIRespId                     (PIRespId),
    .PIRespPriority               (PIRespPriority),

    .IRam0Addr                    (IRam0Addr),
    .IRam0En                      (IRam0En),
    .IRam0Wr                      (IRam0Wr),
    .IRam0WordEn                  (IRam0WordEn),
    .IRam0WrData                  (IRam0WrData),
    .IRam0LoadStore               (IRam0LoadStore),
    .IRam0Busy                    (IRam0Busy),
    .IRam0Data                    (IRam0Data),

    .IRam1Addr                    (IRam1Addr),
    .IRam1En                      (IRam1En),
    .IRam1Wr                      (IRam1Wr),
    .IRam1WordEn                  (IRam1WordEn),
    .IRam1WrData                  (IRam1WrData),
    .IRam1LoadStore               (IRam1LoadStore),
    .IRam1Busy                    (IRam1Busy),
    .IRam1Data                    (IRam1Data),

    .DRam0AddrB0                  (DRam0AddrB0),
    .DRam0EnB0                    (DRam0EnB0),
    .DRam0WrB0                    (DRam0WrB0),
    .DRam0ByteEnB0                (DRam0ByteEnB0),
    .DRam0WrDataB0                (DRam0WrDataB0),
    .DRam0BusyB0                  (DRam0BusyB0),
    .DRam0DataB0                  (DRam0DataB0),
    .DRam0AddrB1                  (DRam0AddrB1),
    .DRam0EnB1                    (DRam0EnB1),
    .DRam0WrB1                    (DRam0WrB1),
    .DRam0ByteEnB1                (DRam0ByteEnB1),
    .DRam0WrDataB1                (DRam0WrDataB1),
    .DRam0BusyB1                  (DRam0BusyB1),
    .DRam0DataB1                  (DRam0DataB1),

    .DRam1AddrB0                  (DRam1AddrB0),
    .DRam1EnB0                    (DRam1EnB0),
    .DRam1WrB0                    (DRam1WrB0),
    .DRam1ByteEnB0                (DRam1ByteEnB0),
    .DRam1WrDataB0                (DRam1WrDataB0),
    .DRam1BusyB0                  (DRam1BusyB0),
    .DRam1DataB0                  (DRam1DataB0),
    .DRam1AddrB1                  (DRam1AddrB1),
    .DRam1EnB1                    (DRam1EnB1),
    .DRam1WrB1                    (DRam1WrB1),
    .DRam1ByteEnB1                (DRam1ByteEnB1),
    .DRam1WrDataB1                (DRam1WrDataB1),
    .DRam1BusyB1                  (DRam1BusyB1),
    .DRam1DataB1                  (DRam1DataB1),

    .CLK                          (CLK),
    .BReset                       (BReset)
  );



  hifi4_Aquila_E2_PROD_pifmem pif(
    .POReqValid                   (POReqValid),
    .PIReqRdy                     (PIReqRdy),
    .POReqCntl                    (POReqCntl),
    .POReqAdrs                    (POReqAdrs),
    .POReqAttribute               (POReqAttribute),
    .POReqData                    (POReqData),
    .POReqDataBE                  (POReqDataBE),
    .POReqId                      (POReqId),
    .POReqPriority                (POReqPriority),
    .PIRespValid                  (PIRespValid),
    .PORespRdy                    (PORespRdy),
    .PIRespCntl                   (PIRespCntl),
    .PIRespData                   (PIRespData),
    .PIRespId                     (PIRespId),
    .PIRespPriority               (PIRespPriority),
    .CLK                          (CLK)
  );



  hifi4_Aquila_E2_PROD_iram0 iram0(
    .IRam0Addr                    (IRam0Addr),
    .IRam0En                      (IRam0En),
    .IRam0Wr                      (IRam0Wr),
    .IRam0WordEn                  (IRam0WordEn),
    .IRam0WrData                  (IRam0WrData),
    .IRam0LoadStore               (IRam0LoadStore),
    .IRam0Busy                    (IRam0Busy),
    .IRam0Data                    (IRam0Data),
    .CLK                          (CLK)
  );



  hifi4_Aquila_E2_PROD_iram1 iram1(
    .IRam1Addr                    (IRam1Addr),
    .IRam1En                      (IRam1En),
    .IRam1Wr                      (IRam1Wr),
    .IRam1WordEn                  (IRam1WordEn),
    .IRam1WrData                  (IRam1WrData),
    .IRam1LoadStore               (IRam1LoadStore),
    .IRam1Busy                    (IRam1Busy),
    .IRam1Data                    (IRam1Data),
    .CLK                          (CLK)
  );



  hifi4_Aquila_E2_PROD_dram0 dram0(
    .DRam0AddrB0                  (DRam0AddrB0),
    .DRam0EnB0                    (DRam0EnB0),
    .DRam0WrB0                    (DRam0WrB0),
    .DRam0ByteEnB0                (DRam0ByteEnB0),
    .DRam0WrDataB0                (DRam0WrDataB0),
    .DRam0BusyB0                  (DRam0BusyB0),
    .DRam0DataB0                  (DRam0DataB0),
    .DRam0AddrB1                  (DRam0AddrB1),
    .DRam0EnB1                    (DRam0EnB1),
    .DRam0WrB1                    (DRam0WrB1),
    .DRam0ByteEnB1                (DRam0ByteEnB1),
    .DRam0WrDataB1                (DRam0WrDataB1),
    .DRam0BusyB1                  (DRam0BusyB1),
    .DRam0DataB1                  (DRam0DataB1),
    .CLK                          (CLK)
  );



  hifi4_Aquila_E2_PROD_dram1 dram1(
    .DRam1AddrB0                  (DRam1AddrB0),
    .DRam1EnB0                    (DRam1EnB0),
    .DRam1WrB0                    (DRam1WrB0),
    .DRam1ByteEnB0                (DRam1ByteEnB0),
    .DRam1WrDataB0                (DRam1WrDataB0),
    .DRam1BusyB0                  (DRam1BusyB0),
    .DRam1DataB0                  (DRam1DataB0),
    .DRam1AddrB1                  (DRam1AddrB1),
    .DRam1EnB1                    (DRam1EnB1),
    .DRam1WrB1                    (DRam1WrB1),
    .DRam1ByteEnB1                (DRam1ByteEnB1),
    .DRam1WrDataB1                (DRam1WrDataB1),
    .DRam1BusyB1                  (DRam1BusyB1),
    .DRam1DataB1                  (DRam1DataB1),
    .CLK                          (CLK)
  );


  always  #500   CLK = ~CLK;
  initial #10000 BReset = 1'b0;

  initial begin
    if($test$plusargs("dumpvars")) begin
      $dumpvars();
    end
  end

endmodule   // }



module Testbench; 

  Subsystem xtensa0();

  /*
     If desired, you can instantiate more instances of the entire Xtensa sub-system. 
     For example, by uncommenting the "Subsystem xtensa1() line below.

     Other things to do when instantiating the Subsystem multiple times:

     1) You should set the full_hierarchical_names xtsc_initialize_parms parameter to true
        so that parameters of modules in the second and subsequent Subsystem instances can
        be overridden.  This can be done at run time as shown in the examples in (2) below
        or it can be done at code-generation time by, for example, adding the following line
        to xtsc-run.inc:
           --set_xtsc_parm=full_hierarchical_names=true

     2) In this example, you should also override the SimTargetOutput parameter (this cannot
        be done from xtsc-run.inc).  For example:
        Cadence tool chain:
          ncsim +dumpvars -licqueue +preloadmems Testbench \
            +systemc_args+-full_hierarchical_names=true \
            +systemc_args+-Testbench.xtensa1.core0.core0.SimTargetOutput=core1_output.log \
            +systemc_args+-xxdebug
        Mentor tool chain:
          vsim -c -do "run -all; quit" +dumpvars +preloadmems  Testbench \
            -sc_arg -full_hierarchical_names=true \
            -sc_arg -/Testbench/xtensa1/core0/core0.SimTargetOutput=core1_output.log \
            -sc_arg --xxdebug
        Synopsys tool chain:
          ./simv +dumpvars +vcs+lic+wait -q +preloadmems -systemcrun \
            -full_hierarchical_names=true \
            -Testbench_xtensa1_core0.core0.SimTargetOutput=core1_output.log \
            -xxdebug

     3) If each core needs to run a different program then you should override the
        SimTargetProgram xtsc_core_parms parameter for the second and subsequent cores.
        This can be done similarly to the SimTargetOutput parameter shown above.

     4) If using a DSO, you will also need to override the dso_cookie parameter for each
        xtsc_tlm2pin_memory_transactor used in the xtensa1 sub-system.  For example for
        the tlm2pin_pif instance add:
        Cadence tool chain:
            +systemc_args+-Testbench.xtensa1.core0.tlm2pin_pif.dso_cookie=Testbench.xtensa1.pif
        Mentor tool chain:
            -sc_arg -/Testbench/xtensa1/core0/tlm2pin_pif.dso_cookie=Testbench.xtensa1.pif
        Synopsys tool chain:
            -Testbench_xtensa1_core0.tlm2pin_pif.dso_cookie=Testbench.xtensa1.pif

     See xtsc_initialize_parms "full_hierarchical_names" and xtsc_core_parms "SimNameForISS"
     in xtsc_rm.pdf.  Also see the sections on "Multi-core SystemC-Verilog Co-simulation",
     "Passing Command Line Arguments to extract_parms() in SystemC-Verilog Co-simulation",
     and "Debugging xtsc_core Target Programs in SystemC-Verilog Co-simulation" in xtsc_ug.pdf.
  */

//Subsystem xtensa1();

endmodule



